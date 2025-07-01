#include "colorCodes.h"
#pragma once

#define PROGRAMASSIST_TITLE		"PASSv2.0"								                        // title of application here. It is allowed to be multiline.
#define PROGRAM_COLOR           TEXT_COLOR(COLOR_FG_BrightGreen,STYLE_BOLD_BRIGHT)		    // color of the text that the main interactive program should display in
#define USER_COLOR	            TEXT_COLOR(COLOR_FG_Cyan,STYLE_BOLD_BRIGHT)			        // color of the text that the user should write in
#define PROGRAM_2_COLOR			TEXT_COLOR(COLOR_FG_BrightCyan,STYLE_BOLD_BRIGHT)           // color of the text that the secondary program should display in
#define ERROR_COLOR	            TEXT_COLOR(COLOR_FG_Yellow,STYLE_UNDERLINE)					// errors of less importance will be displayed in this color
#define ERROR_FATAL_COLOR	    TEXT_COLOR(COLOR_FG_BrightRed,STYLE_BOLD_BRIGHT)	        // errors that could potentailly will be displayed in this color.
#define INFO_COLOR              TEXT_COLOR(COLOR_FG_BrightBlue,STYLE_BOLD_BRIGHT)           // color for displaying information
#define VERSION                 0                                                           // version number of the program

// ERROR CODES
#define ERROR_MSG 0
#define FATAL_ERROR_MSG 1
