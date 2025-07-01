#pragma once
/**************************************************
* Code by Ryuou02
* https://github.com/Ryuou02
* 
* This is set of definitions for easy use in
* some of the programs I made.
* 
* This was made for Windows terminal specifically,
* linux would require an alternative for functions 
* taken from conio.h
* 
***************************************************/
#include<conio.h>
#include<chrono>
#include<thread>
#include<regex>

#define COLOR_FG_Black "30"  
#define COLOR_BG_Black "40"
#define COLOR_FG_Red "31"  
#define COLOR_BG_Red "41"
#define COLOR_FG_Green "32"  
#define COLOR_BG_Green "42"
#define COLOR_FG_Yellow "33"  
#define COLOR_BG_Yellow "43"
#define COLOR_FG_Blue "34"  
#define COLOR_BG_Blue "44"
#define COLOR_FG_Magenta "35"  
#define COLOR_BG_Magenta "45"
#define COLOR_FG_Cyan "36"  
#define COLOR_BG_Cyan "46"
#define COLOR_FG_White "37"  
#define COLOR_BG_White "47"
#define COLOR_FG_BrightBlack "90"  
#define COLOR_BG_BrightBlack "100"
#define COLOR_FG_BrightRed "91"  
#define COLOR_BG_BrightRed "101"
#define COLOR_FG_BrightGreen "92"  
#define COLOR_BG_BrightGreen "102"
#define COLOR_FG_BrightYellow "93"  
#define COLOR_BG_BrightYellow "103"
#define COLOR_FG_BrightBlue "94"  
#define COLOR_BG_BrightBlue "104"
#define COLOR_FG_BrightMagenta "95"  
#define COLOR_BG_BrightMagenta "105"
#define COLOR_FG_BrightCyan "96"  
#define COLOR_BG_BrightCyan "106"
#define COLOR_FG_BrightWhite "97"  
#define COLOR_BG_BrightWhite "107"


#define STYLE_DEFAULT "0"		// will reset all colors as well
#define STYLE_BOLD_BRIGHT "1"
#define STYLE_NO_BOLD_BRIGHT "22"
#define STYLE_UNDERLINE "4"
#define STYLE_NO_UNDERLINE "24"
#define STYLE_NEGATIVE "7"      // swap foreground and background colors
#define STYLE_POSITIVE "27"     // reset foreground and background colors



#define TEXT_COLOR(FG_COLOR,STYLE) "\033[" FG_COLOR ";" STYLE "m"

#define SET_COLOR(FG_COLOR,BG_COLOR,STYLE) std::cout << "\033[" << FG_COLOR << ";" << BG_COLOR << ";" << STYLE << "m";		
#define RESET_COLOR _cputs("\033[0m");
#define SLEEP(COUNT) std::this_thread::sleep_for(std::chrono::milliseconds(COUNT));

#define FATAL_ERROR(E) { std::cout <<"\033[0m \033[31;40;1m\n###### FATAL ERROR : " << E << " ######\033[0m"; exit(0);}
#define ERROR_DISPLAY(E) std::cout << "\n#### ERROR : " << E << " ####\n";
#ifdef _DEBUG
#define LOG(E) {std::cout <<"\033[0m \033[34;107;1m\n LOG : " << E << " \033[0m"; } 
#endif
#ifndef _DEBUG
#define LOG(E) 
#endif




#define CTRL_A 1
#define CTRL_B 2
#define CTRL_C 3    // UNTESTED, BASED ON ASSUMPTION
#define BACK 8
#define TAB 9
#define CTRL_J 10
#define CTRL_K 11
#define CTRL_L 12
#define ENTER 13
#define CTRL_N 14
#define CTRL_O 15
#define CTRL_P 16
#define CTRL_S 19
#define CTRL_X 24
#define CTRL_Z 26
#define ESC 27
#define SPACE 32

#define SPECIAL -32
// SPECIAL RANGE
#define END 79
#define UP_ARROW 72
#define DOWN_ARROW 80
#define RIGHT_ARROW 77
#define LEFT_ARROW 75
#define HOME 71
#define PAGEUP 73
#define PAGEDOWN 81
#define DEL 83
#define CTRL_RIGHTARROW 116
#define CTRL_LEFTARROW 115
#define CTRL_DOWNARROW -111
#define CTRL_UPARROW -115
#define F12 -122

#define FUNCTION_KEY 0
// FUNCTION KEY RANGE
#define CTRL_2 3
#define F1 59
#define F2 60
#define F3 61
#define F4 62
#define F5 63
#define F6 64
#define F7 65
#define F8 66
#define F9 67
#define F10 68
#define ALT_DELETE -93
#define ALT_PAGEDOWN -95
#define ALT_DOWNARROW -96
#define ALT_END -97
#define ALT_RIGHTARROW -99
#define ALT_LEFTARROW -101
#define ALT_PAGEUP -103
#define ALT_UPARROW -104
#define ALT_HOME -105


#ifndef VALCOLOR
#define VALCOLOR

#endif
// function 11 not available for use in terminal
