/****************************************
*
* terminal_spill_handler library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/

#include "terminal_spill_handler.h"

bool isPrintableCharacter(char ch) {
	if (ch >= 32 && ch < 127)
		return true;
	else
		return false;
}

void terminal_spill_handler::putChar(char ch)
{
	if (ch == '\n') {
		newline();
		return;
	}
	if (ch == '\t') {
		for (int i = 0; i < 4; i++) {
			beforeCurs.push_back(' ');
			cursorPosition++;
			if (cursorPosition % (screenLength - 2) == 0) {
				_cputs("\033[1M");
			}
			_putch(ch);
			return;
		}
	}
	if (!isPrintableCharacter(ch))
		throw("the given character is non-printable");
	beforeCurs.push_back(ch);
	if (isDisplayingColor) {
		if (ch == 'm')
			isDisplayingColor = false;
	}
	else
		cursorPosition++;
	if (cursorPosition % (screenLength - 2) == 0) {
		_cputs("\033[1M");
	}
	_putch(ch);
}

void terminal_spill_handler::putString(std::string str, bool displayColor)
{
	isDisplayingColor = displayColor;
	for (char& ch : str) {
		if (ch == '\033')
			displayColor = true;
		if (displayColor) {
			_putch(ch);
			if (ch == 'm')
				displayColor = false;
			continue;
		}
		try {
			putChar(ch);
		}
		catch (const char*) {
			if (displayColor && ch == '\033') {
				isDisplayingColor = true;
				_putch(ch);
			}
			
		}
	}
}

char terminal_spill_handler::eraseChar()
{
	if(cursorPosition > 0)
	{
		beforeCurs.pop_back();
		_putch(8);
		_putch(32);
		_putch(8);
		if (cursorPosition % (screenLength - 2) == 0) {
			int len = beforeCurs.length();
			for (int i = screenLength - 3; i > 0; i--) {
				_putch(beforeCurs[len - i]);
			}
		}
		cursorPosition--;
	}
	return 0;
}

std::string terminal_spill_handler::eraseString(int n)
{
	std::string ret = "";
	while(n--) {
		ret.push_back(eraseChar());
	}
	return ret;
}

void terminal_spill_handler::cursorBack()
{
	if (cursorPosition > 0)
	{
		beforeCurs.pop_back();
		_putch(8);
		if (cursorPosition % (screenLength - 2) == 0) {
			int len = beforeCurs.length();
			for (int i = screenLength - 3; i > 0; i--) {
				_putch(beforeCurs[len - i]);
			}
		}
		cursorPosition--;
	}
}

void terminal_spill_handler::cursorBack(int n)
{
	while (n--)
		cursorBack();
}

void terminal_spill_handler::newline()
{
	_putch('\n');
	cursorPosition = 0;
	beforeCurs.clear();
}

void terminal_spill_handler::newline(char ch)
{
	_putch(ch);
	cursorPosition = 0;
	beforeCurs.clear();
}
