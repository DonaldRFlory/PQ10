//So.. planning a firing sequence which pulses successive channels on for 20 msec and runs through
//all channels with a connected cue taking about a second for all cues. Actually, hardly need
//to have access to continuity data, could just fire everything. Might want a slower mode
//which fired once every second or two for fireball sequencing. This could take 48 seconds.
// For slower sequence, skip some cues when wiring.
// Load Wi-Fi library

#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include "type.h"
#include "slink.h"
void TraceBytes(U8 * Ptr, int Count);
void TraceByte(U8 Value);
void WriteTraceBuffer(char* InBuff);

extern bool DoLinkReceive();
bool InitOffset();
bool InitOff2();
void EndContinuityScan();
void InitContinuityScan();
bool FindNextFiringCue();
void ClearSR();
void FireCue(int CueIdx);

void DoTests();
void IdleServe();
void PWMServe();
#define SERIAL_RX_SIZE  300
#define SERIAL_TX_SIZE  300

#define USE_ACCESS_POINT 1
#ifdef USE_ACCESS_POINT
    // Replace with your network credentials
    const char* ssid1 = "PQ10-Access_Point 1";
    const char* ssid2 = "PQ10-Access_Point 2";
    const char* password = "123456789";
#else //using wi-fi router
    // Replace with your network credentials
    const char* ssid = "HOME-31AD-2.4";
    const char* password = "award7773energy";
#endif

const int NOM_OFFSET_X10 =  1000;
const int MAX_CONT_READING = 750;
const int CONT_OPEN_READING = 2000;
//Rough calibration of A/D for resistance measurement:
const float CountsPerOhm    = 12.0;
//const float OffsetCounts    = 10.75;
const float OffsetCounts    = 110.7;

//Constants for various things:
const int NUM_CUES = 48;
const int NUM_SR_BITS = 56;
const int FIRE_MSEC = 20;
const int FIRE_INTERVAL_MSEC = 4000;


// Assign output variables to GPIO pins
const int output  = 5;
const int output26 = 26;
const int output27 = 27;

const int8_t SENSEA = 36;
const int8_t SENSEB = 39;
const int8_t SENSEC = 34;
const int8_t TELL_TALE_1 = 22;
const int8_t TELL_TALE_2 = 19;
const int8_t TELL_TALE_3 = 18;
const int8_t NOT_SR_STROBE = 12;
const int8_t BD_AB_SEL = 15;//if open, selects Board A, if grounded, selects Board B
const int8_t SPI_CLK = 2;
const int8_t SPI_MOSI = 14;
const int8_t SPI_MISO = 21;
const int8_t KEEP_ALIVE = 0;

const int ATTR_CTL = 1;  //control bit in SR unchanged by continuity scan
const int ATTR_CUE = 2;  //a cue control bit
const int ATTR_CTLB = 4; //a control button, not reflected in SR

//const int BT_0 = 0; //button type 0 off/dark-gray
//const int BT_1 = 1; //button type Green
//const int BT_2 = 2; //Dark blue
//const int BT_3 = 3; //yellow

const int CMD_OFF = 0;
const int CMD_ON = 1;
const int CMD_CLEAR = 2;
const int CMD_CONT = 3;
const int CMD_REFRESH = 4;
const int CMD_PWM1 = 5;
const int CMD_PWM2 = 6;
const int NUM_COMMANDS = 8; //keep this last and keep CmdStrs in line with cmd codes
//so... below are strings put in href strings for buttons. The first
//two are used only with first 56 buttons corresponding to SR bits,
//depending on state of cue.(actually the command to toggle to opposite state)
//The last five are used with the five command butons after the array
//of SR mapped buttons
char *CmdStrs[NUM_COMMANDS] =
{
    "off",
    "on",
    "clear",
    "cont",
    "pwm1",
    "pwm2",
    "debug",
    "fire",
};
const int NUM_CMD_BUTTONS = 6;
const int NUM_BUTTONS = NUM_SR_BITS + NUM_CMD_BUTTONS;

//following five defines put command button indices right after SR button indices
const int CMD_CLEAR_IDX = NUM_SR_BITS + 0;
const int CMD_CONT_IDX = NUM_SR_BITS + 1;
const int CMD_PWM1_IDX = NUM_SR_BITS + 2;
const int CMD_PWM2_IDX = NUM_SR_BITS + 3;
const int CMD_DBG_IDX = NUM_SR_BITS + 4;
const int CMD_FIRE_IDX = NUM_SR_BITS + 5;


//overall state of controller
//Background operations are done via IdleServe(), called by loop() and at various
//points in serving up web pages within loop().
//Some states (ST_CONT_DISP for example) have no background operation but
//effect appearance and/or state of buttons on web page.
const int ST_IDLE = 0;
const int ST_CONT_SCAN = 1;
const int ST_CONT_DONE = 2;
const int ST_CONT_DISP = 3;
const int ST_DEBUG = 4;
const int ST_FIRE = 5;
const int ST_FIRE_INTERVAL = 6;

//Defs for SR bits
const uint8_t TEST_CUR_BIT = 29;
const uint8_t HIGH_DRIVE_BIT = 31;
const uint8_t CTRL_BIT1 = 25;//bit after LED2 for debug control
const uint8_t LED2_BIT = 24;
const uint8_t LED1_BIT = 30;

uint8_t PWMState = 0;
const uint8_t PWM_IDLE = 0;
const uint8_t PWM_ONE = 1;
const uint8_t PWM_TWO = 2;

uint8_t CtrlState = ST_IDLE;
int CSCue, CSStep, OffsetIndex;
int FireCounter, FireCueIdx;
bool OffsetFlag;
long CSAccumulator;
long AccSenseA, AccSenseB;


SPIClass * vspi = NULL;
// Set web server port number to 80
WiFiServer server(80);

//-------------------------Millisecond timer-----------------------------
//#include <timer.h>
volatile int MsecTicks;  //incremented each millisecond by timer ISR
hw_timer_t * MsecTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

bool TickToggle;
//The timer ISR
void IRAM_ATTR onMsecTimer()
{
    portENTER_CRITICAL_ISR(&timerMux);
    ++MsecTicks;
    portEXIT_CRITICAL_ISR(&timerMux);
    if(TickToggle)
    {
        digitalWrite(KEEP_ALIVE, HIGH);
    }
    else
    {
        digitalWrite(KEEP_ALIVE, LOW);
    }
    TickToggle = !TickToggle;
}

void InitTimer()
{
    MsecTimer = timerBegin(0, 80, true); //The timer will use 1 microsecond clock after prescale
    timerAttachInterrupt(MsecTimer, &onMsecTimer, true);
    timerAlarmWrite(MsecTimer, 1000, true);
    timerAlarmEnable(MsecTimer);
}
//----------------------End Millisecond timer----------------------------



// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const int NUM_BS_BYTES = 8;
uint8_t  BSBytes[NUM_BS_BYTES];//Button state bytes
uint8_t  SRBytes[NUM_BS_BYTES];//actually only seven are used
uint8_t  SRPWMClrBytes[NUM_BS_BYTES];//actually only seven are used
uint8_t SPIDataRcv[NUM_BS_BYTES];
char StringBuff[100];

#define LINK_ACTIVE true
#ifdef LINK_ACTIVE
#define Trace(A)
#define Traceln(A)
#else
#define Trace(A) Serial.print(A);
#define Traceln(A) Serial.println(A);
#endif

void setup()
{
  Serial1.begin(115200, SERIAL_8N1, 27, 23); // use UART1
  Serial.begin(115200);  //and UART0 on default pins
  Serial.setRxBufferSize(SERIAL_RX_SIZE);
  //Serial.SetTxBufferSize(SERIAL_TX_SIZE);
  pinMode(TELL_TALE_1, OUTPUT);
  pinMode(TELL_TALE_2, OUTPUT);
  pinMode(TELL_TALE_3, OUTPUT);
  pinMode(NOT_SR_STROBE, OUTPUT);
  pinMode(KEEP_ALIVE, OUTPUT);
  pinMode(BD_AB_SEL, INPUT_PULLUP);

  digitalWrite(TELL_TALE_1, LOW);
  digitalWrite(TELL_TALE_2, LOW);
  digitalWrite(TELL_TALE_3, LOW);
  digitalWrite(NOT_SR_STROBE, HIGH);
  digitalWrite(KEEP_ALIVE, LOW);

  // Connect to Wi-Fi network
  #ifdef USE_ACCESS_POINT
    Trace("Setting AP (Access Point)... ");

    if(digitalRead(BD_AB_SEL))
    {
        WiFi.softAP(ssid2, password);
    }
    else
    {
        WiFi.softAP(ssid1, password);
    }

    IPAddress IP = WiFi.softAPIP();
    Trace("AP IP address: ");
    Traceln(IP);
    server.begin();

  #else
      // Connect to Wi-Fi network with SSID and password
      Trace("Connecting to ");
      Traceln(ssid);
      Traceln(password);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Trace(".");
      }
      // Print local IP address and start web server
      Traceln("");
      Traceln("WiFi connected.");
      Traceln("IP address: ");
      Traceln(WiFi.localIP());
      server.begin();
  #endif

  //Setup SPI for 48 queue pyro control (56 bit S/R)
  vspi = new SPIClass(VSPI);
  vspi->begin(SPI_CLK, SPI_MISO, SPI_MOSI, -1);  //not using default pins
  InitTimer();
  WriteTraceBuffer("Test line 1 for Trace Buffer\n");
  WriteTraceBuffer("Test line 2 for Trace Buffer\n");
  WriteTraceBuffer("Test line 3 for Trace Buffer\n");
  WriteTraceBuffer("Test line 4 for Trace Buffer\n");

  char *StringBuff;
  //DoTests();   //Tests HTML response decoding functions and sends out byte array for
                //each test on vspi.
}


//Continuity scan values times 10
int CSOffsetValsX10[2];
int CSValsX10[NUM_CUES] =
{
    10, 11, 12, 13, 14, 15, 16, 17,
    20, 31, 42, 53, 064, 75, 86, 97,
    110, 121, 132, 143, 154, 165, 176, 187,
    190, 201, 212, 223, 234, 245, 256, 267,
    650, 661, 672, 683, 694, 705, 716, 727,
    900, 911, 922, 933, 944, 955, 966, 977,
};

//Shift register mapped button attributes
//All SRBits with ATTR_CTRL, are unmodified during continuity scan
uint8_t BAttrs[NUM_BUTTONS] = //button attributes, and for first 56, also SR attributes
{
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CTL, ATTR_CTL, ATTR_CTL, ATTR_CTL, ATTR_CTL,  ATTR_CTL, ATTR_CTL, ATTR_CTL,
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE, ATTR_CUE,  ATTR_CUE, ATTR_CUE, ATTR_CUE,
    ATTR_CTLB, ATTR_CTLB, ATTR_CTLB, ATTR_CTLB, ATTR_CTLB,  ATTR_CTLB
};


char *BStrs[NUM_BUTTONS] =
{
    "000", "001", "002", "003", "004", "005", "006", "007",
    "008", "009", "010", "011", "012", "013", "014", "015",
    "016", "017", "018", "019", "020", "021", "022", "023",
    "LD2", "CT1", "---", "---", "---", "TST", "LD1", "HDV",
    "024", "025", "026", "027", "028", "029", "030", "031",
    "032", "033", "034", "035", "036", "037", "038", "039",
    "040", "041", "042", "043", "044", "045", "046", "047",
    "CLR", "OHM", "PW1", "PW2", "DBG", "FIR"
};

bool IsBSBitSet(int BitNo)
{
    if(BitNo >= NUM_BUTTONS)
    {
        return false;
    }
    return (BSBytes[BitNo >> 3] &  (1 << (BitNo & 7))) != 0;
}

void SetSRBit(uint8_t * SRBytes, int BitNo)
{
    if(BitNo >= NUM_SR_BITS)
    {
        BitNo = NUM_SR_BITS -1;
    }
    SRBytes[BitNo >> 3] |=  (1 << (BitNo & 7));
}

void ClrSRBit(uint8_t * SRBytes, int BitNo)
{
    if(BitNo >= NUM_SR_BITS)
    {
        BitNo = NUM_SR_BITS - 1;
    }
    SRBytes[BitNo >> 3] &=  ~(1 << (BitNo & 0X7));
}

//no error checking of input range!!
int CueToSRIdx(int CueIdx)
{
    if(CueIdx > 23)
    {
        return CueIdx + 8;
    }
    return CueIdx;
}

void Load56()
{
    digitalWrite(NOT_SR_STROBE, LOW);
    digitalWrite(NOT_SR_STROBE, HIGH);
}

void Send56()
{
    vspi->beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
    digitalWrite(TELL_TALE_2, HIGH);
    vspi->transferBytes(SRBytes, SPIDataRcv, 7);
    vspi->endTransaction();
    digitalWrite(TELL_TALE_2, LOW);
}

//Here we process Header sent back by client when button was pressed by user
bool GetCommandAndIndex(String &Header, int & Index, int & Command)
{
    bool Found = false;
    int i, j;
    char IdxStr[3];
    char CommandString[100];

    for(i = 0; i < NUM_COMMANDS; ++i)
    {
        sprintf(CommandString, "GET /%s/", CmdStrs[i]);
        if((j = Header.indexOf(CommandString)) >= 0)
        {
            j += strlen(CommandString); //skip index over the Command to start of button number
            Command = i;
            Found = true;
            break;
        }
    }
    if(!Found)
    {
        return false;
    }

    IdxStr[0] = Header.charAt(j++);
    IdxStr[1] = Header.charAt(j);
    IdxStr[2] = 0;
    Index = atoi(IdxStr);
    if(Index >= NUM_BUTTONS)
    {
        return false;
    }
    return true;
}

//We will do 20 counts per channel with five counts for settling
//So far, mainly for continuity scan
void StateServe()
{
    float Reading;
    int ADPin;
    switch(CtrlState)
    {
        case ST_CONT_SCAN:
            ADPin = (CSCue < 24) ? SENSEA : SENSEB;
            if(CSStep < 10)//first ten steps are settling delay
            {
                CSAccumulator = 0;//just keep clearing it
                ++CSStep;
                return;//wait for settling
            }
            for(int i = 0; i < 10; ++i)
            {
                CSAccumulator += analogRead(ADPin); //take ten readings
            }
            //for first reading on step 10, we will fake completion of averaging for open connections
            //to speed the process
            if(CSStep == 10)
            {
                if(CSAccumulator >= (10 * MAX_CONT_READING))
                {
                    CSAccumulator = 500l * CONT_OPEN_READING;
                    CSStep = 60;
                }
            }


            if(CSStep >= 60)
            { //done with this cue
                 ClrSRBit(SRBytes, CueToSRIdx(CSCue)); //clear the just read cue's low drive
                //So.. normally the cue tells us where we are in the scan,
                //but if doing offset, it just tells us what cue we used for offset
                //reading. The scan is in fact completed and this is follow up work
                if(OffsetFlag)
                {
                    if(OffsetIndex)
                    {
                        //second offset reading
                        CSOffsetValsX10[1] = (int)(CSAccumulator/50);
                        EndContinuityScan();//changes CtrlState
                     }
                    else
                    {
                        //first offset reading
                        CSOffsetValsX10[0] = (int)(CSAccumulator/50);
                        if(!InitOff2())
                        {
                            EndContinuityScan();
                        }
                    }
                }
                else
                {
                    CSValsX10[CSCue] = (int)(CSAccumulator/50);
                    if(CSCue >= (NUM_CUES - 1))
                    { //Done with main scan
                        if(!InitOffset())
                        {
                            EndContinuityScan();
                        }
                    }
                    else
                    {
                        CSStep = 0;
                        ++CSCue;
                        SetSRBit(SRBytes, CueToSRIdx(CSCue));//turn on low drive of next cue
                    }
                }
                Send56();//send out new state to SR
                Load56();
            }
            ++CSStep;
            break;

        case ST_FIRE:
            if(FireCounter)
            {
                --FireCounter;
                return;
            }
            ClearSR(); //turn all SR bits off
            CtrlState = ST_FIRE_INTERVAL;//and go back to Fire Interval state
            FireCounter = FIRE_INTERVAL_MSEC;
            break;

        case ST_FIRE_INTERVAL:
            if(FireCounter)
            {
                --FireCounter;
                return;
            }
            if(FindNextFiringCue())
            {
                FireCue(FireCueIdx, FIRE_MSEC);
                CtrlState = ST_FIRE;//and go back to Fire state
            }
            else
            {
                ClearSR(); //turn all SR bits off
                CtrlState = ST_IDLE;//and go back to Idle state
            }
            break;


        default:            //when not doing state related activity like continuity scan,
                            //we read ADC0, ADC3 and ADC6 (SENSEA-SENSE3) each once a millisecond
            AccSenseA += analogRead(SENSEA);
            AccSenseB += analogRead(SENSEB);
            ++CSStep;
            if(CSStep >= 5000)
            {
                //print the average of readings over last 5 seconds
                sprintf(StringBuff, "%d, %d", (int)(AccSenseA/5000), (int)(AccSenseB/5000));
                Traceln(StringBuff);
                //and reset
                CSStep = 0;
                AccSenseA = AccSenseB = 0;
            }
            break;
    }
}

//returns true if valid in-range channel was found and setup for offset reading
bool InitOffset()
{
    int MinVal = 10 * MAX_CONT_READING;
    CSCue = -1;
    CSStep = 0;
    CSOffsetValsX10[0] = NOM_OFFSET_X10; //just in case, set a norminal value
    CSOffsetValsX10[1] = NOM_OFFSET_X10; //just in case, set a norminal value
    ClrSRBit(SRBytes, TEST_CUR_BIT);//test current off

    for(int i = 0; i < (NUM_CUES / 2); ++i)
    {
        if(CSValsX10[i] < MinVal)
        {
            CSCue = i;
            MinVal = CSValsX10[i];
        }

    }
    if(CSCue >= 0)
    {
        SetSRBit(SRBytes, CueToSRIdx(CSCue));
        OffsetFlag = true;
        OffsetIndex = 0;
        //Traceln("InitOffset succeeded");
        return true;
    }
    OffsetFlag = false;
    //no channels have a connection in group zero
    return InitOff2();
}

//returns true if valid in-range channel was found and setup for offset reading
bool InitOff2()
{
    int MinVal = 10 * MAX_CONT_READING;
    CSStep = 0;
    CSCue = -1;
    for(int i = NUM_CUES / 2; i < NUM_CUES; ++i)
    {
        if(CSValsX10[i] < MinVal)
        {
            CSCue = i;
            MinVal = CSValsX10[i];
        }
    }
    if(CSCue >= 0)
    {
        SetSRBit(SRBytes, CueToSRIdx(CSCue));
        OffsetFlag = true;
        OffsetIndex = 1;
        //Traceln("InitOff2 succeeded");
        return true;
    }
    OffsetFlag = false;
    return false;
}

void EndContinuityScan()
{
    AccSenseA = AccSenseB = 0;
    ClrSRBit(BSBytes, TEST_CUR_BIT);//test current off
    CtrlState = ST_CONT_DONE;
    Traceln("Continuity Scan Complete\r\n");
    sprintf(StringBuff, "Offsets: %d, %d", CSOffsetValsX10[0], CSOffsetValsX10[1]);
    Traceln(StringBuff);
}

bool FindNextFiringCue()
{
    //At start of firing, FireCueIdx is set to -1.
    //later on in sequence it is cue just fired.
    for(int i = FireCueIdx + 1; i < NUM_CUES; ++i)
    {
        if(IsBSBitSet(CueToSRIdx(i)))
        {
            FireCueIdx = i;
            return true;
        }
    }
    return false;
}

void ClearSRBytes()
{
    for(int i = 0; i < NUM_BS_BYTES; ++i)
    {
        SRBytes[i] = 0; //clear SR bytes
    }
}

void LoadSR()
{
    Send56();
    Load56();
}

void ClearSR()
{
    ClearSRBytes();
    LoadSR();
}



void FireCue(int CueIndex, int FireMsec)
{
    sprintf(StringBuff, "Firing Cue: %d", CueIndex);
    Traceln(StringBuff);
    ClearSRBytes();
    SetSRBit(SRBytes,CueToSRIdx(CueIndex));
    SetSRBit(SRBytes,HIGH_DRIVE_BIT);
    SetSRBit(SRBytes,LED1_BIT);//visual indication of firing state
    LoadSR();
    FireCounter = FireMsec;
}

void InitFiring()
{
    if(CtrlState == ST_IDLE)
    {
        FireCueIdx = -1;
        if(FindNextFiringCue())
        {
            CtrlState = ST_FIRE;
            FireCue(FireCueIdx, FIRE_MSEC);//fires and sets timer
        }
    }
}

void InitContinuityScan()
{
    CtrlState = ST_CONT_SCAN;

    Traceln("InitContinuityScan()\r\n");
    for(int i = 0; i < NUM_SR_BITS; ++i)
    {
        ClrSRBit(BSBytes, i);//clear all BS bits   (in the SR bit range)
    }
    CSStep = 0;
    CSCue = 0;
    OffsetFlag = false;
    OffsetIndex = 0;
    //Setup to measure cue 0
    SetSRBit(SRBytes, TEST_CUR_BIT);//test current on
    SetSRBit(SRBytes, CueToSRIdx(0));//Set first cue low drive
    Send56();
    Load56();
}

//state to ST_IDLE.
// in ST_IDLE, we process all the ATTR_CTL bits and allow change both BSBytes and SRBytes. We always load SRBytes to driver
//board whatever the command.
//in ST_IDLE , we only modify BSBytes for CUES.
//In other states, we do not process CUE commands.
//So in other states, we do not want anyone messing with HiDrive, or Test.
//Mainly it is a problem if anyone were to mess with HiDrive or test when CONT scan is going on
//or results being displayed.

//PWM1, PWM2, REFRESH, CONT, FIRE, DEBUG are only active during ST_IDLE
//We always honor CLEAR command and clear all bits in BSBytes and SRBytes, also change
//Think PWMx commands will act like FIRE command but with PWM
 void ProcessCommand(String & Header) //Set or clear bits in the seven byte array for PQ driver board
{
    bool Shift = false;
    bool DbgFlag = false;
    int Index = -1, Command = -1;
    if(GetCommandAndIndex(Header, Index, Command))
    {
        //Now we are ignoring the Command and just considering them all button presses
        //We decide what to do based on the Index and our state info on the ESP32
        //So.. for all Cue buttons
        //    We always check corresponding bit in BSBytes and if the bit is On
        //    we consider it an OFF command and if it is On as an OFF command.
        //    We toggle the BSByte in other words.
        //    In ST_DEBUG, we load the BSByte values into the SR.
        //    If not in ST_DEBUG, we want all cue bits to be zero.
        //    Again if not in ST_DEBUG, we

        if(Index == LED1_BIT)  //this is a temporary debugging kludge
        {
            DbgFlag = true;
        }

        if(CtrlState == ST_DEBUG)
        {
            Shift = true;
        }
         //So... Am thinking that only in debug state do we automatically shift out SR bits.

        switch(BAttrs[Index])
        {
            case ATTR_CUE:
                if(IsBSBitSet(Index))
                {
                    sprintf(StringBuff, "Cmd: off, %d", Index);
                    ClrSRBit(BSBytes, Index);
                }
                else
                {
                    sprintf(StringBuff, "Cmd: on, %d", Index);
                    SetSRBit(BSBytes, Index);
                }
                break;

            case ATTR_CTL:
                if(CtrlState == ST_DEBUG)
                {
                    if(IsBSBitSet(Index))
                    {
                        sprintf(StringBuff, "CtlCmd: off, %d", Index);
                        ClrSRBit(BSBytes, Index);
                    }
                    else if(CtrlState == ST_DEBUG)
                    {
                        sprintf(StringBuff, "CtlCmd: on, %d", Index);
                        SetSRBit(BSBytes, Index);
                    }
                    Shift = true;
                }
                break;

            case ATTR_CTLB:
                switch(Index)
                {
                    case CMD_CLEAR_IDX:
                        sprintf(StringBuff, "Cmd: clear, %d", Index);
                        for(int i = 0; i < NUM_BS_BYTES; ++i)
                        {
                            BSBytes[i] = 0;
                        }
                        CtrlState = ST_IDLE;
                        Shift = true;
                        break;

                    case CMD_CONT_IDX:
                        sprintf(StringBuff, "Cmd: cont, %d", Index);
                        if(CtrlState == ST_IDLE)
                        {
                            InitContinuityScan();//the shift from SRBits, happens within InitContinuityScan()
                        }
                        else if(CtrlState == ST_CONT_DONE)
                        {
                            CtrlState = ST_CONT_DISP;
                        }
                        else if(CtrlState == ST_CONT_DISP)
                        {
                            CtrlState = ST_IDLE;
                        }
                        break;

                    case CMD_PWM1_IDX:
                        break;

                    case CMD_PWM2_IDX:
                        break;

                    case CMD_DBG_IDX:
                        sprintf(StringBuff, "Cmd: debug, %d", Index);
                        if(CtrlState == ST_DEBUG)
                        {
                            CtrlState = ST_IDLE;
                        }
                        else
                        {
                            CtrlState = ST_DEBUG;
                        }
                        break;

                    case CMD_FIRE_IDX:
                        sprintf(StringBuff, "Cmd: fire, %d", Index);
                        InitFiring();//if active cue is found, shift is done by InitFiring()
                        StringBuff[0] = 0;//so we do not print the same fire message again below
                        //state is changed and StateServe is responsible for turning
                            //it off and sequencing to next active cue or to ST_IDLE
                        break;
                }
        }

        if(Shift)
        {
            for(int i = 0; i < NUM_BS_BYTES; ++i)
            {
                SRBytes[i] = BSBytes[i];
            }
            Send56();
            Load56();
        }
    }
    else
    {
        sprintf(StringBuff, "Cmd: None");
    }
    Traceln(StringBuff);
    if(DbgFlag)
    {
        sprintf(StringBuff, "Offsets: %d, %d", CSOffsetValsX10[0], CSOffsetValsX10[1]);
        Traceln(StringBuff);
        sprintf(StringBuff, "A: %d, %d, %d, %d", CSValsX10[0], CSValsX10[1], CSValsX10[2], CSValsX10[3]);
        Traceln(StringBuff);
        sprintf(StringBuff, "B: %d, %d, %d, %d", CSValsX10[24], CSValsX10[25], CSValsX10[26], CSValsX10[27]);
        Traceln(StringBuff);
    }
}


void DoTest(String &TestString)
{
    bool Result;
    int Index = -1, State = -1;

    Serial.write("DoTest(): \"");
//    Serial.write(Header.c_str());
    Serial.write("\"\r\n");
    Result = GetCommandAndIndex(TestString, Index, State);
    if(State == 1)
    {
        for(int i = 0; i < 8; ++i)
        {
            SRBytes[i] = 0X00;
        }
        SetSRBit(SRBytes, Index);
        sprintf(StringBuff, "OK: %d, %d\r\n", Index, State);
    }
    else if(State == 0)
    {
        for(int i = 0; i < 8; ++i)
        {
            SRBytes[i] = 0XFF;
        }
        ClrSRBit(SRBytes, Index);
        sprintf(StringBuff, "OK: %d, %d\r\n", Index, State);
    }
    else
    {
        sprintf(StringBuff, "OK: %d, %d\r\n", Index, State);
    }
    Serial.write(StringBuff);
    Serial.write("\r\n");
    if((State == 0) || (State == 1))
    {
        sprintf(StringBuff, "SrBytes = %2X, %2X, %2X, %2X, %2X, %2X, %2X, %2X\r\n", SRBytes[0],  SRBytes[1],  SRBytes[2],  SRBytes[3],  SRBytes[4],  SRBytes[5],  SRBytes[6],  SRBytes[7] );
    }
    Serial.write(StringBuff);
    Serial.write("\r\n");
    Send56();//send bytes out on SPI
}

void GenResistanceString(char * RString, int ButtonIdx)
{
    int CueIndex;
    int CurVal, Offset;
    //Two offset values for the two A/D channels used for two High Drive lines
    if(ButtonIdx < NUM_CUES / 2)
    {
        CueIndex = ButtonIdx;
        Offset = CSOffsetValsX10[0];
    }
    else
    {
        CueIndex = ButtonIdx - 8;  //account for ctrl buttons in middle of SR
        Offset = CSOffsetValsX10[1];
    }

    if(CSValsX10[CueIndex] < (10 * MAX_CONT_READING))
    {
        CurVal = (CSValsX10[CueIndex] - Offset)/10;
        CurVal = CurVal < 0 ? 0 : CurVal;
        sprintf(RString, "%2d.%d", CurVal/10, CurVal%10);
    }
    else
    {
        sprintf(RString, "X");
    }
}

//Note: all buttons corresponding to SR bits toggle buttons. Green indicates ON. Pressing button changes state
//There is a line of command buttons following the SR mapped buttons. Only the CONT button changes state. Others
//stay the same and produce execution of their associated command when pressed.
void GenButton(String &Text, int ButtonIdx, bool On)
{
    int CueIndex = -1;
    uint8_t Type;
    char RString[10];
    int CmdIdx;
    char * Cmd;
    char * ButtonString;
    char * ButtonClass;
    bool IsCue;
    char WorkStr[100];
    int AttrBits;
    if(ButtonIdx >= NUM_BUTTONS)
    {
        return;
    }

    Cmd = CmdStrs[CMD_ON]; //now this is always the command. We decide what to do based on ButtonIdx and state
    AttrBits = BAttrs[ButtonIdx];
    ButtonString = BStrs[ButtonIdx];//so far doesn't change due to attrib or state
              //except for cues in ST_CONT_DISP

    switch(AttrBits)
    {
        case ATTR_CUE:  //in Cont Display state, blue, otherwise gray for inactive and green for active
            switch(CtrlState)
            {
                default:
                    ButtonClass = "button"; //gray
                    break;

                case ST_DEBUG:
                case ST_IDLE:
                    if(IsBSBitSet(ButtonIdx))
                    {
                        ButtonClass = "button button4";//red
                    }
                    else
                    {
                        ButtonClass = "button button1";//green
                    }
                    break;

                case ST_CONT_DISP:
                    ButtonClass = "button button2";//blue
                    GenResistanceString(RString, ButtonIdx);
                    ButtonString = RString;
                    break;
            }
            break;

        case ATTR_CTL:
            switch(CtrlState)
            {
                default:
                    ButtonClass = "button"; //gray
                    break;

                case ST_DEBUG:
                    if(IsBSBitSet(ButtonIdx))
                    {
                        ButtonClass = "button button4";//red
                    }
                    else
                    {
                        ButtonClass = "button button1";//green
                    }
                    break;
            }
            break;

        case ATTR_CTLB:
            switch(ButtonIdx)
            {
                case CMD_CONT_IDX:
                    switch(CtrlState)
                    {
                        default:
                        case ST_IDLE:
                            ButtonClass = "button button3";//yellow
                            break;

                        case ST_CONT_SCAN:
                            ButtonClass = "button button2";//blue, active
                            break;

                        case ST_CONT_DISP:
                            ButtonClass = "button button4";//red, displaying
                            break;

                        case ST_DEBUG:
                            ButtonClass = "button"; //gray, inactive
                        break;
                    }
                    break;

                case CMD_DBG_IDX:
                    switch(CtrlState)
                    {
                        default:
                            ButtonClass = "button button3";//yellow, can activate debug
                            break;

                        case ST_DEBUG:
                            ButtonClass = "button button4";//red, active, pressing button will deactivate
                            break;
                    }
                    break;

                case CMD_CLEAR_IDX:  //in any state, it is active
                    ButtonClass = "button button3";//yellow, can activate in any state
                    break;

                default:
                    switch(CtrlState)
                    {
                        case ST_IDLE:
                            ButtonClass = "button button3";//yellow
                            break;

                        default:
                            ButtonClass = "button"; //gray, inactive
                            break;
                    }
                    break;
            } //end of switch(ButtonIdx)
            break;

        default:
            break;

    }//end of switch(AttrBits)

    //new code to generate buttons in one place
    sprintf(WorkStr, "<a href=\"/%s/%02d\"><button class=\"%s\">%s</button></a>", Cmd, ButtonIdx, ButtonClass, ButtonString);
    Text += WorkStr;

}

void GenerateButtons(String &PageStr)
{
    int i = 0;
    //Traceln("GenerateButtons\r\n");
    // All 56 ON/OFF buttons
    PageStr += "<p>"; //start of button line  (paragraph)
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i  >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    PageStr += "<p>"; //start of button line
    for(int j = 0; j < 8; ++j)
    {
        GenButton(PageStr, i, (SRBytes[i >> 3] & (1<<j)) != 0);
        ++i;
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    // End of 56 ON/OFF SR bit mapped buttons

    PageStr += "<p>"; //start of button line
    //Now we do the command button line. These buttons do not correspond to
    //SR bits as all above button lines do.
    //currently all the commands buttons must fit on a single line
    for(i = 0; i < NUM_CMD_BUTTONS; ++i)
    {
        GenButton(PageStr, NUM_SR_BITS + i, false);
    }
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    //Traceln(PageStr.c_str());
}

// Variable to store the HTTP request
String header;

String PageStr= "";  // String to accumulate pieces of webpage as we generate it
void loop(){
  IdleServe();

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
     currentTime = millis();
     IdleServe();
    previousTime = currentTime;
    Traceln("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {  // loop while the client's connected
      IdleServe();
      currentTime = millis();
      if (client.available())
      {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n')
        {                    // if the byte is a newline character
          digitalWrite(TELL_TALE_2, HIGH);
          IdleServe();
          digitalWrite(TELL_TALE_2, LOW);
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            PageStr =  "HTTP/1.1 200 OK\r\n";
            PageStr += "Content-type:text/html\r\n";
            PageStr += "Connection: close\r\n\n";
            client.println(PageStr); //and send it out to client
            //Traceln(PageStr.c_str());
            IdleServe();

            ProcessCommand(header); //Set or clear bits in the seven byte array for PQ driver board

            // Display the HTML web page
            PageStr = "<!DOCTYPE html><html>\r\n";
            PageStr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
            PageStr += "<link rel=\"icon\" href=\"data:,\">\r\n";

            //basic button is medium dark gray and is used to indicate an inactive cue
            //button1 is Green and indicates a cue which is active or on
            //button2 is dark blue indicates cue in resitance display mode and Test in active mode
            //button3 is Yellow and indicates a control button
            //button4 is Red and indicates a Dbg button when in Dbg state
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            PageStr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\r\n";
            PageStr += ".button { background-color: #555555; border: none; color: white; padding: 8px 8px; width: 40px;\r\n"; //medium dark gray
            PageStr += "text-decoration: none; font-size: 12px; margin: 2px; cursor: pointer;}\r\n";
            PageStr += ".button1 {background-color: #4CAF50;\r\n}"; //green
            PageStr += ".button2 {background-color: #025ADC;}";  //dark blue
            PageStr += ".button3 {background-color: #AAAA00;}";  //yellow
            PageStr += ".button4 {background-color: #FF0000;}";  //Red
            PageStr += "</style></head>";
            client.println(PageStr);
//            Traceln(PageStr.c_str());
            IdleServe();

            // Web Page Heading
            PageStr = "<body><h1>ESP32 Web Server</h1>\r\n";

            PageStr += "<p>GPIO 26 - State </p>\r\n";
            GenerateButtons(PageStr);
            client.println(PageStr);
            //Traceln(PageStr.c_str());
            IdleServe();
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } //end of if current line == 0
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {  // if you got anything else but a carriage return character,
          IdleServe();
          currentLine += c;      // add it to the end of the currentLine
        } //end of if char is newline
      } //end of if client has a character
    } //end of while client is connnected
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Traceln("Client disconnected.");
    Traceln("");
    PWMServe();
    }
}

//This function blocks for about 30 msec to PWM the low end driver for Cue firing
void PWMServe()
{
    int OnCount, OffCount, i;
    if(PWMState == PWM_IDLE)
    {
        return;
    }
    else if(PWMState == PWM_ONE)
    {
        OnCount = 6;
        OffCount = 40;
    }
    else
    {
        OnCount = 6;
        OffCount = 20;
    }
    SetSRBit(SRBytes, HIGH_DRIVE_BIT); //set high drive bit in SRBytes
    for(i = 0; i < 8; ++i)
    { //copy state of SRBytes into the Clr set
        SRPWMClrBytes[i] = SRBytes[i];
    }
    for(i = 0; i < NUM_SR_BITS; ++i)
    {
        if(BAttrs[i] & ATTR_CUE)
        {
            ClrSRBit(SRPWMClrBytes, i); //clear all the CUE bits
        }
    }
    portENTER_CRITICAL(&timerMux);
    MsecTicks = 0; //clear the msec interrupt timer
    portEXIT_CRITICAL(&timerMux);

    vspi->beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));

    vspi->transferBytes(SRBytes, SPIDataRcv, 7); //Load the initial active state
    while(MsecTicks < 30)
    {
        digitalWrite(NOT_SR_STROBE, LOW);//Transfer active state to outputs
        digitalWrite(NOT_SR_STROBE, HIGH);
        for(i = 0; i < OnCount; ++i)
        {
            vspi->transferBytes(SRPWMClrBytes, SPIDataRcv, 7); //Load the off state for timing
        }
        digitalWrite(NOT_SR_STROBE, LOW);//Transfer low stateto outputs
        digitalWrite(NOT_SR_STROBE, HIGH);
        for(i = 0; i < OffCount; ++i)
        {
            vspi->transferBytes(SRBytes, SPIDataRcv, 7); //Load active high state, multiple times for timing
        }
    }
    ClrSRBit(SRBytes, HIGH_DRIVE_BIT); //clear high drive bit in SRBytes
    vspi->transferBytes(SRBytes, SPIDataRcv, 7); //Set all but high drive back to initial state
    digitalWrite(NOT_SR_STROBE, LOW);//And transfer to outputs
    digitalWrite(NOT_SR_STROBE, HIGH);
    portENTER_CRITICAL(&timerMux);
    MsecTicks = 0; //clear the msec interrupt timer
    portEXIT_CRITICAL(&timerMux);
    PWMState = PWM_IDLE;
    vspi->endTransaction();
 }


//For first tests, we loop back.
bool ReceiveActive = false;
uint8_t StableCount;
int LastRBCount;
static long TotalMillisec = 0;
void IdleServe()
{
    //Check ticker here and modify telltale once for each count
    if(MsecTicks > 0)
    {
        ++TotalMillisec;
        if((TotalMillisec % 1000) == 0)
        {
            sprintf(StringBuff, "On Seconds = %ld\n", TotalMillisec/1000);
            WriteTraceBuffer(StringBuff);
        }
        #ifdef LINK_ACTIVE
        if(DoLinkReceive())
        {
            LinkServe();
        }
        #endif
        StateServe();  //so far only manages continuity scan or background AD readings, and Firing
        portENTER_CRITICAL(&timerMux);
        --MsecTicks;
        portEXIT_CRITICAL(&timerMux);
        if(digitalRead(TELL_TALE_1))
        {
            digitalWrite(TELL_TALE_1, LOW);
        }
        else
        {
            digitalWrite(TELL_TALE_1, HIGH);
        }
    }
}

void TraceBytes(U8 * Ptr, int Count)
{
    Serial1.write(Ptr, Count);
}

void TraceByte(U8 Value)
{
    Serial1.write(&Value, 1);
}
