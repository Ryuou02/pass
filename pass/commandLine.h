/****************************************
*
* CommandLine library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/
#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H


#include "autocomplete.h"
#include "colorCodes.h"
#include "terminal_spill_handler.h"
#include<fstream>
#include<map>



#define EXIT_PROGRAM 100001 // error code for exit program

// check whtether the color is valid or not.
inline bool validateColor(std::string inputColor) {
	return regex_match(inputColor, std::regex("\\\033\\[\\d+(;\\d+)?(;\\d+)?m"));
}

// this is just used to make naming more understandable, is not used for anything else
constexpr struct config {
	int displayTitle,			// display text can be ascii art
		lineStart,				// characters to display at beginning of every input line
		outputLineStart,		// characters to display at beginning of every output line
		inputTextColor,			// color of input
		outputTextColor,		// color of normal output
		errorTextColor,			// color of warning/non-fatal error messages
		fatalErrorTextColor,	// color of fatal error messages
		verboseTextColor,		// color of verbose statements
		infoTextColor,			// color of information messages
		validInputColor;		// color of text to be changed to in case it is a valid command
}terminalConfig{0,1,2,3,4,5,6,7,8,9};


// split a string into multiple strings separated by splch character
std::vector<std::string> splitString(std::string&, char splch = ' ');

class commandLine
{
	terminal_spill_handler term;
	map<std::string, autocomplete> validCommReplacement;
	std::vector<std::string> configVariables;
	
	autocomplete *obj, defaultObj;
	int line_len;
	std::string command, commandNext;
	

	void rewriteColoured(string ogString, const char* color) {
		int len = ogString.length();
		term.cursorBack(len);
		term.putString(color);
		term.putString(ogString);
	}
	void writeColored(string s1, const char* color) {
		term.putString(color);
		term.putString(s1);
	}
	void addCharToCommand(char ch) {
		line_len += 1;
		command += ch;
	}
	void addStringToCommand(std::string str) {
		line_len += str.length();
		command += str;
	}
	char removeCommandBack() {		char c = command.back();
		command.pop_back();
		line_len--;
		return c;
	}
	std::string removeCommandBack(int n) {
		if (n > line_len)
			throw("illegal operation");
		std::string c = "";
		for (int i = 0; i < n; i++) {
			c.push_back(command.back());
			command.pop_back();
		}
		line_len -= n;
		return c;
	}
	void putSpace() {
		term.putChar(SPACE);
		obj->clearWordBuffer();
		addCharToCommand(SPACE);
	}
	void doAutoComplete() {
		term.putString(obj->getGuessWriteString());
		addStringToCommand(obj->getGuessWriteString());
		obj->fillBuffer();
	}
	
	void moveCursorBack() {
		if (!command.empty()) {
			term.cursorBack();

			commandNext.push_back(removeCommandBack());
			reconstructAutocompleteObj(command);
		}
	}
	void moveCursorBack(int steps) {
		term.cursorBack(steps);
		commandNext += removeCommandBack(steps);	
		reconstructAutocompleteObj(command);
	}

	void moveCursorForward() {
		if (!commandNext.empty()) {
			term.putChar(commandNext.back());

			addCharToCommand(commandNext.back());
			commandNext.pop_back();
			wordToACbuffer(*obj, command);
		}
	}
	void moveCursorForward(int steps) {
		for (int i = 0; i < steps && !commandNext.empty(); i++)
		{
			addCharToCommand(commandNext.back());
			term.putChar(commandNext.back());
			commandNext.pop_back();
		}
		if(!command.empty())
			//wordToACbuffer(*obj, command);
			reconstructAutocompleteObj(command);
	}


	void reconstructAutocompleteObj(std::string command) {
		delete this->obj;
		this->obj = new autocomplete(defaultObj);
		if (command.size() > 0) {
			for (char ch : command) {
				this->obj->addChar(ch);
				if (ch == SPACE) {
					updateAutoCompleteObj();
					obj->clearWordBuffer();
				}
			}
		}
	}

	void updateAutoCompleteObj() {	// based on the word buffer, switch to the appropriate autocomplete object in the map.
		if (this->obj->wordExists(obj->getBuffer()))
			if (validCommReplacement.find(obj->getBuffer()) != validCommReplacement.end())
				changeAutoComplete(validCommReplacement[obj->getBuffer()]);
	}

	std::string getPrevCommand() {
		std::string comm = command;
		if(!prevCommands.empty()) {
			nextCommands.push(command);
			comm = prevCommands.top();
			prevCommands.pop();
		}
		return comm;
	}
	int getPrevLinePos() {
		int pos = line_len;
		if (!prevCursPos.empty()){
			pos = prevCursPos.top();
			nextCursPos.push(line_len);
			pos = prevCursPos.top();
			prevCursPos.pop();
		}
		return pos;
	}

	std::string getNextCommand() {
		std::string comm = command;
		if (!nextCommands.empty()) {
			prevCommands.push(command);
			comm = nextCommands.top();
			nextCommands.pop();
		}
		return comm;
	}
	int getNextLinePos() {
		int pos = line_len;
		if (!nextCursPos.empty()) {
			pos = nextCursPos.top();
			prevCursPos.push(line_len);
			pos = nextCursPos.top();
			nextCursPos.pop();
		}
		return pos;
	}

	std::stack<std::string> prevCommands;
	std::stack<int> prevCursPos;

	std::stack<std::string> nextCommands;
	std::stack<int> nextCursPos;

	bool recentlyDisplayedVerbose;


public:
	/* Initialize commandLine object with set of valid commands and number of commands
	* @param commands[] : list of commands
	* @param commandLength : number of commands in the list
	*/
	commandLine(const char* commands[], int commandsLength){
		obj = new autocomplete(commands, commandsLength);
		defaultObj = autocomplete(*obj);
		line_len = 0;
		configVariables = vector<string>(sizeof(terminalConfig) / sizeof(int));
		recentlyDisplayedVerbose = false;
		setConfigurations();
	}

	/* Initialize the commandLine object with autocomplete object, so that all commands are autocompleted
	* @param obj: instance of autocomplete
	*/
	commandLine(const autocomplete &obj){
		this->obj = new autocomplete(obj);
		defaultObj = autocomplete(obj);
		line_len = 0;
		configVariables = vector<string>(sizeof(terminalConfig) / sizeof(int));
		recentlyDisplayedVerbose = false;
		setConfigurations();
	}
	// defulat constructor starts with default color settings and no commands are inserted
	commandLine() {
		this->obj = new autocomplete();
		defaultObj = autocomplete();
		line_len = 0;
		configVariables = vector<string>(sizeof(terminalConfig) / sizeof(int));
		recentlyDisplayedVerbose = false;
		setConfigurations();
	}

	// change the autocomplete object of the commandLine, in case you have a different set of commands you want it to load.
	void changeAutoComplete(const autocomplete& obj) {
		delete this->obj;
		this->obj = new autocomplete(obj);
	}

	// change the default autoComplete object to which the commandLine object should return.
	void changeDefaultAutoComplete(const autocomplete& obj) {
		defaultObj = autocomplete(obj);
	}


	void writeConfigurations(std::string);
	void setConfigurations(std::string filename);
	void setConfigurations() {
		configVariables[terminalConfig.lineStart] = ">>";
		configVariables[terminalConfig.outputLineStart] = "=>";
		configVariables[terminalConfig.displayTitle] = "";
		setColors();
	}

	void setColors(
		std::string inputText = TEXT_COLOR(COLOR_FG_White, "1"),
		std::string outputText = TEXT_COLOR(COLOR_FG_Blue, "1"),
		std::string error = TEXT_COLOR(COLOR_FG_Yellow, "1"),
		std::string fatalError = TEXT_COLOR(COLOR_FG_BrightRed, "1"),
		std::string verboseMessage = TEXT_COLOR(COLOR_FG_White, "2"),
		std::string infoText = TEXT_COLOR(COLOR_FG_BrightYellow, "1"),
		std::string validInput = TEXT_COLOR(COLOR_FG_BrightGreen, "1")) {
		if (!validateColor(inputText) || !validateColor(outputText) || !validateColor(error) || !validateColor(fatalError) || !validateColor(verboseMessage) || !validateColor(infoText) || !validateColor(validInput))
			throw("invalid color detected");
		configVariables[terminalConfig.inputTextColor] = inputText;
		configVariables[terminalConfig.outputTextColor] = outputText;
		configVariables[terminalConfig.errorTextColor] = error;
		configVariables[terminalConfig.fatalErrorTextColor] = fatalError;
		configVariables[terminalConfig.verboseTextColor] = verboseMessage;
		configVariables[terminalConfig.infoTextColor] = infoText;
		configVariables[terminalConfig.validInputColor] = validInput;

	}

	// set of characters that should be displayed at beginning of line for input
	void setLineStart(std::string s) {
		configVariables[terminalConfig.lineStart].clear();
		for (char& ch : s) {
			if (isPrintableCharacter(ch))
				configVariables[terminalConfig.lineStart].push_back(ch);
		}

	}

	// set display title from filename being given as parameter
	void setDisplayTitle(const char* filename) {
		ifstream fileread;
		fileread.open(filename);
		if (fileread.fail())
			throw("setDisplayTitle : given file doesn't exist");
		fileread.seekg(fileread.end);
		int cursPos = fileread.tellg();
		fileread.seekg(fileread.beg);
		configVariables[terminalConfig.displayTitle] = std::string(cursPos, '\0');
		fileread.read(&configVariables[terminalConfig.displayTitle][0], cursPos);
		fileread.close();
	}

	// set display title from string being given as parameter
	void setDisplayTitle(std::string disp) {
		if (disp.back() != '\n')
			disp.push_back('\n');
		configVariables[terminalConfig.displayTitle] = disp;
	}

	// modify the configurations of commandLine
	void setDisplayTitle(std::string disp, std::string displayColor) {
		if (disp.back() != '\n')
			disp.push_back('\n');
		configVariables[terminalConfig.displayTitle] = displayColor + disp + std::string(TEXT_COLOR(COLOR_FG_White, "0"));
	}

	// a simple getter function
	std::string getLineStart() {
		return configVariables[terminalConfig.lineStart];
	}

	// get the terminal ready for output, by resetting colors.
	void readyForOutput() {
		term.putString("\033[0m");
		term.putString(configVariables[terminalConfig.outputTextColor].c_str());
		term.newline();
		term.putString(configVariables[terminalConfig.outputLineStart].c_str());

		recentlyDisplayedVerbose = false;
	}

	// at beginning, when waiting for user input, printLineStart() is used.
	void printLineStart() {
		term.newline();
		term.putString(configVariables[terminalConfig.inputTextColor].c_str());
		term.putString(configVariables[terminalConfig.lineStart]);
	}
	
	void printDisplayTitle(std::string applicationName = "");

	void displayChar(char ch) {
		term.putChar(ch);
	}

	// display error message
	void displayError(std::string error) {
		readyForOutput();
		term.putString(configVariables[terminalConfig.errorTextColor].c_str());
		term.putString("** ERROR : ");
		term.putString(error);
	}
	
	// display very dangerous error (program crash)
	void displayFatalError(std::string fatalError) {
		readyForOutput();
		term.putString(configVariables[terminalConfig.fatalErrorTextColor].c_str());
		term.putString("**** FATAL ERROR : ");
		term.putString(fatalError);
	}

	// display a normal information message
	void displayMessage(std::string info) {
		readyForOutput();
		term.putString("[*]");
		term.putString(configVariables[terminalConfig.infoTextColor].c_str());
		term.putString(info);
	}

	// display program output or make terminal ready for program output
	void displayOutput(std::string output = "") {
		readyForOutput();
		term.putString(output);
	}

	void displayInput(std::string input) {
		printLineStart();
		term.putString(input);
	}

	// display random messages to tell the user that some processing is happening
	void displayVerbose(std::string msg) {
		term.putString(configVariables[terminalConfig.verboseTextColor].c_str());
		term.putString("[+]");
		if (recentlyDisplayedVerbose) {
			term.putString("\033[1M");
			term.newline('\r');
		}
		else
			term.newline();
		term.putString(msg);
		recentlyDisplayedVerbose = true;
	}

	// change autocomplete object on finding this in the previous valid command
	void addAutocompletePrevWordChange(std::string word, const char* commands[], int num_commands) { 
		validCommReplacement.insert(
			pair<std::string, autocomplete>(word, autocomplete(commands, num_commands))
		);
	}
	// change autocomplete object on finding this in the previous valid command
	void addAutocompletePrevWordChange(std::string word, autocomplete& obj) {
		validCommReplacement.insert(pair<std::string,autocomplete>(word,obj));
	}
	// change autocomplete object on finding this in the previous valid command
	void addAutocompletePrevWordChange(std::string word, string* commands, int num_commands) {
		validCommReplacement.insert(pair<std::string, autocomplete>(word, autocomplete(commands,num_commands)));
	}

	// main/most important function of commandLine, used to retrieve command from the user.
	std::string getCommand();
	~commandLine() {
		term.putString("\033[0m");
		delete obj;
	}
};

#endif // COMMAND_LINE_H