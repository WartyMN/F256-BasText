/*
 * text.c
 *
 *  Created on: Feb 19, 2022
 *      Author: micahbly
 */


// This is a cut-down, semi-API-compatible version of the OS/f text.c file from Lich King (Foenix)
// adapted for Foenix F256 Jr starting November 29, 2022



/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lk_text.h"
#include "lk_sys.h"

// C includes
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cc65 includes


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

int8_t*	global_temp_buff_384b;

extern System*			global_system;


/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// Fill attribute or text char memory. Writes to char memory if for_attr is false.
// calling function must validate the screen ID before passing!
//! @return	Returns false on any error/invalid input.
bool Text_FillMemory(bool for_attr, uint8_t the_fill);

//! Fill character and attribute memory for a specific box area
//! calling function must validate screen id, coords, attribute value before passing!
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	width: width, in character cells, of the rectangle to be filled
//! @param	height: height, in character cells, of the rectangle to be filled
//! @param	the_attribute_value: a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_FillMemoryBoxBoth(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t the_char, uint8_t the_attribute_value);

//! Fill character OR attribute memory for a specific box area
//! calling function must validate screen id, coords, attribute value before passing!
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	width: width, in character cells, of the rectangle to be filled
//! @param	height: height, in character cells, of the rectangle to be filled
//! @param	for_attr: true to work with attribute data, false to work character data. Recommend using SCREEN_FOR_TEXT_ATTR/SCREEN_FOR_TEXT_CHAR.
//! @param	the_fill: either a 1-byte character code, or a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_FillMemoryBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool for_attr, uint8_t the_fill);

/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// **** NOTE: all functions in private section REQUIRE pre-validated parameters. 
// **** NEVER call these from your own functions. Always use the public interface. You have been warned!

//! Fill attribute or text char memory. 
//! calling function must validate the screen ID before passing!
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	for_attr: true to work with attribute data, false to work character data. Recommend using SCREEN_FOR_TEXT_ATTR/SCREEN_FOR_TEXT_CHAR.
//! @param	the_fill: either a 1-byte character code, or a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_FillMemory(bool for_attr, uint8_t the_fill)
{
	uint16_t	the_write_len;
	uint8_t*	the_write_loc;

	// LOGIC: 
	//   On F256jr, the write len and write locs are same for char and attr memory, difference is IO page 2 or 3

	if (for_attr)
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	}
	else
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	}

	the_write_len = PHYSICAL_SCREEN_TOTAL_BYTES;
	the_write_loc = (uint8_t*)SCREEN_TEXT_MEMORY_LOC;
	memset(the_write_loc, the_fill, the_write_len);
		
	Sys_RestoreIOPage();

	//printf("Text_FillMemory: done \n");
	//DEBUG_OUT(("%s %d: done (for_attr=%u, the_fill=%u)", __func__, __LINE__, for_attr, the_fill));

	return true;
}


//! Fill character and attribute memory for a specific box area
//! calling function must validate screen id, coords, attribute value before passing!
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	width: width, in character cells, of the rectangle to be filled
//! @param	height: height, in character cells, of the rectangle to be filled
//! @param	the_attribute_value: a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_FillMemoryBoxBoth(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t the_char, uint8_t the_attribute_value)
{
	uint8_t*	the_write_loc;
	uint8_t		max_row;

	// LOGIC: 
	//   On F256jr, the write len and write locs are same for char and attr memory, difference is IO page 2 or 3

	// set up initial loc
	the_write_loc = Text_GetMemLocForXY(x, y);
	
	max_row = y + height;
	
	for (; y <= max_row; y++)
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
		memset(the_write_loc, the_attribute_value, width);
		Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
		memset(the_write_loc, the_char, width);

		the_write_loc += SCREEN_NUM_COLS;
	}
		
	Sys_RestoreIOPage();
			
	return true;
}


//! Fill character OR attribute memory for a specific box area
//! calling function must validate screen id, coords, attribute value before passing!
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	width: width, in character cells, of the rectangle to be filled
//! @param	height: height, in character cells, of the rectangle to be filled
//! @param	for_attr: true to work with attribute data, false to work character data. Recommend using SCREEN_FOR_TEXT_ATTR/SCREEN_FOR_TEXT_CHAR.
//! @param	the_fill: either a 1-byte character code, or a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_FillMemoryBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool for_attr, uint8_t the_fill)
{
	uint8_t*	the_write_loc;
	uint8_t		max_row;

	// LOGIC: 
	//   On F256jr, the write len and write locs are same for char and attr memory, difference is IO page 2 or 3

	if (for_attr)
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	}
	else
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	}

	// set up initial loc
	the_write_loc = Text_GetMemLocForXY(x, y);
	
	max_row = y + height;
	
	for (; y <= max_row; y++)
	{
		memset(the_write_loc, the_fill, width);
		the_write_loc += SCREEN_NUM_COLS;
	}
		
	Sys_RestoreIOPage();
			
	return true;
}



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory. It works on the basis of a screen ID, which corresponds to the text memory for Vicky's Channel A and Channel B video memory.


// **** Block copy functions ****


//! Copy a rectangular area of text or attr to or from an off-screen buffer
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	the_buffer: valid pointer to a block of memory big enough to store (or alternatively act as the source of) the character or attribute data for the specified rectangle of screen memory.
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	to_screen: true to copy to the screen from the buffer, false to copy from the screen to the buffer. Recommend using SCREEN_COPY_TO_SCREEN/SCREEN_COPY_FROM_SCREEN.
//! @param	for_attr: true to work with attribute data, false to work character data. Recommend using SCREEN_FOR_TEXT_ATTR/SCREEN_FOR_TEXT_CHAR.
//! @return	Returns false on any error/invalid input.
bool Text_CopyMemBox(uint8_t* the_buffer, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool to_screen, bool for_attr)
{
	uint8_t*		the_vram_loc;
	uint8_t*		the_buffer_loc;
	uint8_t			the_write_len;
	int16_t			initial_offset;

	// LOGIC: 
	//   On F256jr, the write len and write locs are same for char and attr memory, difference is IO page 2 or 3

	if (for_attr)
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	}
	else
	{
		Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	}
		
	// get initial read/write locs
	initial_offset = (SCREEN_NUM_COLS * y1) + x1;
	the_buffer_loc = the_buffer + initial_offset;
	the_write_len = x2 - x1 + 1;

	the_vram_loc = (uint8_t*)SCREEN_TEXT_MEMORY_LOC + initial_offset;
	
	// do copy one line at a time	

//DEBUG_OUT(("%s %d: vramloc=%p, buffer=%p, bufferloc=%p, to_screen=%i, the_write_len=%i", the_vram_loc, the_buffer, the_buffer_loc, to_screen, the_write_len));

	for (; y1 <= y2; y1++)
	{
		if (to_screen)
		{
			memcpy(the_vram_loc, the_buffer_loc, the_write_len);
		}
		else
		{
			memcpy(the_buffer_loc, the_vram_loc, the_write_len);
		}
		
		the_buffer_loc += SCREEN_NUM_COLS;
		the_vram_loc += SCREEN_NUM_COLS;
	}
		
	Sys_RestoreIOPage();

	return true;
}



// **** Block fill functions ****



//! Clear the text screen and reset foreground and background colors
void Text_ClearScreen(uint8_t fore_color, uint8_t back_color)
{
	uint8_t			the_attribute_value;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);
	//DEBUG_OUT(("%s %d: the_attribute_value=%u", __func__, __LINE__, the_attribute_value));

	Text_FillMemory(SCREEN_FOR_TEXT_CHAR, ' ');
	Text_FillMemory(SCREEN_FOR_TEXT_ATTR, the_attribute_value);
}


//! Fill character and attribute memory for a specific box area
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_char: the character to be used for the fill operation
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_FillBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t the_char, uint8_t fore_color, uint8_t back_color)
{
	uint8_t		dy;
	uint8_t		dx;
	uint8_t		the_attribute_value;

 	// add 1 to H line len, because dx becomes width, and if width = 0, then memset gets 0, and nothing happens.
	// dy can be 0 and you still get at least one row done.
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 0;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	return Text_FillMemoryBoxBoth(x1, y1, dx, dy, the_char, the_attribute_value);
}


//! Fill character memory for a specific box area
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_char: the character to be used for the fill operation
//! @return	Returns false on any error/invalid input.
bool Text_FillBoxCharOnly(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t the_char)
{
	uint8_t		dy;
	uint8_t		dx;
	
	if (x1 > x2 || y1 > y2)
	{
		return false;
	}

 	// add 1 to H line len, because dx becomes width, and if width = 0, then memset gets 0, and nothing happens.
	// dy can be 0 and you still get at least one row done.
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 0;

	return Text_FillMemoryBox(x1, y1, dx, dy, SCREEN_FOR_TEXT_CHAR, the_char);
}


//! Fill attribute memory for a specific box area
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_FillBoxAttrOnly(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fore_color, uint8_t back_color)
{
	uint8_t			dy;
	uint8_t			dx;
	uint8_t			the_attribute_value;
	
	if (x1 > x2 || y1 > y2)
	{
		return false;
	}

 	// add 1 to H line len, because dx becomes width, and if width = 0, then memset gets 0, and nothing happens.
	// dy can be 0 and you still get at least one row done.
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 0;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	return Text_FillMemoryBox(x1, y1, dx, dy, SCREEN_FOR_TEXT_ATTR, the_attribute_value);
}


//! Invert the colors of a rectangular block.
//! As this requires sampling each character cell, it is no faster (per cell) to do for entire screen as opposed to a subset box
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @return	Returns false on any error/invalid input.
bool Text_InvertBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint8_t			the_attribute_value;
	uint8_t			the_inversed_value;
	uint8_t*		the_write_loc;
	uint8_t			the_col;
	uint8_t			skip_len;
	uint8_t			back_nibble;
	uint8_t			fore_nibble;
	
	// get initial read/write loc
	the_write_loc = Text_GetMemLocForXY(x1, y1);	
	
	// amount of cells to skip past once we have written the specified line len
	skip_len = SCREEN_NUM_COLS - (x2 - x1) - 1;

	Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	
	for (; y1 <= y2; y1++)
	{
		for (the_col = x1; the_col <= x2; the_col++)
		{
			the_attribute_value = (unsigned char)*the_write_loc;
			
			// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
			back_nibble = ((the_attribute_value & 0xF0) >> 4);
			fore_nibble = ((the_attribute_value & 0x0F) << 4);
			the_inversed_value = (fore_nibble | back_nibble);
			
			*the_write_loc++ = the_inversed_value;
		}

		the_write_loc += skip_len;
	}
		
	Sys_RestoreIOPage();

	return true;
}




// **** FONT RELATED *****

//! replace the current font data with the data at the passed memory buffer
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	new_font_data: Pointer to 2K (256 characters x 8 lines/bytes each) of font data. Each byte represents one line of an 8x8 font glyph.
//! @return	Returns false on any error/invalid input.
bool Text_UpdateFontData(char* new_font_data)
{
	if (new_font_data == NULL)
	{
		return false;
	}

	Sys_SwapIOPage(VICKY_IO_PAGE_FONT_AND_LUTS);

	memcpy((uint8_t*)FONT_MEMORY_BANK, new_font_data, (8*256));
		
	Sys_RestoreIOPage();

	return true;
}



// **** Set char/attr functions *****


//! Set a char at a specified x, y coord
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_char: the character to be used
//! @return	Returns false on any error/invalid input.
bool Text_SetCharAtXY(uint8_t x, uint8_t y, uint8_t the_char)
{
	uint8_t*	the_write_loc;
	
	Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	
	the_write_loc = Text_GetMemLocForXY(x, y);	
	*the_write_loc = the_char;
		
	Sys_RestoreIOPage();
	
	return true;
}


//! Set the attribute value at a specified x, y coord
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_attribute_value: a 1-byte attribute code (foreground in high nibble, background in low nibble)
//! @return	Returns false on any error/invalid input.
bool Text_SetAttrAtXY(uint8_t x, uint8_t y, uint8_t the_attribute_value)
{
	uint8_t*	the_write_loc;
	
	Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	
	the_write_loc = Text_GetMemLocForXY(x, y);	
	*the_write_loc = the_attribute_value;
		
	Sys_RestoreIOPage();
	
	return true;
}


//! Set the attribute value at a specified x, y coord based on the foreground and background colors passed
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_SetColorAtXY(uint8_t x, uint8_t y, uint8_t fore_color, uint8_t back_color)
{
	uint8_t			the_attribute_value;

	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);
	
	return Text_SetAttrAtXY(x, y, the_attribute_value);
}


//! Draw a char at a specified x, y coord, also setting the color attributes
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_char: the character to be used
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_SetCharAndAttrAtXY(uint8_t x, uint8_t y, uint8_t the_char, uint8_t the_attribute_value)
{
	uint8_t*		the_write_loc;

	the_write_loc = Text_GetMemLocForXY(x, y);	
	
	Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	*the_write_loc = the_attribute_value;
	Sys_RestoreIOPage();

	Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	*the_write_loc = the_char;
	Sys_RestoreIOPage();
	
	return true;
}


//! Draw a char at a specified x, y coord, also setting the color attributes
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_char: the character to be used
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_SetCharAndColorAtXY(uint8_t x, uint8_t y, uint8_t the_char, uint8_t fore_color, uint8_t back_color)
{
	uint8_t*		the_write_loc;
	uint8_t			the_attribute_value;
			
	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	the_write_loc = Text_GetMemLocForXY(x, y);	
	
	Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);
	*the_write_loc = the_attribute_value;
	Sys_RestoreIOPage();

	Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);
	*the_write_loc = the_char;
	Sys_RestoreIOPage();
	
	return true;
}


// //! Set a char at a y*80+x screen index point. 
// bool Text_SetCharAtMemLoc(uint16_t the_write_loc, uint8_t the_char)
// {
// 	the_write_loc += SCREEN_TEXT_MEMORY_LOC;
// 	
// 	*(uint8_t*)the_write_loc = the_char;
// 	
// 	return true;
// }




// **** Get char/attr functions *****





// **** Drawing functions *****


//! Draws a horizontal line from specified coords, for n characters, using the specified char and/or attribute
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_line_len: The total length of the line, in characters, including the start and end character.
//! @param	the_char: the character to be used when drawing
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	the_draw_choice: controls the scope of the action, and is one of CHAR_ONLY, ATTR_ONLY, or CHAR_AND_ATTR. See the text_draw_choice enum.
//! @return	Returns false on any error/invalid input.
void Text_DrawHLine(uint8_t x, uint8_t y, uint8_t the_line_len, uint8_t the_char, uint8_t fore_color, uint8_t back_color, uint8_t the_draw_choice)
{
	uint8_t			the_attribute_value;
	
	// LOGIC: 
	//   an H line is just a box with 1 row, so we can re-use Text_FillMemoryBox(Both)(). These routines use memset, so are quicker than for loops. 
	
	if (the_draw_choice == CHAR_ONLY)
	{
		Text_FillMemoryBox(x, y, the_line_len, 0, SCREEN_FOR_TEXT_CHAR, the_char);
	}
	else
	{
		// calculate attribute value from passed fore and back colors
		// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground

		the_attribute_value = ((fore_color << 4) | back_color);
	
		if (the_draw_choice == ATTR_ONLY)
		{
			Text_FillMemoryBox(x, y, the_line_len, 0, SCREEN_FOR_TEXT_ATTR, the_attribute_value);
		}
		else
		{
			Text_FillMemoryBoxBoth(x, y, the_line_len, 0, the_char, the_attribute_value);
		}
	}
}


//! Draws a vertical line from specified coords, for n characters, using the specified char and/or attribute
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_line_len: The total length of the line, in characters, including the start and end character.
//! @param	the_char: the character to be used when drawing
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	the_draw_choice: controls the scope of the action, and is one of CHAR_ONLY, ATTR_ONLY, or CHAR_AND_ATTR. See the text_draw_choice enum.
//! @return	Returns false on any error/invalid input.
void Text_DrawVLine(uint8_t x, uint8_t y, uint8_t the_line_len, uint8_t the_char, uint8_t fore_color, uint8_t back_color, uint8_t the_draw_choice)
{
	uint8_t		dy;
	
	switch (the_draw_choice)
	{
		case CHAR_ONLY:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetCharAtXY(x, y + dy, the_char);
			}
			break;
			
		case ATTR_ONLY:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetColorAtXY(x, y + dy, fore_color, back_color);
			}
			break;
			
		case CHAR_AND_ATTR:
		default:
			for (dy = 0; dy < the_line_len; dy++)
			{
				Text_SetCharAndColorAtXY(x, y + dy, the_char, fore_color, back_color);		
			}
			break;			
	}
}


//! Draws a box based on 2 sets of coords, using the predetermined line and corner "graphics", and the passed colors
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x1: the leftmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y1: the uppermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	x2: the rightmost horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y2: the lowermost vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
void Text_DrawBoxCoordsFancy(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fore_color, uint8_t back_color)
{
	uint8_t		dy;
	uint8_t		dx;
	
	//DEBUG_OUT(("%s %d: %u, %u x %u, %u", __func__, __LINE__, x1, y1, x2, y2));
	
// 	// add 1 to H line len, because dx becomes width, and if width = 0, then memset gets 0, and nothing happens.
	// dy can be 0 and you still get at least one row done.
	// but, for this, because of how we draw H line, do NOT add 1 to x1. see "x1+1" below... 
	dx = x2 - x1 + 0;
	dy = y2 - y1 + 0;
	
	// draw all lines one char shorter on each end so that we don't overdraw when we do corners
	
	Text_DrawHLine(x1+1, y1, dx, CH_WALL_H, fore_color, back_color, CHAR_AND_ATTR);
	Text_DrawHLine(x1+1, y2, dx, CH_WALL_H, fore_color, back_color, CHAR_AND_ATTR);
	Text_DrawVLine(x2, y1+1, dy, CH_WALL_V, fore_color, back_color, CHAR_AND_ATTR);
	Text_DrawVLine(x1, y1+1, dy, CH_WALL_V, fore_color, back_color, CHAR_AND_ATTR);
	
	// draw the 4 corners with dedicated corner pieces
	Text_SetCharAndColorAtXY(x1, y1, CH_WALL_UL, fore_color, back_color);		
	Text_SetCharAndColorAtXY(x2, y1, CH_WALL_UR, fore_color, back_color);		
	Text_SetCharAndColorAtXY(x2, y2, CH_WALL_LR, fore_color, back_color);		
	Text_SetCharAndColorAtXY(x1, y2, CH_WALL_LL, fore_color, back_color);		
}



// **** Draw string functions *****


//! Draw a string at a specified x, y coord, also setting the color attributes.
//! If it is too long to display on the line it started, it will be truncated at the right edge of the screen.
//! No word wrap is performed. 
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the starting horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the starting vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	the_string: the null-terminated string to be drawn
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	Returns false on any error/invalid input.
bool Text_DrawStringAtXY(uint8_t x, uint8_t y, char* the_string, uint8_t fore_color, uint8_t back_color)
{
	uint8_t*		the_char_loc;
	uint8_t*		the_attr_loc;
	uint8_t			the_attribute_value;
	uint8_t			i;
	uint8_t			max_col;
	uint8_t			draw_len;
	
	draw_len = strlen(the_string); // can't be wider than the screen anyway
	max_col = SCREEN_NUM_COLS - 1;
	
	if (x + draw_len > max_col)
	{
		draw_len = (max_col - x) + 1;
	}
	
	//DEBUG_OUT(("%s %d: draw_len=%i, max_col=%i, x=%i", __func__, __LINE__, draw_len, max_col, x));
	//printf("%s %d: draw_len=%i, max_col=%i, x=%i \n", __func__, __LINE__, draw_len, max_col, x);
	
	// calculate attribute value from passed fore and back colors
	// LOGIC: text mode only supports 16 colors. lower 4 bits are back, upper 4 bits are foreground
	the_attribute_value = ((fore_color << 4) | back_color);

	// set up char and attribute memory initial loc
	the_attr_loc = the_char_loc = Text_GetMemLocForXY(x, y);

	//printf("%s %d: the_char_loc=%p, *charloc=%u \n", __func__, __LINE__, the_char_loc, *the_char_loc);
	//printf("%s %d: string=%s \n", __func__, __LINE__, the_string);
	
	// draw the string
	Sys_SwapIOPage(VICKY_IO_PAGE_CHAR_MEM);

	for (i = 0; i < draw_len; i++)
	{
		*the_char_loc++ = the_string[i];
	}
		
	Sys_RestoreIOPage();

	// draw the attributes

	Sys_SwapIOPage(VICKY_IO_PAGE_ATTR_MEM);

	memset(the_attr_loc, the_attribute_value, draw_len);
		
	Sys_RestoreIOPage();
	
	return true;
}



// **** Plotting functions ****


//! Calculate the VRAM location of the specified coordinate
//! @param	the_screen: valid pointer to the target screen to operate on
//! @param	x: the horizontal position, between 0 and the screen's text_cols_vis_ - 1
//! @param	y: the vertical position, between 0 and the screen's text_rows_vis_ - 1
//! @param	for_attr: true to work with attribute data, false to work character data. Recommend using SCREEN_FOR_TEXT_ATTR/SCREEN_FOR_TEXT_CHAR.
uint8_t* Text_GetMemLocForXY(uint8_t x, uint8_t y)
{
	uint8_t*	the_write_loc;
	uint16_t	initial_offset;
	
	// LOGIC:
	//   For plotting the VRAM, A2560 uses the full width, regardless of borders. 
	//   So even if only 72 are showing, the screen is arranged from 0-71 for row 1, then 80-151 for row 2, etc. 
	
	initial_offset = (SCREEN_NUM_COLS * y) + x;
	the_write_loc = (uint8_t*)SCREEN_TEXT_MEMORY_LOC + initial_offset;
	
	//DEBUG_OUT(("%s %d: screen=%i, x=%i, y=%i, for-attr=%i, calc=%i, loc=%p", __func__, __LINE__, (int16_t)the_screen_id, x, y, for_attr, (the_screen->text_mem_cols_ * y) + x, the_write_loc));
	
	return the_write_loc;
}




// **** "Text Window" Functions ****
// **** Move these back into OS/f Text Library in the future!




