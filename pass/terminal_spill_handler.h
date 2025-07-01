/****************************************
*
* terminal_spill_handler library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/
#include<Windows.h>
#include<conio.h>
#include<string>

#pragma once

inline int getScreenLength() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
	GetConsoleScreenBufferInfo(hOut, &ScreenBufferInfo);
	int sl = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
	return sl;
}

class terminal_spill_handler
{
	int screenLength;
	int totalSegments, currentSegment, cursorPosition;
	std::string beforeCurs;
	
public:
	// variable to tell commandline whether some invisible characters are currently being printed
	bool isDisplayingColor;

	terminal_spill_handler(int screenLength = getScreenLength()) : screenLength(screenLength) ,isDisplayingColor(false) {
		beforeCurs = "";
		currentSegment = 0;
		totalSegments = 0;
		cursorPosition = 0;
	}

	// get all that is currently written before the cursor
	std::string getLine() {
		return beforeCurs;
	}

	// adding characters to terminal
	void putChar(char ch);
	void putString(std::string str, bool displayColor = false);

	// removing characters from terminal
	char eraseChar();
	std::string eraseString(int n/*number of characters to erase*/);

	// moving cursor backward on the screen
	void cursorBack();
	void cursorBack(int n);

	void newline();
	void newline(char ch);

};

bool isPrintableCharacter(char ch);

