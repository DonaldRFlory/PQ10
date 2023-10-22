/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
void DoTests();
void TellTale();
#define USE_ACCESS_POINT 1
#ifdef USE_ACCESS_POINT
    // Replace with your network credentials
    const char* ssid = "ESP32-Access_Point";
    const char* password = "123456789";
#else //using wi-fi router
    // Replace with your network credentials
    const char* ssid = "HOME-31AD-2.4";
    const char* password = "award7773energy";
#endif

//Rough calibration of A/D for resistance measurement:
const float CountsPerOhm    = 12.0;
//const float OffsetCounts    = 10.75;
const float OffsetCounts    = 110.7;

// Assign output variables to GPIO pins
const int NUM_CUES = 48;
const int NUM_SR_BITS = 56;
const int NUM_BUTTONS = 58;
const int CLEAR_BUTTON_INDEX = 56;
const int CONT_BUTTON_INDEX = 57;
const int REFRESH_BUTTON_INDEX = 58;
const int output  = 5;
const int output26 = 26;
const int output27 = 27;
const int SENSE12 = 34;
const int SENSEA = 36;
const int SENSEB = 39;
const int TELL_TALE_1 = 22;
const int TELL_TALE_2 = 21;
const int TELL_TALE_3 = 17;
const int NOT_SR_STROBE = 5;
const int KEEP_ALIVE = 16;
const int ATTR_CTRL = 1;
const int ATTR_CTRL1 = 2;

const int BT_0 = 0; //button type 0 off/dark-gray
const int BT_1 = 1; //button type Green
const int BT_2 = 2; //Dark blue

const int CMD_ON = 0;
const int CMD_OFF = 1;
const int CMD_CONT = 2;
const int CMD_CLEAR = 3;

const int ST_IDLE = 0; //overall state of controller
const int ST_CONT_SCAN = 1;
const int ST_CONT_DISP = 2;

//Defs for SR bits
const uint8_t TEST_CUR_BIT = 29;
const uint8_t HIGH_DRIVE_BIT = 31;
const uint8_t LED2_BIT = 24;
const uint8_t LED1_BIT = 30;

uint8_t CtrlState = ST_IDLE;
int ContScanCue, ContScanCount;
long ContScanAccumulator;


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

// Variable to store the HTTP request
String header;



// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
uint8_t  SRBytes[8];
uint8_t SPIDataRcv[8];
char StringBuff[100];

bool Flag;
void setup()
{
  Serial.begin(115200);
  pinMode(TELL_TALE_1, OUTPUT);
  pinMode(TELL_TALE_2, OUTPUT);
  pinMode(TELL_TALE_3, OUTPUT);
  pinMode(NOT_SR_STROBE, OUTPUT);
  pinMode(KEEP_ALIVE, OUTPUT);

  digitalWrite(TELL_TALE_1, LOW);
  digitalWrite(TELL_TALE_2, LOW);
  digitalWrite(TELL_TALE_3, LOW);
  digitalWrite(NOT_SR_STROBE, HIGH);
  digitalWrite(KEEP_ALIVE, LOW);

  // Connect to Wi-Fi network
  #ifdef USE_ACCESS_POINT
    Serial.print("Setting AP (Access Point)... ");

    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    server.begin();

  #else
      // Connect to Wi-Fi network with SSID and password
      Serial.print("Connecting to ");
      Serial.println(ssid);
      Serial.println(password);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      // Print local IP address and start web server
      Serial.println("");
      Serial.println("WiFi connected.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      server.begin();
  #endif

  //Setup SPI for 48 queue pyro control (56 bit S/R)
  vspi = new SPIClass(VSPI);
  vspi->begin();  //using default pins
  InitTimer();
  //DoTests();   //Tests HTML response decoding functions and sends out byte array for
                //each test on vspi.
}


//Initial values are for early tests
int RValsX10[NUM_CUES] =
{
    10, 11, 12, 13, 14, 15, 16, 17,
    20, 31, 42, 53, 064, 75, 86, 97,
    110, 121, 132, 143, 154, 165, 176, 187,
    190, 201, 212, 223, 234, 245, 256, 267,
    650, 661, 672, 683, 694, 705, 716, 727,
    900, 911, 922, 933, 944, 955, 966, 977,
};

//Shift register mapped button attributes
uint8_t SRBAttrs[NUM_BUTTONS] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    ATTR_CTRL, ATTR_CTRL, ATTR_CTRL, ATTR_CTRL, ATTR_CTRL,  0, ATTR_CTRL, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};


char *BStrs[NUM_BUTTONS] =
{
    "000", "001", "002", "003", "004", "005", "006", "007",
    "008", "009", "010", "011", "012", "013", "014", "015",
    "016", "017", "018", "019", "020", "021", "022", "023",
    "LD2", "---", "---", "---", "---", "TST", "LD1", "HDV",
    "024", "025", "026", "027", "028", "029", "030", "031",
    "032", "033", "034", "035", "036", "037", "038", "039",
    "040", "041", "042", "043", "044", "045", "046", "047",
};

void SetSRBit(int BitNo)
{
    int Byte;
    if(BitNo >= NUM_SR_BITS)
    {
        BitNo = NUM_SR_BITS -1;
    }
    SRBytes[BitNo >> 3] |=  (1 << (BitNo & 7));
}

void ClrSRBit(int BitNo)
{
    int Byte;
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

String Header;
bool GetCommandAndIndex(String &Header, int & Index, int & Command)
{
    int i;
    bool Result;
    char IdxStr[3];

    if((i = Header.indexOf("GET /off/")) >= 0)
    {
        i += 9;
        Command = CMD_OFF;
    }
    else if((i = Header.indexOf("GET /on/")) >= 0)
    {
        i += 8;
        Command = CMD_ON;
    }
    else if((i = Header.indexOf("GET /clear/")) >= 0)
    {
        i += 11;
        Command = CMD_CLEAR;
    }
    else if((i = Header.indexOf("GET /cont/")) >= 0)
    {
        i += 10;
        Command = CMD_CONT;
    }
    else
    {
        return false;
    }
    IdxStr[0] = Header.charAt(i++);
    IdxStr[1] = Header.charAt(i);
    IdxStr[2] = 0;
    Index = atoi(IdxStr);
    return true;
}

//We will do 20 counts per channel with five counts for settling
void ContinuityScan()
{
    float Reading;
    int Cue, Step, ADPin;
    if(CtrlState == ST_CONT_SCAN)
    {
        Cue = ContScanCount/20;
        ADPin = (Cue < 24) ? SENSEA : SENSEB;
        Step = ContScanCount%20;
        if(Step < 5)
        {
            ContScanAccumulator = 0;//just keep clearing it
            ++ContScanCount;
            return;//wait for settling
        }
        for(int i = 0; i < 10; ++i)
        {
            ContScanAccumulator += analogRead(ADPin); //take ten readings
        }
        if(Step >= 19)
        { //done with this cue
             ClrSRBit(CueToSRIdx(Cue)); //clear the just read cue's low drive

            //Compute and store resistance value
            Reading = ContScanAccumulator / 150.0;
            Reading -= OffsetCounts;
            Reading /= CountsPerOhm;
            Reading = Reading > 99.0 ? 99.0 : Reading < 0 ? 0 : Reading; //limit to 0-99 ohms
            RValsX10[Cue] = (int)(Reading * 10);

            if(Cue >= (NUM_CUES - 1))
            { //All done with scan
                ClrSRBit(TEST_CUR_BIT);//test current off
                CtrlState = ST_CONT_DISP;
                Serial.println("Continuity Scan Complete\r\n");
            }
            else
            {
                SetSRBit(CueToSRIdx(Cue + 1));//turn on low drive of next cue
            }
            Send56();//send out new state to SR
            Load56();
        }
        ++ContScanCount;
    }
}

void InitContinuityScan()
{
    CtrlState = ST_CONT_SCAN;

    Serial.println("InitContinuityScan()\r\n");
    for(int i = 0; i < NUM_SR_BITS; ++i)
    {
        if((SRBAttrs[i] & ATTR_CTRL) == 0)
        {
            ClrSRBit(i);//clear all SR bits which to not have ATTR_CTRL attribute
        }
    }
    ContScanCount = 0;

    //Setup to measure cue 0
    SetSRBit(TEST_CUR_BIT);//test current on
    SetSRBit(CueToSRIdx(0));//Set first cue low drive
    //shifted out at end of ProcessCommand()
}

void ProcessCommand(String & Header) //Set or clear bits in the seven byte array for PQ driver board
{
    bool Result;
    int Index = -1, Command = -1;
    if(GetCommandAndIndex(Header, Index, Command))
    {
        switch(Command)
        {
            case CMD_OFF:
                if(CtrlState == ST_IDLE || ((SRBAttrs[Index] & ATTR_CTRL) != 0) )
                {
                    ClrSRBit(Index);
                }
                sprintf(StringBuff, "Cmd: off, %d", Index);
                break;

            case CMD_ON:
                if(CtrlState == ST_IDLE || ((SRBAttrs[Index] & ATTR_CTRL) != 0) )
                {
                    SetSRBit(Index);
                }
                sprintf(StringBuff, "Cmd: on, %d", Index);
                break;

            case CMD_CLEAR:
                for(int i = 0; i < NUM_SR_BITS; ++i)
                {
                    ClrSRBit(i);
                }
                CtrlState = ST_IDLE;
                sprintf(StringBuff, "Cmd: clear, %d", Index);
                break;

            case CMD_CONT:
                sprintf(StringBuff, "Cmd: cont, %d", Index);
                if(CtrlState == ST_IDLE)
                {
                    InitContinuityScan();
                }
                else if(CtrlState == ST_CONT_DISP)
                {
                    CtrlState = ST_IDLE;
                }
                break;
        }
        Send56();
        Load56();
    }
    else
    {
        sprintf(StringBuff, "Cmd: None");
    }
    Serial.println(StringBuff);
}


void DoTest(String &TestString)
{
    bool Result;
    int Index = -1, State = -1;

    Serial.write("DoTest(): \"");
    Serial.write(Header.c_str());
    Serial.write("\"\r\n");
    Result = GetCommandAndIndex(TestString, Index, State);
    if(State == 1)
    {
        for(int i = 0; i < 8; ++i)
        {
            SRBytes[i] = 0X00;
        }
        SetSRBit(Index);
        sprintf(StringBuff, "OK: %d, %d\r\n", Index, State);
    }
    else if(State == 0)
    {
        for(int i = 0; i < 8; ++i)
        {
            SRBytes[i] = 0XFF;
        }
        ClrSRBit(Index);
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

//Note: all buttons are toggle buttons. Green indicates ON. Pressing button changes state
void GenButton(String &Text, int ButtonIdx, bool On)
{
    int CueIndex = -1;
    uint8_t Type;
    char RString[10];
    char * Cmd;
    char * ButtonString;
    bool IsCue;

    char WorkStr[100];
    if(ButtonIdx == CLEAR_BUTTON_INDEX)
    {
        sprintf(WorkStr, "<a href=\"/clear/%02d\"><button class=\"button\">CLR</button></a>", ButtonIdx);
        Text += WorkStr;
        return;
    }
    else if(ButtonIdx == CONT_BUTTON_INDEX)
    {
        switch(CtrlState)
        {
            default:
            case ST_IDLE:
                sprintf(WorkStr, "<a href=\"/cont/%02d\"><button class=\"button\">CONT</button></a>", ButtonIdx);
                break;

            case ST_CONT_SCAN:    //blue while scanning
                sprintf(WorkStr, "<a href=\"/cont/%02d\"><button class=\"button button2\">CONT</button></a>", ButtonIdx);
                break;

            case ST_CONT_DISP:    //green while displaying continuity values
                sprintf(WorkStr, "<a href=\"/cont/%02d\"><button class=\"button button1\">CONT</button></a>", ButtonIdx);
                break;
        }
        Text += WorkStr;
        return;
    }
    else if(ButtonIdx == REFRESH_BUTTON_INDEX)
    {
        sprintf(WorkStr, "<a href=\"/refresh/%02d\"><button class=\"button\">REFR</button></a>", ButtonIdx);
        Text += WorkStr;
        return;
    }

    //Now we handle buttons in main array mapping on to shift register
    if(ButtonIdx >= NUM_SR_BITS)
    {
        ButtonIdx = NUM_SR_BITS - 1;
    }

    if(On)
    {
        Cmd = "off";
        Type = BT_1;
    }
    else
    {
        Cmd = "on";
        Type = BT_0;
    }

    ButtonString = BStrs[ButtonIdx];

    if(CtrlState == ST_CONT_DISP)
    {
        //So.. if button is one of the cues, we want to replace it with the approprite resistance value
        if(ButtonIdx < 24)
        {
            IsCue = true;
            CueIndex = ButtonIdx;
        }
        else if(ButtonIdx < 32)
        {
            IsCue = false;
        }
        else if(ButtonIdx >= 32)
        {
            IsCue = true;
            CueIndex = ButtonIdx - 8;
        }
        if(IsCue)
        {
            if(RValsX10[CueIndex] < 600)
            {
                sprintf(RString, "%2d.%d", RValsX10[CueIndex]/10, RValsX10[CueIndex]%10);
            }
            else
            {
                sprintf(RString, "X");
            }
            ButtonString = RString;
            Type = BT_2;
        }
    }

    switch(Type)
    {
        default:
        case BT_0:
            sprintf(WorkStr, "<a href=\"/%s/%02d\"><button class=\"button\">%s</button></a>", Cmd, ButtonIdx, ButtonString);
            break;

        case BT_1:
            sprintf(WorkStr, "<a href=\"/%s/%02d\"><button class=\"button button1\">%s</button></a>", Cmd,  ButtonIdx, ButtonString);
            break;

        case BT_2:    //will never hit this unless we change code above
            sprintf(WorkStr, "<a href=\"/%s/%02d\"><button class=\"button button2\">%s</button></a>", Cmd,  ButtonIdx, ButtonString);
            break;
    }
    Text += WorkStr;
}

void GenerateButtons(String &PageStr)
{
    int i = 0;
    //Serial.println("GenerateButtons\r\n");
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

//    PageStr += "<p>"; //start of button line
//    GenButton(PageStr, CLEAR_BUTTON_INDEX, false);
//    GenButton(PageStr, CONT_BUTTON_INDEX, false);
//    GenButton(PageStr, REFRESH_BUTTON_INDEX, false);
    PageStr += "</p>\r\n"; //end of button line  (paragraph)
    //Serial.println(PageStr.c_str());
}

String PageStr= "";  // String to accumulate pieces of webpage as we generate it
void loop(){
  TellTale();

  WiFiClient client = server.available();   // Listen for incoming clients
  TellTale();

  if (client) {                             // If a new client connects,
     currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    String PageStr= "";                // make a String to hold the webpage we serve up
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {  // loop while the client's connected
      TellTale();
      currentTime = millis();
      if (client.available())
      {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n')
        {                    // if the byte is a newline character
          digitalWrite(TELL_TALE_2, HIGH);
          TellTale();
          digitalWrite(TELL_TALE_2, LOW);
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            PageStr =  "HTTP/1.1 200 OK\r\n";
            PageStr += "Content-type:text/html\r\n";
            PageStr += "Connection: close\r\n\n";
            client.println(PageStr); //and send it out to client
            //Serial.println(PageStr.c_str());
            TellTale();

            ProcessCommand(header); //Set or clear bits in the seven byte array for PQ driver board

            // Display the HTML web page
            PageStr = "<!DOCTYPE html><html>\r\n";
            PageStr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n";
            PageStr += "<link rel=\"icon\" href=\"data:,\">\r\n";

            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            PageStr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\r\n";
            PageStr += ".button { background-color: #555555; border: none; color: white; padding: 8px 8px; width: 40px;\r\n"; //medium dark gray
            PageStr += "text-decoration: none; font-size: 12px; margin: 2px; cursor: pointer;}\r\n";
            PageStr += ".button1 {background-color: #4CAF50;\r\n}"; //green
            PageStr += ".button2 {background-color: #025ADC;}";  //dark blue
            PageStr += "</style></head>";
            client.println(PageStr);
//            Serial.println(PageStr.c_str());
            TellTale();

            // Web Page Heading
            PageStr = "<body><h1>ESP32 Web Server</h1>\r\n";

//            PageStr += "<p>GPIO 26 - State </p>\r\n";
//            GenerateButtons(PageStr);
            client.println(PageStr);
            //Serial.println(PageStr.c_str());
            TellTale();
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
          TellTale();
          currentLine += c;      // add it to the end of the currentLine
        } //end of if char is newline
      } //end of if client has a character
    } //end of while client is connnected
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void TellTale()
{
    //Check ticker here and modify telltale once for each count
    if(MsecTicks > 0)
    {
        ContinuityScan();
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

void DoTests()
{
    Header = "GET /off/00!anyJunk";
    DoTest(Header);
    Header = "GET /off/01!anyJunk";
    DoTest(Header);
    Header = "GET /off/02!anyJunk";
    DoTest(Header);
    Header = "GET /off/03!anyJunk";
    DoTest(Header);
    Header = "GET /off/04!anyJunk";
    DoTest(Header);
    Header = "GET /off/05!anyJunk";
    DoTest(Header);
    Header = "GET /off/06!anyJunk";
    DoTest(Header);
    Header = "GET /off/07!anyJunk";
    DoTest(Header);
    Header = "GET /off/08!anyJunk";
    DoTest(Header);
    Header = "GET /off/09!anyJunk";
    DoTest(Header);
    Header = "GET /off/10!anyJunk";
    DoTest(Header);
    Header = "GET /off/99!anyJunk";
    DoTest(Header);
    Header = "GET /off/54!anyJunk";
    DoTest(Header);

    Header = "GET /on/00!anyJunk";
    DoTest(Header);
    Header = "GET /on/01!anyJunk";
    DoTest(Header);
    Header = "GET /on/02!anyJunk";
    DoTest(Header);
    Header = "GET /on/03!anyJunk";
    DoTest(Header);
    Header = "GET /on/04!anyJunk";
    DoTest(Header);
    Header = "GET /on/05!anyJunk";
    DoTest(Header);
    Header = "GET /on/06!anyJunk";
    DoTest(Header);
    Header = "GET /on/07!anyJunk";
    DoTest(Header);
    Header = "GET /on/08!anyJunk";
    DoTest(Header);
    Header = "GET /on/09!anyJunk";
    DoTest(Header);
    Header = "GET /on/10!anyJunk";
    DoTest(Header);
    Header = "GET /on/99!anyJunk";
    DoTest(Header);
    Header = "GET /on/54!anyJunk";
    DoTest(Header);
}
