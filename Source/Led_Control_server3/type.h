#ifndef TYPE_H
#define TYPE_H
#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

#define INT_IS_16_BITS
/* exact-width unsigned integer types */
//This version is for Arduino nano
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;

typedef uint8_t * UP_PTR_U8;
typedef uint16_t * UP_PTR_U16;
typedef uint32_t * UP_PTR_U32;
typedef uint8_t * DOWN_PTR_U8;
typedef uint16_t * DOWN_PTR_U16;
typedef uint32_t * DOWN_PTR_U32;
typedef float   * DOWN_PTR_FLOAT;
typedef float   * UP_PTR_FLOAT;

//#pragma message("end of compiling type.h")

#endif  /*TYPE_H*/
