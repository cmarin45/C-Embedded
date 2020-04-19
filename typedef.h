/*************************************************************************
 * File: typedef.h
 * Description: General type definitions
 *************************************************************************/

#ifndef _NIVIS_TYPEDEF_H_
#define _NIVIS_TYPEDEF_H_

// Types definition
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

typedef signed char     int8;
typedef unsigned char   uint8;
typedef signed short    int16;
typedef unsigned short  uint16;
typedef signed long     int32;
typedef unsigned long   uint32;
typedef float           fp32;
typedef unsigned long long uint64;

typedef void (* handler_func_type)(void);

typedef uint16 HANDLE;
typedef uint16 SHORT_ADDR;
typedef uint8 LONG_ADDR[8];
typedef uint8 IPV6_ADDR[16];
typedef uint8 EUI64_ADDR[8];
typedef uint32 TIME;

//
// Boolean type.
//
typedef enum
{
  FALSE = 0,
  TRUE  = 1
} BOOL;
#define Boolean BOOL

//
// Global status type.
// This type should be maybe extended. 
//
typedef enum
{
    SUCCESS           = 0,
    UNSPECIFIED_ERROR = 1,
    //  READ_ONLY,
    WRITE_ONLY        = 3,
    INVALID_PARAMETER = 4,
    INVALID_ADDRESS   = 5,
    DUPLICATE         = 6,
    OUT_OF_MEMORY     = 7, // DO NOT CHANGE THIS VALUE!!! USED BY BACKBONE ALSO!!!
    UID_OUT_OF_RANGE  = 8,
    DATA_TOO_LONG     = 9,
    NO_CHANNEL        = 10,
    NO_TIMESLOT       = 11,
    //  NO_NEIGHBOR,
    //  NO_SUPERFRAME,
    NO_LINK           = 12,
    RX_LINK           = 13,
    TX_LINK           = 14,
    NO_ROUTE          = 15,
    NO_CONTRACT       = 16,
    NO_UID            = 17,
    TOO_BIG           = 18,
    QUEUE_FULL        = 19,
    NACK              = 20,
    TIMEOUT           = 21,
    MSG_NOT_FOUND     = 22,
    NO_GRAPH          = 23,
    INVALID_SC        = 24
} SC;


/*
 #define BIT0    0x00000001UL
 #define BIT1    0x00000002UL
 #define BIT2    0x00000004UL
 #define BIT3    0x00000008UL
 #define BIT4    0x00000010UL
 #define BIT5    0x00000020UL
 #define BIT6    0x00000040UL
 #define BIT7    0x00000080UL
 #define BIT8    0x00000100UL
 #define BIT9    0x00000200UL
 #define BIT10   0x00000400UL
 #define BIT11   0x00000800UL
 #define BIT12   0x00001000UL
 #define BIT13   0x00002000UL
 #define BIT14   0x00004000UL
 #define BIT15   0x00008000UL
 #define BIT16   0x00010000UL
 #define BIT17   0x00020000UL
 #define BIT18   0x00040000UL
 #define BIT19   0x00080000UL
 #define BIT20   0x00100000UL
 #define BIT21   0x00200000UL
 #define BIT22   0x00400000UL
 #define BIT23   0x00800000UL
 #define BIT24   0x01000000UL
 #define BIT25   0x02000000UL
 #define BIT26   0x04000000UL
 #define BIT27   0x08000000UL
 #define BIT28   0x10000000UL
 #define BIT29   0x20000000UL
 #define BIT30   0x40000000UL
 #define BIT31   0x80000000UL
 */
#ifndef NULL
#define NULL (void *)(0)
#endif

#define offsetof(s,m)	(uint8)&(((s *)0)->m)

#endif /* _TYPEDEF_H_ */




