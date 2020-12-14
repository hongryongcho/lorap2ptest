/*
*
@file		type.h
*
*/

#ifndef _TYPE_H_
#define _TYPE_H_


/***************************************************
 * attribute for mcu ( types, ... ) 
 ***************************************************/

#define	MAX_SOCK_NUM		8	/**< Maxmium number of socket */

/**
@brief	 __DEF_IINCHIP_MAP_xxx__ : define memory map for iinchip 
*/
//#define __DEF_IINCHIP_PPP

#define IINCHIP_ISR_DISABLE()  __disable_fault_irq()
#define IINCHIP_ISR_ENABLE() __enable_fault_irq()

#ifndef NULL
#define NULL		((void *) 0)
#endif

//typedef enum { false, true } bool;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

/**
 * The 8-bit signed data type.
 */
typedef char int8;
/**
 * The volatile 8-bit signed data type.
 */
typedef volatile char vint8;
/**
 * The 8-bit unsigned data type.
 */
typedef unsigned char uint8;
/**
 * The volatile 8-bit unsigned data type.
 */
typedef volatile unsigned char vuint8;

/**
 * The 16-bit signed data type.
 */
typedef int int16;
/**
 * The volatile 16-bit signed data type.
 */
//typedef volatile int vint16;
/**
 * The 16-bit unsigned data type.
 */
typedef unsigned short uint16;
/**
 * The volatile 16-bit unsigned data type.
 */
//typedef volatile unsigned int vuint16;
/**
 * The 32-bit signed data type.
 */
typedef long int32;
/**
 * The volatile 32-bit signed data type.
 */
//typedef volatile long vint32;
/**
 * The 32-bit unsigned data type.
 */
typedef unsigned long uint32;
/**
 * The volatile 32-bit unsigned data type.
 */
//typedef volatile unsigned long vuint32;

/* bsd */
typedef uint8		    u_char;	  /**< 8-bit value */
typedef uint8 		    SOCKET;
typedef uint16		    u_short;	  /**< 16-bit value */
typedef uint16		    u_int;	  /**< 16-bit value */
typedef uint32		    u_long;	  /**< 32-bit value */

typedef double                Flo64;    // Double precision floating point
typedef double              * pFlo64;
typedef float                 Flo32;    // Single precision floating point
typedef float               * pFlo32;
typedef signed   long long    Int64S;   // Signed   64 bit quantity
typedef signed   long long  * pInt64S;
typedef unsigned long long    Int64U;   // Unsigned 64 bit quantity
typedef unsigned long long  * pInt64U;
typedef signed   int          Int32S;   // Signed   32 bit quantity
typedef signed   int        * pInt32S;
typedef unsigned int          Int32U;   // Unsigned 32 bit quantity
typedef unsigned int        * pInt32U;
typedef signed   short        Int16S;   // Signed   16 bit quantity
typedef signed   short      * pInt16S;
typedef unsigned short        Int16U;   // Unsigned 16 bit quantity
typedef unsigned short      * pInt16U;
typedef signed   char         Int8S;    // Signed    8 bit quantity
typedef signed   char       * pInt8S;
typedef unsigned char         Int8U;    // Unsigned  8 bit quantity
typedef unsigned char       * pInt8U;
typedef unsigned int          Boolean;  // Boolean
typedef unsigned int        * pBoolean;

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef unsigned long long U64;

typedef signed char S8;
typedef signed int S16;
typedef signed long S32;




typedef union UU16
{
    U16 U16;
    S16 S16;
    U8 U8[2];
    S8 S8[2];
} UU16;

typedef union UU32
{
    U32 U32;
    S32 S32;
    UU16 UU16[2];
    U16 U16[2];
    S16 S16[2];
    U8 U8[4];
    S8 S8[4];
} UU32;
typedef union _un_l2cval {
	u_long	lVal;
	u_char	cVal[4];
}un_l2cval;

typedef union _un_i2cval {
	u_int	iVal;
	u_char	cVal[2];
}un_i2cval;


#define xdata 
#define idata


#endif		/* _TYPE_H_ */
