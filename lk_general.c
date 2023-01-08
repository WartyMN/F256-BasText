/*
 * general.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 *
 *  - adapted from my Amiga WB2K project's general.c
 */

// This is a cut-down, semi-API-compatible version of the OS/f general.c file from Lich King (Foenix)
// adapted for Foenix F256 Jr starting November 29, 2022


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lk_general.h"
#include "lk_sys.h"
#include "lk_text.h"
#include "basic2text.h"

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
//#include <math.h>

// cc65 includes


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/




/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

uint8_t*				interbank_temp = (uint8_t*)STORAGE_INTERBANK_BUFFER;	// 1-page buffer. see cc65 memory config file. this is outside cc65 space.




// logging related: only do if debugging is active
#if defined LOG_LEVEL_1 || defined LOG_LEVEL_2 || defined LOG_LEVEL_3 || defined LOG_LEVEL_4 || defined LOG_LEVEL_5
	//static char				debug_buffer[256];	// create once, use for every debug and logging function
	//static char*			debug_string = debug_buffer;
	static char*			debug_string = (char*)0x0300;	// using a hard-coded location as a quick test
	static const char*		kDebugFlag[5] = {
								"[ERROR]",
								"[WARNING]",
								"[INFO]",
								"[DEBUG]",
								"[ALLOC]"
							};

	static FILE*			global_log_file;
#endif



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/






/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/







// **** MATH UTILITIES *****






// **** NUMBER<>STRING UTILITIES *****





// **** MISC STRING UTILITIES *****





// **** RECTANGLE UTILITIES *****













// **** FILENAME AND FILEPATH UTILITIES *****








// **** TIME UTILITIES *****


//! Wait for the specified number of ticks before returning
//! In PET/B128 implementation, we don't bother with real ticks.
void General_DelayTicks(int16_t ticks)
{
	int16_t	i;
	int16_t	j;
	
	for (i = 0; i < ticks; i++)
	{
		for (j = 0; j < 5; j++);
	}
}


// //! Wait for the specified number of seconds before returning
// //! In multi-tasking ever becomes a thing, this is not a multi-tasking-friendly operation. 
// void General_DelaySeconds(uint16_t seconds)
// {
// 	long	start_ticks = sys_time_jiffies();
// 	long	now_ticks = start_ticks;
// 	
// 	while ((now_ticks - start_ticks) / SYS_TICKS_PER_SEC < seconds)
// 	{
// 		now_ticks = sys_time_jiffies();
// 	}
// }








// **** USER INPUT UTILITIES *****

// // Wait for one character from the keyboard and return it
// char General_GetChar(void)
// {
// 	uint8_t		the_char;
// 	
// #ifdef _C256_FMX_
// 	char	(*Kernal_GetCharWithWait)(void);
// 	//GETCHW	$00:104C	Get a character from the input channel. Waits until data received. A=0 and Carry=1 if no data is waiting
// 	Kernal_GetCharWithWait = (void*)(0x00104c);
// 	
// 	the_char = Kernal_GetCharWithWait();
// #else
// 	the_char = getchar();
// #endif
// 
// 	return the_char;
// }





// **** MISC UTILITIES *****





// **** LOGGING AND DEBUG UTILITIES *****


#if defined LOG_LEVEL_1 || defined LOG_LEVEL_2 || defined LOG_LEVEL_3 || defined LOG_LEVEL_4 || defined LOG_LEVEL_5


// DEBUG functionality I want:
//   3 levels of logging (err/warn/info)
//   additional debug out function that leaves no footprint in compiled release version of code (calls to it also disappear)
//   able to pass format string and multiple variables when needed

void General_LogError(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_string, format, args);
	va_end(args);

#ifdef _SIMULATOR_
	// f256jr emulator has a log to console feature:
	// *((long *)-4) = (long)&debug_buffer;
// 	__AX__ = &debug_buffer;
// 	asm("cb $300");
	Memory_DebugOut(); // calls a simple assembly routine that does "CB &debug_buffer". cc65's inline assembler only accepts valid 6502 opcodes.
#else
	// if not on simulator, write out to file
// 	fprintf(global_log_file, "%s %s\n", kDebugFlag[LogError], debug_string);
// 	printf("%s %s\r", kDebugFlag[LogError], debug_string);
#endif
}

void General_LogWarning(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_string, format, args);
	va_end(args);

#ifdef _SIMULATOR_
	// f256jr emulator has a log to console feature:
	Memory_DebugOut(); // calls a simple assembly routine that does "CB &debug_buffer". cc65's inline assembler only accepts valid 6502 opcodes.
#else
	// if not on simulator, write out to file
// 	fprintf(global_log_file, "%s %s\n", kDebugFlag[LogWarning], debug_string);
// 	printf("%s %s\r", kDebugFlag[LogWarning], debug_string);
#endif
}

void General_LogInfo(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_string, format, args);
	va_end(args);

#ifdef _SIMULATOR_
	// f256jr emulator has a log to console feature:
	Memory_DebugOut(); // calls a simple assembly routine that does "CB &debug_buffer". cc65's inline assembler only accepts valid 6502 opcodes.
#else
	// if not on simulator, write out to file
// 	fprintf(global_log_file, "%s %s\n", kDebugFlag[LogInfo], debug_string);
// 	printf("%s %s\r", kDebugFlag[LogInfo], debug_string);
#endif
}

void General_DebugOut(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_string, format, args);
	va_end(args);
	
#ifdef _SIMULATOR_
	// f256jr emulator has a log to console feature:
	Memory_DebugOut(); // calls a simple assembly routine that does "CB &debug_buffer". cc65's inline assembler only accepts valid 6502 opcodes.
#else
	// if not on simulator, write out to file
// 	fprintf(global_log_file, "%s %s\n", kDebugFlag[LogDebug], debug_string);
// 	printf("%s %s\r", kDebugFlag[LogDebug], debug_string);
#endif
}

void General_LogAlloc(const char* format, ...)
{
	va_list		args;
	
	va_start(args, format);
	vsprintf(debug_string, format, args);
	va_end(args);
	
#ifdef _SIMULATOR_
	// f256jr emulator has a log to console feature:
	Memory_DebugOut(); // calls a simple assembly routine that does "CB &debug_buffer". cc65's inline assembler only accepts valid 6502 opcodes.
#else
	// if not on simulator, write out to file
// 	fprintf(global_log_file, "%s %s\n", kDebugFlag[LogAlloc], debug_string);
// 	printf("%s %s\r", kDebugFlag[LogAlloc], debug_string);
#endif
}

// initialize log file
// globals for the log file
bool General_LogInitialize(void)
{
	const char*		the_file_path = "lk_log.txt";

// 	global_log_file = fopen( the_file_path, "w");
	
	if (global_log_file == NULL)
	{
		printf("General_LogInitialize: log file could not be opened! \n");
		return false;
	}
	
	return true;
}

// close the log file
void General_LogCleanUp(void)
{
	if (global_log_file != NULL)
	{
// 		fclose(global_log_file);
	}
}

#endif





// debug function to print out a chunk of memory character by character
void General_PrintBufferCharacters(uint8_t* the_mem_loc, int16_t the_len)
{
	int16_t		bytes_out = 0;
	int16_t		bytes_left = the_len;
	char*		out = (char*)interbank_temp;
	uint8_t		y = 0;
	uint8_t		i;
	
	// LOGIC: 
	//   we only have 80x60 to work with, and "MEM DUMP" takes 1 row, so 59 rows * 16 bytes = 944 max bytes can be shown
	#define MEM_DUMP_BYTES_PER_ROW		16
	#define MAX_MEM_DUMP_LEN			(59 * MEM_DUMP_BYTES_PER_ROW)
	#define MEM_DUMP_START_X_FOR_HEX	7
	#define MEM_DUMP_START_X_FOR_CHAR	(MEM_DUMP_START_X_FOR_HEX + MEM_DUMP_BYTES_PER_ROW * 3)
	
	if (the_len > MAX_MEM_DUMP_LEN)
	{
		the_len = MAX_MEM_DUMP_LEN;
	}
	
	strcpy(out, "MEM DUMP: ");
	Text_DrawStringAtXY(0, y++, out, COLOR_BRIGHT_CYAN, COLOR_BLACK);

	// print one row
	while (bytes_left > 0)
	{
		sprintf(out, "%p: ", the_mem_loc);
		Text_DrawStringAtXY(0, y, out, COLOR_GREEN, COLOR_BLACK);
		
		sprintf(out, "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x  ", 
			the_mem_loc[0], the_mem_loc[1], the_mem_loc[2], the_mem_loc[3], the_mem_loc[4], the_mem_loc[5], the_mem_loc[6], the_mem_loc[7], 
			the_mem_loc[8], the_mem_loc[9], the_mem_loc[10], the_mem_loc[11], the_mem_loc[12], the_mem_loc[13], the_mem_loc[14], the_mem_loc[15]);

		Text_DrawStringAtXY(MEM_DUMP_START_X_FOR_HEX, y, out, COLOR_BRIGHT_GREEN, COLOR_BLACK);

		// render chars with char draw function to avoid problem of 0s getting treated as nulls in sprintf
		for (i = 0; i < MEM_DUMP_BYTES_PER_ROW; i++)
		{
			Text_SetCharAndColorAtXY(MEM_DUMP_START_X_FOR_CHAR + i, y, the_mem_loc[i], COLOR_BRIGHT_GREEN, COLOR_BLACK);
		}
		
		bytes_left -= MEM_DUMP_BYTES_PER_ROW;
		the_mem_loc += MEM_DUMP_BYTES_PER_ROW;
		++y;
	}
	
	getchar();
}
