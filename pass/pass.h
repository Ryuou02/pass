/****************************************
*
* pass library
* Developed by Ryuou02
* https://github.com/Ryuou02
*
*****************************************/
#pragma once
#include "commandLine.h"
#include"ColorDefinitions.h"
#include"files_and_folders.h"

// in case an error needs to be displayed, this struct is used to assist with throwing it.
typedef struct {
	short int type;
	std::string message;
} message;



class pass {
	void startup_interactive(commandLine& passCLI);
	map<std::string, int(*)(int, const char**, commandLine*)> commandFunction;

public:
	/*
	* Directly access the commandLine object, for extra functionality.
	*/
	commandLine passCLI;
	void addCommand(std::string commandName, int (*commandF)(int, const char**, commandLine*));
	pass();

	int pass_exec(int k, const char* args[]);
};

