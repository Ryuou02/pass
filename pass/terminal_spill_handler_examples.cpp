#include "commandLine.h"

/*
* The below example is used to show how various display statements work and can be used.
*/
void example2() {
	const char* commands[] = {
		"start",
		"end",
		"quit"
	};
	const char* startComm[] = {
		"something",
		"nothing"
	};
	commandLine CLI1(commands, std::size(commands));
	CLI1.addAutocompletePrevWordChange("start", startComm, std::size(startComm));
	//CLI1.writeConfigurations("configurations1.tfc");

	CLI1.setConfigurations("configurations1.tfc");

	CLI1.setDisplayTitle("--------------------------------------------display title--------------------------------------------ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ", TEXT_COLOR(COLOR_FG_BrightGreen, "6"));
	CLI1.printDisplayTitle();
	CLI1.displayVerbose("[+] hello, welcome to program!");
	SLEEP(5000);
	CLI1.displayVerbose("[+] I like to eat apple!");
	SLEEP(5000);
	CLI1.displayVerbose("[+] I also like to eat banana");
	SLEEP(5000);
	CLI1.displayVerbose("[+] but I don't like bananana");
	SLEEP(5000);
	CLI1.displayMessage("what are you going to give me?");
	std::string item = CLI1.getCommand();
	CLI1.displayOutput("THANK YOU FOR " + item);
	SLEEP(5000);
	CLI1.displayError("wait, this is not a banana!");
	SLEEP(5000);
	CLI1.displayFatalError("THIS IS A BANANANA, I HATE THIS!!");
}


/*
* This is a simple usage axample for the autocomplete module.
*/
void example3() {
	const char* commands[] = {
		"start",
		"end",
		"quit"
	};
	const char* startComm[] = {
		"something",
		"nothing"
	};

	vector<string> endComm(3);
	endComm.push_back("all");
	endComm.push_back("this");
	endComm.push_back("nothing");

	commandLine CLI1(commands, std::size(commands));
	CLI1.addAutocompletePrevWordChange("start", startComm, std::size(startComm));
	CLI1.addAutocompletePrevWordChange("end", endComm.data(), endComm.size());

	std::string command;
	do {
		try {
			command = CLI1.getCommand();
		}
		catch (int error_code) {
			switch (error_code) {
			case EXIT_PROGRAM:
				command = "quit";
				break;
			default:
				break;
			}

		}
	} while (command != std::string("quit"));
}

int main() {
	example2();
	example3();
}