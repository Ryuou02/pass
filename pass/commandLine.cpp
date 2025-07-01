/****************************************
*
* CommandLine library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/
#include "commandLine.h"


/*
*  Function to validate the terminal configurations file (.tfc)
*	correct format for file -
* --tfc
* <display title (can be ascii art)>
* ~EODT
* <input_line>
* <output_line>
* <input_color>
* <output_color>
* <error_color>
* <fatal_error_color>
* <verbose_text_color>
* <info_text_color>
* <valid_input_color>
* `` any comment, must start with '``' at beginning of line
*/


/*
* A simple method for your convenience to split the string into a vector of strings
* @param string that needs to be split (string)
* @param split character, based on which the string needs to be split. Takes default value of ' ' [space] (character)
* @return vector of strings after splitting
*/

std::vector<std::string> splitString(std::string& str, char splch) {
	std::vector<std::string> splits;
	std::string s = "";
	for (char ch : str) {
		if (ch == splch) {
			splits.push_back(s);
			s = "";
		}
		else {
			s.push_back(ch);
		}
	}
	splits.push_back(s);
	return splits;
}

/*
* Set the configurations for the commandLine object from a valid terminal configuration file.
* @param filename of a valid tfc configurations file to read the configurations from (string)
* @return none
*/
void commandLine::setConfigurations(std::string filename)
{
	std::ifstream fileread;
	std::string temp;
	fileread.open(filename);
	if (fileread.fail())
		throw("file doesn't exist");
	getline(fileread, temp);

	configVariables[terminalConfig.displayTitle] = "";
	if (temp != "--tfc")
		throw("invalid input file");
	getline(fileread, temp);
	while (temp != "~#EODT") {
		if (fileread.eof())
			throw("invalid input file");
		configVariables[terminalConfig.displayTitle] += temp + '\n';
		getline(fileread, temp);
	}

	for (int i = 1; i < sizeof(terminalConfig) / sizeof(int); i++) {
		getline(fileread, temp);
		if (fileread.eof())
			throw("invalid input file");
		while (temp[0] == '`' && temp[1] == '`') {	// ignore comments
			getline(fileread, temp);
			if (fileread.eof())
				throw("invalid input file");
		}

		if (i > 2 && !validateColor(temp))
			throw("invalid input file");
		configVariables[i] = temp;
	}

	fileread.close();
}

/*
* Print the display title directly without setting it first.
* @param name : what is to be printed as display title
* @return none
*/
void commandLine::printDisplayTitle(std::string name)
{
	if (name == "")
		name = configVariables[terminalConfig.displayTitle];
	RESET_COLOR;
	std::string color;
	term.putString(name);
	term.newline();
}

/*
* write commandLine object configurations into a file.
* @param filename: string
*/
void commandLine::writeConfigurations(std::string filename)
{
	std::ofstream filewrite;
	filewrite.open(filename);
	filewrite << "--tfc\n" << configVariables[terminalConfig.displayTitle] << "\n~#EODT\n";
	for (int i = 1; i < sizeof(terminalConfig) / sizeof(int); i++) {
		filewrite << configVariables[i] << '\n';
	}
	filewrite.close();

}


/*
* @param none
* @return command given as input by the user (string)
*/
std::string commandLine::getCommand()
{
	recentlyDisplayedVerbose = false;
	line_len = 0;
	command.clear();
	printLineStart();

	char in = 0;
	int len;	// temporary length variable
	bool wordExisted = false;

	int guessLength = -1;

	do {
		RESET_COLOR;
		term.putString(configVariables[terminalConfig.inputTextColor]);
		in = _getch();
		switch (in) {
		case ENTER:
			moveCursorForward(commandNext.length());
			obj->clearWordBuffer();
			break;

		case SPACE:
			updateAutoCompleteObj();
			putSpace();
			break;

		case BACK:	//backspace
			if (line_len > 0) {
				removeCommandBack();
				term.eraseChar();		//backspace on terminal
				if (!obj->backspace())	// refer documentation of autcomplete backspace() to understand this
				{
					//wordToACbuffer(*obj, command);
					reconstructAutocompleteObj(command);
				}
			}
			break;

		case TAB:
			doAutoComplete();
			break;

		case ESC:	// exit the program
			throw(EXIT_PROGRAM);
			in = ENTER;
			break;

		case SPECIAL:
			in = _getch();

			switch (in) {
			case UP_ARROW:
				len = getPrevLinePos();
				moveCursorForward(commandNext.length());
				term.eraseString(command.length());
				command = getPrevCommand();

				term.putString(command);
				line_len = command.length();
				moveCursorBack(command.length() - len);
				break;

			case DOWN_ARROW:
				len = getNextLinePos();
				moveCursorForward(commandNext.length());
				term.eraseString(command.length());
				command = getNextCommand();

				term.putString(command);
				line_len = command.length();
				moveCursorBack(command.length() - len);

				break;

			case END:
				moveCursorForward(commandNext.length());
				commandNext.clear();
				break;

			case HOME:
				moveCursorBack(line_len);
				break;

			case LEFT_ARROW:
				moveCursorBack();
				break;

			case RIGHT_ARROW:
				moveCursorForward();
				break;
			default:
				throw(int(in));
			}
			break;
		default:	// normal alphabet/numeric
			if (in < 32)
				throw(int(in));
			obj->addChar(in);
			term.putChar(in);
			addCharToCommand(in);
		}



		// code for only text formatting purposes

		if (!commandNext.empty()) {
			len = commandNext.length();
			for (int i = 0; i < len; i++) {
				term.putChar(commandNext[len - i - 1]);
			}
			term.putChar(SPACE);	// suppose an extra character exists which needs to be erased
			term.cursorBack(len + 1);
		}
		else {
			if (obj->guessExists()) {
				writeColored(obj->getGuessWriteString(), "\033[2m");
				if (obj->getBuffer().length() > 0)
					term.cursorBack(obj->getGuessWriteString().length());
				else
					term.eraseString(obj->getGuessWriteString().length());

				RESET_COLOR;
				guessLength = obj->getGuessWriteString().length();
			}
			else if (guessLength > 0) {
				for (int i = 0; i < guessLength - 1; i++)
					term.putChar(SPACE);
				term.cursorBack(guessLength - 1);
				guessLength = -1;
			}
		}

		if (obj->wordExists(obj->getBuffer())) {
			rewriteColoured(obj->getBuffer(), configVariables[terminalConfig.validInputColor].c_str());
			wordExisted = true;
			RESET_COLOR;
		}
		else if (wordExisted) {
			rewriteColoured(obj->getBuffer(), configVariables[terminalConfig.inputTextColor].c_str());
			wordExisted = false;
		}
	} while (in != ENTER);
	prevCommands.push(command);
	prevCursPos.push(line_len);
	return command;
}

