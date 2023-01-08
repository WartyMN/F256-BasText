//! @file general.h

/*
 * general.h
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 */

// This is a cut-down, semi-API-compatible version of the OS/f general.c file from Lich King (Foenix)
// adapted for Foenix F256 Jr starting November 29, 2022


#ifndef GENERAL_H_
#define GENERAL_H_


/* about this class
 *
 *
 *
 *** things this class needs to be able to do
 * various utility functions that any app could find useful.
 * intended to be re-usable across apps, with at most minimal differences.
 * this file should contain only cross-platform code. platform-specific code should go into general_[platformname].h/.c
 *
 *** things objects of this class have
 *
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
//#include "lk_text.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// cc65 includes

/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define CROSS_BANK_COPY_BUFFER_LEN	256	// 1 page, 256b is available for cross-screen copying


// replacement for sys jiffies
#define sys_time_jiffies()	*(uint8_t*)0x8d * 65536 + *(uint8_t*)0x8e * 256 + *(uint8_t*)0x8f

// general
#define MAX_STRING_COMP_LEN		192		//!< 255 + terminator is max string size for compares

// word-wrap and string measurement related
#define GEN_NO_STRLEN_CAP		-1		//!< for the xxx_DrawString function's max_chars parameter, the value that corresponds to 'draw the entire string if it fits, do not cap it at n characters' 
#define WORD_WRAP_MAX_LEN		192	//!< For the xxx_DrawStringInBox function, the strnlen char limit. 40*25. 

#ifdef LOG_LEVEL_1 
	#define LOG_ERR(x) General_LogError x
#else
	#define LOG_ERR(x)
#endif
#ifdef LOG_LEVEL_2
	#define LOG_WARN(x) General_LogWarning x
#else
	#define LOG_WARN(x)
#endif
#ifdef LOG_LEVEL_3
	#define LOG_INFO(x) General_LogInfo x
#else
	#define LOG_INFO(x)
#endif
#ifdef LOG_LEVEL_4
	#define DEBUG_OUT(x) General_DebugOut x
#else
	#define DEBUG_OUT(x)
#endif
#ifdef LOG_LEVEL_5
	#define LOG_ALLOC(x) General_LogAlloc x
#else
	#define LOG_ALLOC(x)
#endif


/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/

#define LogError	0
#define LogWarning	1
#define LogInfo		2
#define LogDebug	3
#define LogAlloc	4


/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct Coordinate
{
    uint8_t x;
    uint8_t y;
} Coordinate;

typedef struct Rectangle
{
	uint8_t		MinX, MinY;
	uint8_t		MaxX, MaxY;
} Rectangle;

/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/



// **** WORD-WRAP UTILITIES *****





// **** MATH UTILITIES *****




// **** NUMBER<>STRING UTILITIES *****






// **** MISC STRING UTILITIES *****

// return the global string for the passed ID
// this is just a wrapper around the string, to make it easier to re-use and diff code in different overlays
char* General_GetString(uint8_t the_string_id);




// **** RECTANGLE UTILITIES *****





// **** FILENAME AND FILEPATH UTILITIES *****





// **** TIME UTILITIES *****


//! Wait for the specified number of ticks before returning
//! In PET/B128 implementation, we don't bother with real ticks.
void General_DelayTicks(int16_t ticks);

// //! Wait for the specified number of seconds before returning
// //! In multi-tasking ever becomes a thing, this is not a multi-tasking-friendly operation. 
// void General_DelaySeconds(uint16_t seconds);



// **** USER INPUT UTILITIES *****

// // Wait for one character from the keyboard and return it
// char General_GetChar(void);





// **** MISC UTILITIES *****




// **** LOGGING AND DEBUG UTILITIES *****





// *********  logging functionality. requires global_log_file to have been opened.
void General_LogError(const char* format, ...);
void General_LogWarning(const char* format, ...);
void General_LogInfo(const char* format, ...);
void General_DebugOut(const char* format, ...);
void General_LogAlloc(const char* format, ...);
bool General_LogInitialize(void);
void General_LogCleanUp(void);


// debug function to print out a chunk of memory character by character
void General_PrintBufferCharacters(uint8_t* the_mem_loc, int16_t the_len);


#endif /* GENERAL_H_ */
