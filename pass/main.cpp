#include "pass.h"

int calc(int k, const char** args, commandLine* obj);

int main(int k, const char** args) {
	pass passObj;
	passObj.addCommand("calc", calc);
	std::string calcCommands[] = { "sin","cos","tan" };
	passObj.passCLI.addAutocompletePrevWordChange("calc", calcCommands, std::size(calcCommands));

	passObj.pass_exec(k - 1, args + 1);
}