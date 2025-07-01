#include<vector>
#include<string>
#include<stdexcept>
#include<unordered_map>
#include"commandLine.h"

using namespace std;

// below definition is used in eval()
#define CLEAR_UP numbers.erase(numbers.begin() + i + 1);\
op.erase(op.begin() + i);\
evaluated = true;

// if the program is in debug mode then debug messages will be displayed
#ifdef _DEBUG
std::string verboseBuffer;
#define LOG(A) verboseBuffer += A ;
#define DISPLAY_LOG obj->displayMessage(verboseBuffer);
#else
#define LOG(A)
#define DISPLAY_LOG
#endif


std::string compile(std::string expression, unordered_map<std::string, double> variableMap);
static double sinCalc(vector<double> args);
static double cosCalc(vector<double> args);
static double tanCalc(vector<double> args);
int calc(int k, const char** args, commandLine* obj);
void ExecCalcCommand(std::string command, ofstream& filewrite, commandLine& obj2);


// GLOBALS
unordered_map<std::string, double> mainVariableMap;	// contains variables along with their value
unordered_map<std::string, double(*)(vector<double>args)> predefinedFunctions = { {"sin",sinCalc},{"cos",cosCalc},{"tan", tanCalc} };
unordered_map<std::string, std::string> userDefinedFunctions;	// function name and functionExpression


/* given a simple expression consisting only of numbers and brackets, the numeric value of result of computations within the expression is returned as output.
* @param a mathematical expression containing brackets, numbers and operators only (string)
* @return result of mathematical computation (double)
*/
double eval(std::string exp)
{
	int count = 0;
	double res;
	std::vector<double> numbers;
	std::vector<char> op;
	int dec = 0;


	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] == '[' || exp[i] == '{')
			exp[i] = '(';
		else if (exp[i] == ']' || exp[i] == '}')
			exp[i] = ')';
	}

	for (int i = 0; i < exp.size(); i++)
	{	
		if (exp[i] == '.')
			dec++;
		LOG(exp + '\n');
		if (exp[i] == '('){
			int k = i;
			int br = 1;
			while (br > 0)
			{
				i++;
				if (exp[i] == ')')
					br--;
				else if (exp[i] == '(')
					br++;
				if (i == exp.size())
				{
					throw std::runtime_error("brackets not terminated");
					return 0;
				}
			}
			
			res = eval(exp.substr(k + 1, i - k - 1));
			
			LOG("\nresult obtained " + to_string(res) + "\tk = " + to_string(k) + " \t i = " + to_string(i));
			
			exp.erase(k, i - k + 1);
			exp.insert(k, std::to_string(res));
			i = 0;
		}
	}

	for (int i = 0; i < exp.size(); i++)
	{
		if (exp[i] == '|') {
			int k = i;
			int br = 1;
			while (br > 0)
			{
				i++;
				if (exp[i] == '|')
					br--;
				if (i == exp.size())
				{
					throw std::runtime_error("brackets not terminated");
					return 0;
				}
			}
			res = abs(eval(exp.substr(k + 1, i - k - 1)));

			LOG("\nresult obtained " + to_string(res) + "\tk = " + to_string(k) + " \t i = " + to_string(i));

			exp.erase(k, i - k + 1);
			exp.insert(k, std::to_string(res));
			i = 0;
		}
	}

	LOG("\nevaluating expression => " + exp);

	char* nend;
	char* expc = new char[exp.size()];

	for (int i = 0; i < exp.size(); i++)
		expc[i] = exp[i];

	char* pEnd = expc;
	do {
		numbers.push_back(strtod(pEnd, &nend));
		if (nend - expc != exp.size()) {
			op.push_back(nend[0]);
			pEnd = nend + 1;
		}
	} while (nend - expc < exp.size());

	if (numbers.size() != op.size() + 1)
		throw std::exception("the expression doesn't seem to be right");
	bool evaluated = false;
	do {
		/*
		* Don't place all into a single for loop since the separate for loops are used to ensure precedence of operators
		*/
		evaluated = false;	// variable to indicate whether a computation has taken place
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '^')
			{
				numbers[i] = pow(numbers[i], numbers[i + 1]);
				CLEAR_UP;
			}
		}
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '/')
			{
				if (numbers[i + 1] == 0) {
					throw std::runtime_error("division by 0 is not allowed!");
				}
				numbers[i] = numbers[i] / numbers[i + 1];
				CLEAR_UP	
			}
		}
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '%')
			{
				if (numbers[i + 1] == 0) {
					throw std::runtime_error("modulus by 0 is not allowed!");
				}

				numbers[i] = int(numbers[i]) % int(numbers[i + 1]);
				CLEAR_UP
			}
		}
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '*')
			{
				numbers[i] = numbers[i] * numbers[i + 1];
				CLEAR_UP
			}
		}
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '-')
			{
				numbers[i] = numbers[i] - numbers[i + 1];
				CLEAR_UP
			}
		}
		for (int i = 0; i < op.size(); i++)
		{
			if (op[i] == '+')
			{
				numbers[i] = numbers[i] + numbers[i + 1];
				CLEAR_UP
			}
		}
		for (int i = 0; i < op.size(); i++) {
			if (op[i] == '.')
				throw std::exception("double decimal points not allowed pls");
		}
		if (!evaluated && op.size() > 0)
			throw std::exception("it seems that an operator in the expression doesn't exist");
	} while (op.size() > 0);
	return numbers.back();
}


/*
* check whether the variable name is valid or not
* @param variable name (string)
* @return true -> all characters in string are alphabets
* @return false -> there is a non-alphabet character in the string
*/
static bool isVarName(std::string varName) noexcept{
	for (char& a : varName) {
		if (isalpha(a) == 0)
			return false;
	}
	return true;
}

/* check whether the string passed is a valid function definition
* @param a complete mathematical function definition, with variables in brackets
* @return true -> there is a function name, followed by '(' with as many commas to separate the variables
*/
static bool isFunctionName(std::string functionName) {
	int brackets = 0;
	bool isFunction = false;
	for (char& a : functionName) {
		if (isalpha(a) == 0) {
			if (a == '(')
				brackets += 1;
			else if( a!= ',' || a != ')')
				return false;
		}
	}
	if (brackets > 1)
		throw (functionName + " is invalid function name");
	return true;
}

/* get the left hand side(L.H.S.) of the equation
* @param any expression or equation (string)
* @return whatever exists towards the left side of
*/
std::string getEqLHS(std::string equation) {
	std::string varName = "";
	for (char& a : equation) {
		if (a != '=') {
			varName.push_back(a);
		}
		else
			break;
	}
	if (varName.size() == equation.size())
		varName = "";
	return varName;
}


/* get the alphabet string out of a given string.
* @param function call/function/variable
* @return alphabet string out of a given string.
* In case of empty string, the name is returned as "that"
*/
std::string getVarName(std::string name) {
	std::string varName = "";
	for (char& a : name) {
		if (isalpha(a))
			varName.push_back(a);
		else break;
	}
	if (varName == "")
		varName = "that";
	return varName;
}

/*
* This function returns the expression out of the equation
* "x=5*5" -> "5*5"
* @param input equation/expression
* @return whatever exists on the right side of '=', if there exists anything
*/
std::string getExpression(std::string equation) {
	std::string expression = "";
	for (char& a : equation) {
		expression.push_back(a);
		if (a == '=')
			expression.clear();
	}
	return expression;
}

/*	convert string of comma separated expressions into a vector of double
*	@param list of comma separated expressions (string)
*	@return a vector of numbers, which are the evaluated results of those expressions (vector<double>) 
*/
vector<double> numberListStringToVector(std::string list) {
	std::string expression;
	vector<double> numbers;
	for (char& a : list) {
		if (a == ',')
			numbers.push_back(eval(expression));
		else
			expression.push_back(a);
	}
	numbers.push_back(eval(expression));
	return numbers;
}

// predfined functions for sake of function calls.
double sinCalc(std::vector<double> args) {
	return sin(args[0]);
}
double cosCalc(std::vector<double> args) {
	return cos(args[0]);
}
double tanCalc(std::vector<double> args) {
	return tan(args[0]);
}

/* functionName(x, y, z), {10,20,30} -> {x:10, y : 20, z : 30}
* @param functionName(x, y, z), {10,20,30}		[function definition and list of numbers to replace each variable in the defintion]
* @return {x:10, y : 20, z : 30}				[a map of each variable to it's corresponding numeric value]
*/
unordered_map<std::string, double> createVariableList(std::string functionName, vector<double> args) {
	unordered_map<std::string, double> variableList;
	/*
	* functionName(x,y,z)
	* ^
	*/
	int i = 0;
	while (i < functionName.size() && functionName[i] != '(') {
		i++;
	}
	i++;
	/*
	* functionName(x,y,z)
	*             ^
	*/

	if (i == functionName.size()) {
		throw functionName + " doesn't have variables specified in it!";
	}
	std::string var;
	int k = 0;
	while (k < args.size()) {
		var = "";
		while (functionName[i] != ',' && functionName[i] != ')') {
			var.push_back(functionName[i++]);
		}
		variableList[var] = args[k];
		k++;
	}
	return variableList;
}

/* calculate the numeric value of a function being used.
* @param name of the function being called/used (string)
* @param arguments used within the function, i.e. if f(5,6) is the function, 5,6 is the argument
* @return value of the evaluated function.
*/
double calcFunction(std::string functionName, std::string args) {
	// conversion functions are left to be done in future.
	if(functionName == "currencyConv"){
	}
	else if (functionName == "lengthConv") {
	}
	else if (functionName == "weightConv") {
	}
	else if (functionName == "numberConv") {
		// input is by default converted to decimal
		// requires 3 arguments - existing number system, number system to convert into, input
	}

	// check if the function is a predefined function.
	else if (predefinedFunctions.find(functionName) != predefinedFunctions.end()) {
		return predefinedFunctions[functionName](numberListStringToVector(args));
	}
	else {
		// is a user defined function
		if (userDefinedFunctions.find(functionName) == userDefinedFunctions.end())
			throw functionName + " function doesn't exist";

		return eval(compile(getExpression(userDefinedFunctions[functionName]), createVariableList(getEqLHS(userDefinedFunctions[functionName]), numberListStringToVector(args))));
	}
	return 0;
}

/*
* This function serves the following purposes - 
* 1. converts all variables in the expression (equation is not allowed) into its respective values.
*	given {x:5,y:10}
*	"5x+2y" -> "5*5+2*10"
* 
* 2. places multiplication signs in appropriate locations, i.e. suppose 5(5) is written, it's rewritten as 5*(5)
*		5(5) -> 5*(5)
*		(5)5 -> (5)*5
*		5x -> 5*x
*		x5 -> x*5
* 3. suppose x(5) is written, x is considered as a function and 5 will be its argument.
* @param mathematical expression (string)
* @param variableMap
* @return an expression with only numbers
*/
std::string compile(std::string expression, unordered_map<std::string,double> variableMap) {
	std::string newExpression = "";
	std::string var = "";

	for (int i = 0; i < expression.size(); i++) {
		if (isalpha(expression[i])) {

			// 5x -> 5*x
			if (i > 0 && isdigit(expression[i - 1])) {
				newExpression.push_back('*');
			}

			var.push_back(expression[i]);
			
			if (i + 1 == expression.size() || isalpha(expression[i + 1]) == 0) {
				try {
					if (i + 1 < expression.size() && expression[i + 1] == '(')	// is a function, not variable
					{
						int brackets = 1;
						i ++;	// place i immediately after the '(' bracket
						std::string args = "";
						std::string arg = "";
						while (expression[i] != ')' || brackets > 1) {
							i++;
							arg = "";
							do {
								arg.push_back(expression[i]);
								if (expression[i] == '(')
									brackets++;
								if (expression[i] == ')')
									brackets--;
								i++;
							} while (expression[i] != ',' && (expression[i] != ')' || brackets > 1));
							args.append(compile(arg, variableMap));
						}
						newExpression.append(to_string(calcFunction(var, args)));
					}
					else {
						if (variableMap.find(var) != variableMap.end())
							newExpression.append(to_string(variableMap[var]));
						else if (mainVariableMap.find(var) != variableMap.end())
							newExpression.append(to_string(mainVariableMap[var]));
						else
							throw("variable is not defined!");
					}
				}
				catch (std::exception e) {
					throw " brackets are not terminated! ";
				}
				var = "";
			}
		}
		else {
			// 5(5) -> 5*(5)
			if (expression[i] == '(' && i > 0 && isdigit(expression[i - 1]))
				newExpression.push_back('*');

			// (5)5 -> (5)*5
			if (expression[i] == ')' && i != expression.size() && isalnum(expression[i + 1]))
				newExpression.push_back('*');

			newExpression.push_back(expression[i]);
		}
	}
	return newExpression;
}

/*check whether a string input resembles function definition or not
* @param left side of equation (string)
* @return whether there is a '(' in it.
*/
static bool isFunction(std::string equationLHS) {
	for (char& a : equationLHS) {
		if (a == '(')
			return true;
	}
	return false;
}

// if an input begins with '//' then it's a comment
inline bool isComment(std::string a) {
	return (a[0] == '/' && a[1] == '/');
}

/*copy contents of filename1 to filename2
* @param name of file1
* @param name of file2
*/
void copyFile(const char* filename1, const char* filename2) {
	ifstream fileread(filename1);
	if (fileread.fail())
		throw("file " + std::string(filename1) + " is not found");
	ofstream filewrite(filename2);
	while (!fileread.eof()) {
		char a = fileread.get();
		if(!fileread.eof())
			filewrite.put(a);
	}
	filewrite.close();
	fileread.close();
}

/* load the contents of a file onto calc
* @param filename: name of file to load
* @param obj: instance of commandLine object on which the contents are printed
* @param filewrite: instance of ofstream which is writing to a file.
*/
static void loadFile(std::string filename, commandLine& obj, ofstream& filewrite) {
	ifstream fileread;
	fileread.open(filename);
	if (fileread.fail())
		throw("File " + filename + " doesn't exist;");
	std::string command;	
	while (!fileread.eof()) {
		getline(fileread, command);
		obj.displayInput(command);
		ExecCalcCommand(command, filewrite, obj);
	}
	fileread.close();
}

// copy contens of string b to a
void stringCopy(char* a, std::string b) {
	for (int i = 0; i < b.size(); i++)a[i] = b[i];
}

/* Execute the command, which may be a function definition, or equation 
*/
void ExecCalcCommand(std::string command, ofstream &filewrite, commandLine &obj2) {
	const char* commands[2];
	commands[0] = "calc";
	const char *comm = command.c_str();
	filewrite << command;
	if (!isComment(command)) {
		commands[1] = comm;
		calc(2, commands, &obj2);
	}
}

/* calc function is used as an interface to the pass object, it is also used to run calc in interactive mode if necessary.
*/
int calc(int k, const char** args, commandLine* obj) {
	ofstream filewrite;
#ifdef _DEBUG
	verboseBuffer = "";
#endif

	if (k == 1) {
		commandLine obj2;
		obj2.printDisplayTitle(std::string("\nCALC [press 'esc' key to exit, ctrl+s to save and ctrl+l to load]"));
		obj2.setLineStart(std::string("calc>"));
		
		filewrite.open("calc.temp");
		while (true) {
			try {
				ExecCalcCommand(obj2.getCommand(), filewrite, obj2);
			}
			catch (int a) {
				switch (a) {
				case EXIT_PROGRAM:
					filewrite.close();
					return 0;

				case CTRL_S:
					obj2.displayMessage("Enter file name to save to");
					filewrite.close();
					copyFile("calc.temp", obj2.getCommand().c_str());
					filewrite.open("calc.temp",ios::app);
					break;
				case CTRL_L:
					obj2.displayMessage("Enter file name to load");
					loadFile(obj2.getCommand(), obj2,filewrite);
					break;
				}
			}
		}
		return 0;
	}
	std::string useful_string = "";
	for (int i = 1; i < k; i++)
	{
		obj->displayMessage(args[i]);
	}

	obj->displayMessage(to_string(k));

	for (int i = 1; i < k; i++)
	{
		obj->displayMessage(args[i]);
		for (int j = 0; j < strlen(args[i]); j++)
		{
			if (args[i][j] != 32) {
				useful_string += args[i][j];
			}
		}
	}

	try {

		std::string LHS = getEqLHS(useful_string);
		if (isFunction(LHS)) {
			userDefinedFunctions[getVarName(LHS)] = useful_string;
			LOG( "\n" + LHS + " is function is recognized")
			DISPLAY_LOG
			return 1;
		}
		else {
			mainVariableMap[getVarName(LHS)] = eval(compile(getExpression(useful_string), mainVariableMap));
			obj->displayMessage("received:" + useful_string);
			obj->displayOutput(to_string(mainVariableMap[getVarName(LHS)]));
			DISPLAY_LOG
			return 1;
		}
	}
	catch (std::runtime_error e)
	{
		obj->displayError(e.what());
		DISPLAY_LOG
		return 0;
	}
	catch (std::exception& e) {
		obj->displayError(e.what());
		DISPLAY_LOG
		return 0;
	}
	catch (std::string e) {
		DISPLAY_LOG
		obj->displayError(e);
		return 0;
	}
}