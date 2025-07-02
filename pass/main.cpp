#include "commandLine.h"

int calc(int k, const char** args, commandLine* obj);

int main(int k, const char** args) {
	commandLine obj;
	calc(k,args,&obj);
}
