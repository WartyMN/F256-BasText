/*
 * CBM Basic2Text
 *
 *  Created on: Created on: Jan 4, 2023 (forked from BasText by Peter Krefting)
 *      F256 port: micah bly
 *
 */
 


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes
#include "basic2text.h"
#include "lk_general.h"
#include "lk_text.h"
#include "lk_sys.h"

#include "inmode.h"
#include "detokenize.h"

// C includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cc65 includes

/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/

#define ATTR_CURSOR					176	// bright yellow on black
#define ATTR_USER_INPUT				176	// bright yellow on black
#define FILENAME_QUESTION_Y			0	// will ask q on row 0, get filename on row 1
#define FILENAME_INPUT_Y			1	// will ask q on row 0, get filename on row 1
#define FILENAME_INPUT_X			0	// user will start typing at position 0

#define MAX_FILENAME_LEN			16	// CBM DOS defined

/*****************************************************************************/
/*                          File-Scope Variables                             */
/*****************************************************************************/

static char			in_filename_buf[MAX_FILENAME_LEN+1];
static char*		in_filename = in_filename_buf;
static char			out_filename_buf[MAX_FILENAME_LEN+1];
static char*		out_filename = out_filename_buf;

/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

static char		temp_buff_192b_1[192];
char* 			global_string_buffer = temp_buff_192b_1;




/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// get a string from the user and store in the passed buffer, drawing chars to screen as user types
// allows a maximum of the_max_length characters.
// returns false if no string built.
bool GetStringFromUser(char* the_buffer, int8_t the_max_length, int8_t x, int8_t y);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// get a string from the user and store in the passed buffer, drawing chars to screen as user types
// allows a maximum of the_max_length characters.
// returns false if no string built.
bool GetStringFromUser(char* the_buffer, int8_t the_max_length, int8_t start_x, int8_t start_y)
{
	char*		original_string = the_buffer;
	char*		the_user_input = the_buffer;
	int8_t		x = start_x;
	uint8_t		characters_remaining;
	uint8_t		the_char;
	uint8_t		the_cursor_char_code = CH_CHECKERBOARD;
	uint8_t		attr_cursor = ATTR_CURSOR;
	uint8_t		attr_text = ATTR_USER_INPUT;
	
	//DEBUG_OUT(("%s %d: entered; the_max_length=%i", __func__, __LINE__, the_max_length));

	Text_FillBox(
		0, start_y,
		79, start_y, 
		CH_SPACE, COLOR_BRIGHT_YELLOW, COLOR_BLACK
	);

	// return false if the_max_length is so small we can't make a string
	if (the_max_length < 1)
	{
		return false;
	}

	if (the_max_length == 1)
	{
		the_user_input[0] = '\0';
		return false;
	}

	characters_remaining = (uint8_t)the_max_length - 1;

	// have cursor blink while here
	//Sys_EnableTextModeCursor(true);	// NOTE: on f256jr, this would work. would also need to set dc14-dc17 as cursor moves. skipping because already have working cursor.

	Text_SetCharAtXY(x, start_y, the_cursor_char_code);
	//gotoxy(x, SPLASH_GET_NAME_INPUT_Y);
	
	while ( (the_char = getchar() ) != CH_ENTER)
	{
		//DEBUG_OUT(("%s %d: input=%x ('%c')", __func__, __LINE__, the_char, the_char));
		
		if (the_char == CH_DEL || the_char == CH_CURS_LEFT)
		{
			*the_user_input = '\0';

			if (the_user_input != original_string) // original string was starting point of name string, so this prevents us from trying to delete past start
			{
				Text_SetCharAtXY(x, start_y, CH_SPACE);
				--x;
				Text_SetCharAtXY(x, start_y, the_cursor_char_code);
				//gotoxy(x, SPLASH_GET_NAME_INPUT_Y);

				--the_user_input;

				// we just went back in the string, so from the new point, we have more chars available
				++characters_remaining;
			}
			else
			{
				// we backed up as far as the original string (in other words, nothing)
				if (x > start_x)
				{
					Text_SetCharAtXY(x, start_y, the_cursor_char_code);
					//gotoxy(x, SPLASH_GET_NAME_INPUT_Y);
				}

				x = start_x;
			}
		}
		else
		{
			// NOTE: c64 / CBM DOS apparently allows all chars to be used, so no filter is used here
			
			*the_user_input = the_char;
			//DEBUG_OUT(("%s %d: the_user_input='%s', chrs remain=%u", __func__, __LINE__, the_user_input, characters_remaining));
			
			if (characters_remaining)
			{
				//the_char = Display_PetsciiToScreen(the_char); // we get as petscii, but display as screen codes
				//*the_user_input = the_char;  // we get as petscii, but store as screen codes
				Text_SetCharAtXY(x, start_y, the_char);
				++the_user_input;
				++x;
				Text_SetCharAtXY(x, start_y, the_cursor_char_code);
				//gotoxy(x, SPLASH_GET_NAME_INPUT_Y);
				--characters_remaining;
			}
			else
			{
				// no space to display more, so don't show the character the user typed.
				Text_SetCharAtXY(x, start_y, the_cursor_char_code);
				//gotoxy(x, SPLASH_GET_NAME_INPUT_Y);
			}
		}
	}

	*the_user_input = '\0';

	// did user end up entering anything?
	if (x == start_x)
	{
		return false;
	}

	// turn off cursor
	//Sys_EnableTextModeCursor(false);

	return true;
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// display error message, wait for user to confirm, and exit
void exit_with_wait(uint8_t the_error_number)
{
	uint8_t		x1 = 0;
	uint8_t		y1 = 0;

	printf("exit code: %u \n", the_error_number);
	
	// turn cursor back on
	Sys_EnableTextModeCursor(true);

	printf("Hit any key \n");
	getchar();
	
	//asm("JMP ($FFFC)");
	exit(0);	
}



int main(void)
{
// 	uint8_t		i;

	FILE*		in_file;
	FILE*		out_file;
	int16_t		cbm_addr;
	int16_t		addr_hi;
	int16_t		addr_lo;
	int16_t		test_addr;
	uint8_t		feedback_y = FILENAME_INPUT_Y-1; // for drawing instructions/getting input
	uint8_t		error_code = ERROR_NO_ERROR;

	// FLOW
	//  ask user for a file name
	//  try to open a file with that name
	//  if file open works, add ".bas" to filename - the out file
	//  detokenize the file, save as another file.
	
	
// 	printf("main: start \n");
		
	if (Sys_InitSystem() == false)
	{
		//asm("JMP ($FFFC)");
		exit(0);
	}
	
	// clear screen, set text mode (no overlay), turn off visual cursor
	Sys_SetBorderSize(16, 16);
	Sys_EnableTextModeCursor(false);
	Text_ClearScreen(COLOR_BRIGHT_WHITE, COLOR_BLACK);

	// DO STUFF
	// get filename from user
	printf("Enter filename of BASIC program to convert to text: \n");

	if (GetStringFromUser(in_filename, MAX_FILENAME_LEN, FILENAME_INPUT_X, ++feedback_y) == false)
	{
		// user canceled out somehow
		error_code = ERROR_FILENAME_ENTRY_ISSUE;
		goto error;
	}

	// get output filename from user
	printf("\nEnter filename to save text version under: \n\n"); // extra line spacing to get past input text
	++feedback_y;

	if (GetStringFromUser(out_filename, MAX_FILENAME_LEN, FILENAME_INPUT_X, ++feedback_y) == false)
	{
		// user canceled out somehow
		error_code = ERROR_FILENAME_ENTRY_ISSUE;
		goto error;
	}


	// try to open input file for reading
	printf("Attempting to open input file... \n");
	in_file = fopen(in_filename, "r");
	
	if (in_file == NULL)
	{
		printf("Error: could not open file for reading. \n");
		error_code = ERROR_UNABLE_TO_OPEN_INPUT_FILE;
		goto error;
	}

	// get first 2 bytes of input file - used to determine what kind of BASIC it is (2.0 vs 7.0, etc.)
	printf("Getting initial address... \n");

	addr_lo = fgetc(in_file); // low byte

	if (addr_lo < 0)
	{
		printf("Error getting 1st byte in file \n");
		error_code = ERROR_UNABLE_TO_OPEN_OUTPUT_FILE;
		goto error;
	}
	
	printf("first char of addr=%x \n", addr_lo);

	addr_hi = fgetc(in_file); // low byte

	if (addr_hi < 0)
	{
		printf("Error getting 2nd byte in file \n");
		error_code = ERROR_UNABLE_TO_OPEN_OUTPUT_FILE;
		goto error;
	}
	
	printf("2nd char of addr=%x \n", addr_hi);
	cbm_addr = addr_lo + (addr_hi << 8);

	printf("initial address=%x (%x, %x) \n", cbm_addr, addr_hi, addr_lo);

	#ifdef TRY_TO_WRITE_TO_DISK
		// test if fgetc still works after opening another file - this one should work
		addr_lo = fgetc(in_file); // low byte
		addr_hi = fgetc(in_file); // low byte
		test_addr = addr_lo + (addr_hi << 8);
		printf("first line address=%x (%x, %x); in_file=%p \n", test_addr, addr_hi, addr_lo, in_file);

		// try to open output for writing
		printf("Attempting to open output file... \n");
		out_file = fopen(out_filename, "w");

		if (out_file == NULL)
		{
			printf("Error: could not open file for writing. \n");
			error_code = ERROR_UNABLE_TO_OPEN_OUTPUT_FILE;
			goto error;
		}

		// test if fgetc still works after opening another file - this one will fail
		addr_lo = fgetc(in_file); // low byte
		addr_hi = fgetc(in_file); // low byte
		test_addr = addr_lo + (addr_hi << 8);
		printf("next 2 bytes=%x (%x, %x); in_file=%p \n", test_addr, addr_hi, addr_lo, in_file);
	#endif
	
	/* Now convert the file to text */
	printf("Converting file... \n");
	inconvert(in_file, out_file, cbm_addr);

	/* Close files */
	fclose(in_file);
	#ifdef TRY_TO_WRITE_TO_DISK
		fclose(out_file);
	#endif
	
	printf("Done \n");
	
	// jump to reset vector to get out of here and back to DOS. zzzzzap!
	//asm("jmp $FFFC");

	exit_with_wait(error_code);
	return 0;

error:	
	// check if either input or output file exists, and if so, try to close it so drive light goes out.
	if (in_file)	fclose(in_file);
	if (out_file)	fclose(out_file);
	exit_with_wait(error_code);
}