/****************************************
*
* pass library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/
#include "pass.h"

// just a function to return -1, which means that the program should terminate.
int exit(int k, const char* args[], commandLine* passCLI) {
	return -1;
}

/* Checks and makes sure that the required folder exists, and prepares terminal
*/
void pass::startup_interactive(commandLine& passCLI)
{
	vector<string> keys;
	for (auto it = commandFunction.begin(); it != commandFunction.end(); ++it) {
		keys.push_back(it->first);
	}
	autocomplete obj(keys.data(), keys.size());
	passCLI.changeDefaultAutoComplete(obj);
	passCLI.changeAutoComplete(obj);
	
	std::ofstream filewrite;
	filewrite.open(ROOT_PROGRAM_FOLDER "/metadata");
	if (filewrite.fail())
	{
		system("mkdir " ROOT_PROGRAM_FOLDER);
		filewrite.open(ROOT_PROGRAM_FOLDER "/metadata");
		if (filewrite.fail()) {
			passCLI.displayFatalError("not able to open program files!");
			passCLI.displayMessage("possible fix - open pass with administrative power and it can finally take over your system");
			exit(0);
		}
		else {
			filewrite.close();
		}
	}
	filewrite.close();
}

/*
* Relate a command name with the function required to execute the command.
*/
void pass::addCommand(std::string commandName, int(*commandF)(int, const char**, commandLine*))
{
	commandFunction[commandName] = commandF;
}

// default constructor checks whether the settings are done on pass_config.tfc, if file doesn't exist then the settings prescribed in ColorDefinitions.h is used to create the settings.
// note that if pass_config.tfc exists, pass will no longer look at ColorDefinitions.h.
pass::pass()
{
	try {
		passCLI.setConfigurations("pass_config.tfc");
	}
	catch (const char*) {
		passCLI.setColors(USER_COLOR, PROGRAM_COLOR, ERROR_COLOR, ERROR_FATAL_COLOR, PROGRAM_2_COLOR, INFO_COLOR);
		passCLI.setDisplayTitle("#####    " PROGRAMASSIST_TITLE + to_string(VERSION / (float)100) + "   ######", PROGRAM_COLOR);
		passCLI.writeConfigurations("pass_config.tfc");
	}
	addCommand("exit", exit);
}

// execute the command called. In case nothing is passed/there are no commands, interactive mode is chosen.
int pass::pass_exec(int k, const char* args[])
{
	if (k >= 1) {
		if (commandFunction.find(std::string(args[0])) != commandFunction.end()) {
			int ret = commandFunction[std::string(args[0])](k, args, &passCLI);
			return ret;
		}
		return -2;
	}

	startup_interactive(passCLI);
	passCLI.printDisplayTitle();

	std::string command;
	vector<string>cm;
	const char** vargs;
	while (1) {
		try {
			command = passCLI.getCommand();
		}
		catch (int errorCode) {
			switch (errorCode) {
			case EXIT_PROGRAM:
				return 0;
			}
		}
		cm = splitString(command);

		vargs = new const char* [cm.size()];
		for (int i = 0; i < cm.size(); i++) {
			vargs[i] = cm[i].c_str();
		}
		try {
			int exit = pass_exec(cm.size(), vargs);
			if (exit == -1)
				return 0;
			else if (exit == -2)
				throw(message{ ERROR_MSG, std::string("command does not exist") });
		}
		catch (message e) {
			switch (e.type) {
			case ERROR_MSG:
				passCLI.displayError(std::string(e.message));
				break;
			case FATAL_ERROR_MSG:
				passCLI.displayFatalError(std::string(e.message));
				delete[] vargs;
				return 0;
				break;
			}
		}
		delete[] vargs;
	}
	return 0;
}
