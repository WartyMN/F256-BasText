/*
 * basic_2_text.h
 *
 *  Created on: Jan 6, 2023
 *      Author: micahbly
 */
 

#ifndef BASIC_2_TEXT_H
#define BASIC_2_TEXT_H

/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes

// C includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

// hide __fastcall_ from everything but CC65 (to squash some warnings in LSP/BBEdit)
#ifndef __CC65__
	#define __fastcall__ 
#endif

#ifndef NULL
#  define NULL 0
#endif


#define MAJOR_VERSION	0
#define MINOR_VERSION	2
#define UPDATE_VERSION	0


/*****************************************************************************/
/*                             MEMORY LOCATIONS                              */
/*****************************************************************************/

// temp storage for data outside of normal cc65 visibility - extra memory!
#define STORAGE_STRING_MERGE_BUFFER		global_temp_buff_192b_1		// 192 bytes
#define STORAGE_STRING_MERGE_BUFFER_SIZE	192 // will use for snprintf, strncpy, etc.


// TEMP DEFINES - remove after bug testing completes
#define STORAGE_INTERBANK_BUFFER		0x6000	// 1-page buffer. see cc65 memory config file. this is outside cc65 space.
#define STORAGE_INTERBANK_BUFFER_LEN	0x0100	// 1-page buffer. see cc65 memory config file. this is outside cc65 space.


/*****************************************************************************/
/*                               Character-codes                             */
/*****************************************************************************/

// named keys
#define CH_SPACE				32
#define CH_CHECKERBOARD			199 // assumes foenix std font

#define CH_F1      		224	// as defined in cbm610.h
#define CH_CURS_UP      0x10
#define CH_CURS_DOWN	0x0e
#define CH_CURS_LEFT    0x02
#define CH_CURS_RIGHT	0x06
#define CH_DEL			0x08
#define CH_ENTER         13
#define CH_ESC           27
#define CH_TAB		      9


/*****************************************************************************/
/*                                 Error Codes                               */
/*****************************************************************************/

#define ERROR_NO_ERROR													0
#define ERROR_LOAD_BUFFER_TOO_SMALL										1
#define ERROR_LOAD_DATA_INTEGRITY										2
#define ERROR_SAVE_DATA_INTEGRITY										3
#define ERROR_UNEXPECTED_FILE_DATA										4
#define ERROR_UNABLE_TO_OPEN_INPUT_FILE									5
#define ERROR_UNABLE_TO_OPEN_OUTPUT_FILE								6
#define ERROR_FILENAME_ENTRY_ISSUE										7
#define ERROR_INVALID_BASIC_FILE										8
#define ERROR_INVALID_BASIC_START_ADDRESS								9
#define ERROR_SAVE_BUFFER_TOO_SMALL										10


/*****************************************************************************/
/*                            String ID Values                               */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/




/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/



/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/

// display error message at top of screen and exit
void exit_with_wait(uint8_t the_error_number);





#endif /* BASIC_2_TEXT_H */
