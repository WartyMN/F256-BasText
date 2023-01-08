#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


/* inconvert
 * - performs the actual conversion
 * in:	input - open file, positioned at start of BASIC program
 * 		output - open file, to write to
 * out:	none
 */
void inconvert(FILE* in_file, FILE* output_fd, int16_t cbm_addr);


#endif /* INMODE_H */
