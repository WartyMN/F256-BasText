/* inmode.c
 * - Routines for converting binary to text
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inmode.h"
#include "detokenize.h"
#include "select.h"

#include "basic2text.h"

// made next 2 static because cc65 doesn't like creating that much on the stack.
static char buf[256];
static char text[512];


/* inconvert
 * - performs the actual conversion
 * in:	input - open file, positioned at start of BASIC program
 * 		output - open file, to write to
 * out:	none
 */
void inconvert(FILE* in_file, FILE* out_file, int16_t cbm_addr)
{
	int16_t		expected_len;
// 	int16_t		actual_len;
	int16_t		detokenized_len;
	int16_t		nextadr;
	int16_t		addr_lo;
	int16_t		addr_hi;
	basic_t		mode;

	/* Check for valid BASIC file */
	if (cbm_addr == 0x0401 || cbm_addr == 0x0801 || cbm_addr == 0x1c01 ||
	    cbm_addr == 0x4001 || cbm_addr == 0x132D) 
	{
		mode = selectbasic(cbm_addr);

		printf("mode=%u \n", mode);

		/* If this is a combined BASIC 7.1 extension + BASIC text,
		 * skip over the header (0x132D - 0x1C00)
		 */
		// MB note: no fseek available. dunno what 7.1 extension is, so skipping.
// 		if (cbm_addr == 0x132D) {
// 			fseek(input, 0x1C01 - 0x132D, SEEK_CUR);
// 			cbm_addr = 0x1C01;
// 		}

		/* We suppose this is a valid BASIC file, so start reading it
		 * line for line.
		 * Line format is this:
		 *  [0-1]- address to next line
		 *  [2-3]- line number                     \_ sent to
		 *  [4-n]- tokenized line, null terminated /  detokenize
		 */

		/* Read address to next line */
// 		nextadr = fgetc(in_file); // low byte
// 		nextadr |= fgetc(in_file) << 8; // high byte
		addr_lo = fgetc(in_file); // low byte

		if (addr_lo < 0)
		{
			printf("Error getting 1st byte of next line address \n");
			exit_with_wait(ERROR_UNABLE_TO_OPEN_OUTPUT_FILE);
		}
	
		addr_hi = fgetc(in_file); // low byte

		if (addr_hi < 0)
		{
			printf("Error getting 2nd byte of next line address \n");
			exit_with_wait(ERROR_UNABLE_TO_OPEN_OUTPUT_FILE);
		}
	
		nextadr = addr_lo + (addr_hi << 8);

		
		if (nextadr < 0 || nextadr == 1)
		{
			exit_with_wait(ERROR_UNEXPECTED_FILE_DATA);
		}
		else if (nextadr == 0)
		{
			// no more data = end of file
		}
		else
		{
			/* Address to next line is null when the program is ended.
			 * Address to next line must be higher than the current address.
			 * The line cannot be longer than 256 bytes
			 */
			while (nextadr && nextadr > cbm_addr && nextadr - cbm_addr < 256) {
				expected_len = nextadr - cbm_addr - 2;
			
				/* Read the line into the buffer */
				if (fread(buf, 1, expected_len, in_file) != expected_len)
				{
					exit_with_wait(0);
				}
 
 				cbm_addr = nextadr;

				/* Convert to text */
				detokenized_len = detokenize(buf, text, mode);

				/* Write to output */			
				fwrite(text, 1, detokenized_len, out_file);
				
				// dump to screen
				printf("%s", text);
				
				/* Read address to next line */
				addr_lo = fgetc(in_file); // low byte
				addr_hi = fgetc(in_file); // low byte
				nextadr = addr_lo + (addr_hi << 8);
			}

			/* If nextadr != null, then the program was invalid */
			if (nextadr != 0) {
				exit_with_wait(ERROR_INVALID_BASIC_FILE);
			}
		}	
	}
	else {
		exit_with_wait(ERROR_INVALID_BASIC_START_ADDRESS);
	}
}