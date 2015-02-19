//Tony Pham
//CPSC 323
//Assignment #3
//Parser
#include <iostream>
#include <string>
#include <fstream>
#include <cctype>

using namespace std;

//syntax
int rat14su(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int optDeclarationList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int declarationList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int declatationFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int declaration(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int qualifier(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int emptyFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int statementList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int statementFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int statement(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int assign(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int expression(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int expressionPrime(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int term(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int factor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int termPrime(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int primary(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int primaryFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int ids(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int idsFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int writeFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int compound(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int ifFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int ifFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int returnFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int readFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int whileFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int condition(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
int relop(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs);
void getToken(int& counter, string& currentToken, struct data mydata, int datacounter, ofstream &ofs);
void symtable(struct data mydata, int &counter, struct symb &symbtable, int &counter2, int &scope, int &memorynum, int datacounter, ofstream &ofs);
int get_addr(string token, struct symb symbtable, ofstream &ofs);
void gen_instr(string op, int oprnd, struct instr &instr_table, int &instr_num);
int push_jumpstack(int arr[], int &stackCounter, int addr);
int back_patch(int jump_addr, struct instr &instr_table, int stackCounter, int arry[]);

int stackCounter = 1;
int arr[100];

struct data{
	int		line[100];
	string	token[100];
	string	lexeme[100];

} mydata;

struct symb{
	string	identifier[100];
	int		memory[100];
	string	type[100];
	int		scope[100];
} symbtable;

struct instr{
	int address[100];
	string op[100];
	int oprnd[100];
} instr_table;

void print(char store[], int &counter, int &currentState, int &result, ofstream &ofs, data &mydata, int &datacounter, int &line);
int DFSM(char c, int table[][4], int &state);
int check(char store[], int counter, const char key[][9]);


//Globabl variables
int lines = 1;
int instr_num = 1;
int addr;


//strings for assembly output
string POPM = "POPM";
string ADD = "ADD";
string MUL = "MUL";
string SUB = "SUB";
string PUSHI = "PUSHI";
string PUSHM = "PUSHM";
string PUSHS = "PUSHS";
string DIV = "DIV";
string LABEL = "LABEL";
string GRT = "GRT";
string LES = "LES";
string EQU = "EQU";
string NEQ = "NEQ";
string JUMPZ = "JUMPZ";
string JUMP = "JUMP";

int main()
{
	//array of keywords
	const char keys[12][9] = { "function", "int", "boolean", "if", "fi", "else", "return", "write", "read",
		"while", "true", "false" };
	//array of FSM table
	int table[7][4] = {
			{ 0, 0, 0, 0 },
			{ 0, 3, 2, 6 },
			{ 0, 6, 2, 6 },
			{ 0, 4, 6, 5 },
			{ 0, 4, 6, 5 },
			{ 0, 4, 6, 5 },
			{ 0, 6, 6, 6 } };

	int currentState = 1;
	int counter = 0;
	int result = 0;
	char c;
	char store[20];
	string filename;

	int datacounter = 0;
	//open file streams
	ofstream ofs;
	ifstream is;
	//get file names to open
	cout << "Please enter a file name to test: ";
	getline(cin, filename);
	is.open(filename.c_str());
	//opening input file failure
	if (is.fail())
	{
		cout << "Error opening file, now closing" << endl;
		return 1;
	}

	cout << "Please enter a file name to output to: ";
	getline(cin, filename);
	ofs.open(filename.c_str());

	//start format for output file
	ofs << "TOKEN			LEXEME" << endl << endl;

	while (!is.eof())
	{
		result = 0;
		//get character
		c = is.get();
		if (c == '\n')
		{
			lines++;
		}
		if ((c == ' ' && counter > 0) | (c == '\n' && counter > 0) | (c == '\t' && counter > 0))
		{
			//prints out stored array when space, new line, or tab are reached and counter of array is > 0
			result = check(store, counter, keys);
			print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
		}
		else
		{
			//checks for operators
			if (c == '|' || c == '>' || c == '<' || c == '>' || c == '*' || c == '/' || c == '+'
				|| c == '-' || c == '=' || c == '!')
			{
				if (counter > 0)
				{
					if (c == '=')
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = "==";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					if (c == '!')
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = "!=";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					else
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "operator                " << c << " " << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}

				}
				else
				{
					//check for operator that can be more than 1 character == | !=
					if (c == '=')
					{
						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = "==";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					if (c == '!')
					{
						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = "!=";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					else
					{
						ofs << "operator                " << c << " " << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}

				}
			}
			else if (c == ':')
			{
				if (counter > 0)
				{
					//check forward character to see if its an operator, print array 
					//then get multichar operators
					if (is.peek() == ':')
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "operator                " << c;
						c = is.get();
						ofs << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = "::=";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					if (is.peek() == '=')
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = ":=";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					else
						//forward character not needed for oeprator so its a separator, so print array
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "separator               " << c << endl;

						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "separator";
						++datacounter;
					}

				}
				else
				{
					//check forward character to see if its an operator
					if (is.peek() == ':')
					{

						ofs << "operator                " << c;
						c = is.get();
						ofs << c;
						c = is.get();
						ofs << c << endl;

						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = ":==";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					if (is.peek() == '=')
					{

						ofs << "operator                " << c;
						c = is.get();
						ofs << c << endl;

						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = ":=";
						mydata.lexeme[datacounter] = "operator";
						++datacounter;
					}
					else
					{
						ofs << "separator               " << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "separator";
						++datacounter;
					}
				}
			}
			else if (c == '(' || c == ')' || c == ':' || c == ';' || c == '{' || c == '}'
				|| c == '$' || c == ',')
			{
				if (counter > 0)
				{
					//check and print stored array then print multi character separator
					if (c == '$')
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						if (is.peek() == '$')
						{
							ofs << "separator               " << c;
							c = is.get();
							ofs << c << endl;

							mydata.line[datacounter] = lines;
							mydata.token[datacounter] = "$$";
							mydata.lexeme[datacounter] = "separator";
							++datacounter;
						}
						else
						{
							ofs << "illegal                 " << c;
							mydata.line[datacounter] = lines;
							mydata.token[datacounter] = c;
							mydata.lexeme[datacounter] = "illegal";
							++datacounter;
						}

					}
					//check and print array then print the single char separator
					else
					{
						result = check(store, counter, keys);
						print(store, counter, currentState, result, ofs, mydata, datacounter, lines);
						ofs << "separator               " << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "separator";
						++datacounter;
					}
				}
				else
				{
					//$ is the character and there are no inputs in the array, so print
					if (c == '$')
					{
						if (is.peek() == '$')
						{
							ofs << "separator               " << c;
							c = is.get();
							ofs << c << endl;
							mydata.line[datacounter] = lines;
							mydata.token[datacounter] = "$$";
							mydata.lexeme[datacounter] = "separator";
							++datacounter;
						}
						else
						{
							ofs << "illegal                 " << c << endl;
						}
					}
					else
					{
						ofs << "separator               " << c << endl;
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "separator";
						++datacounter;
					}
				}

			}
			else
			{
				//since the character is not any separator or operator character, we input character
				//into DFSM to check if its accepted or rejected accepted state
				if (DFSM(c, table, currentState) == 1)
				{
					store[counter] = c;
					counter++;
				}
				//rejected state
				else if (DFSM(c, table, currentState) == 0 && c != ' ' && c != '\t' && c != '\n'
					&& !is.eof())
				{
					{
						ofs << "illegal                 ";
						ofs << c << endl;
						store[counter] = '\0';
						mydata.line[datacounter] = lines;
						mydata.token[datacounter] = c;
						mydata.lexeme[datacounter] = "illegal";
						++datacounter;
					}
				}


			}
		}
	}

	ofs << endl;

	ofs << endl;
	//testing purposes

	counter = 0;
	//start of SA
	int testcounter = 0;
	int counter2 = 0;
	int scope = 0;
	int memorynum = 1000;
	ofs << endl;
	symtable(mydata, testcounter, symbtable, counter2, scope, memorynum, datacounter, ofs);


	rat14su(mydata.token[0], counter, mydata, datacounter, ofs);
	ofs << endl;
	ofs << endl;
	//for (int i = 0; i < datacounter; ++i)
	//{
	//	ofs << "Data " << i << endl;
	//	ofs << mydata.lexeme[i] << " " << mydata.token[i] << " " << mydata.line[i] << endl;
	//}



	for (int i = 0; i < counter2; ++i)
	{
		ofs << "Identifier: " << symbtable.identifier[i] << " Type: " << symbtable.type[i] << " Memory: " << symbtable.memory[i] << endl;
	}

	ofs << endl;

	ofs << "Assembly results: " << endl;
	for (int i = 1; i < instr_num; ++i)
	{
		ofs << instr_table.address[i] << " " << instr_table.op[i];
		if (instr_table.oprnd[i] != -999)
		{
			ofs << " " << instr_table.oprnd[i] << endl;
		}
		else
		{
			ofs << endl;
		}
	}

	//after SA is done, close opened file streams
	is.close();
	ofs.close();
	return 0;
}


int DFSM(char c, int table[][4], int &state)
{
	//if character is _ input into table to get state
	if (c == '_')
	{
		state = table[state][3];
	}
	//if character is a letter input into table to get state
	else if (isalpha(c))
	{
		state = table[state][1];
	}
	//if character is a number input into table to get a state
	else if (isdigit(c))
	{
		state = table[state][2];
	}
	//accepted states return 1
	if ((state == 2) | (state == 3) | (state == 4) | (state == 5))
	{
		return 1;
	}
	//rejected states return 0
	else
	{
		return 0;
	}
}

//print function
void print(char store[], int &counter, int &currentState, int &result, ofstream &ofs, data &mydata, int &datacounter, int &lines)
{
	//array is equal to keyword so print keyword
	if (result == 1 && counter > 1 && !isdigit(store[0]))
	{
		ofs << "keyword                 ";
		for (int i = 0; i < counter; i++)
		{
			ofs << store[i];
		}
		ofs << endl;
		store[counter] = '\0';
		mydata.line[datacounter] = lines;
		mydata.token[datacounter] = store;
		mydata.lexeme[datacounter] = "keyword";
		++datacounter;
	}
	//array is not a keyword so print identifier
	else if (result == 0 && !isdigit(store[0]))
	{
		ofs << "identifier              ";
		for (int i = 0; i < counter; i++)
		{
			ofs << store[i];
		}
		store[counter] = '\0';
		mydata.line[datacounter] = lines;
		mydata.token[datacounter] = store;
		mydata.lexeme[datacounter] = "identifier";
		++datacounter;
		ofs << endl;
	}
	//array holds numerical values so print integer
	else if (isdigit(store[0]))
	{
		int reject = 0;

		for (int i = 0; i < counter; i++)
		{
			if (store[i] == '.')
			{
				reject = 1;
			}
		}
		if (reject == 1)
		{
			ofs << "illegal                 ";
			for (int i = 0; i < counter; i++)
			{
				ofs << store[i];
			}
			store[counter] = '\0';
			mydata.line[datacounter] = lines;
			mydata.token[datacounter] = store;
			mydata.lexeme[datacounter] = "illegal";
			ofs << endl;
			++datacounter;
		}
		else
		{
			ofs << "integer                 ";
			for (int i = 0; i < counter; i++)
			{
				ofs << store[i];
			}
			ofs << endl;
			store[counter] = '\0';
			mydata.line[datacounter] = lines;
			mydata.token[datacounter] = store;
			mydata.lexeme[datacounter] = "integer";
			++datacounter;
		}

	}
	//reset DFSM and array for new input
	counter = 0;
	currentState = 1;
	result = 0;
}

int check(char store[], int counter, const char key[][9])
{
	char str[20];
	//store character array with terminating character so that we can check against keywords
	for (int i = 0; i < counter; ++i)
	{
		str[i] = store[i];
		str[i + 1] = '\0';
	}

	for (int i = 0; i < 12; ++i) // check against key array
	{
		//if keyword is found, return 1
		if (strcmp(str, key[i]) == 0)
		{
			return 1;
		}
	}
	//else keyword is not found so 0 is returned
	return 0;
}

//Start of Syntax Functions


int rat14su(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "$$")
	{
		ofs << "Token: " << currentToken << endl;
		ofs << "<Rat14SU> ::= $$ <Opt Function Definitions>   $$  <Opt Declaration List> <Statement List>  $$" << endl;
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (currentToken == "$$")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			if (optDeclarationList(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				if (statementList(currentToken, counter, mydata, datacounter, ofs) == 1)
				{
					if (currentToken == "$$")
					{
						//getToken(counter, currentToken, mydata, datacounter, ofs);
						return 1;
					}
				}

			}
		}

	}
	else
	{
		return 2;
	}
}


int optDeclarationList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (declarationList(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (emptyFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else
		return 2;
}

int declarationList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (declaration(currentToken, counter, mydata, datacounter, ofs) == 1) //problem
	{
		if (currentToken == ";")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			if (declatationFactor(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				ofs << "<Declaration Factor> -> <Declaration List>\n";
				return 1;
			}
			else if (declatationFactor(currentToken, counter, mydata, datacounter, ofs) == 3)
			{
				ofs << "<Declaration Factor> -> E\n";
				return 1;
			}
		}

	}
	else
		return 2;
}


int declatationFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (declarationList(currentToken, counter, mydata, datacounter, ofs) == 1)  //problem 1
	{
		return 1;
	}
	else
		return 3;
}

int declaration(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (qualifier(currentToken, counter, mydata, datacounter, ofs) == 1)
	{

		if (ids(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
	}
	else
		return 2;

}


int qualifier(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "int")
	{
		ofs << "<Declaration> -> <Qualifier> <IDs>\n";
		ofs << "<Qualifier> -> int\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else if (currentToken == "boolean")
	{
		ofs << "<Declaration> -> <Qualifier> <IDs>\n";
		ofs << "<Qualifier> -> boolean\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;

	}
	else
	{
		return 2;
	}

}


int ids(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs) //problem fixed
{
	if (mydata.lexeme[counter] == "identifier")
	{
		addr = get_addr(mydata.token[counter], symbtable, ofs);//ASSIGNMENT3 READ Unsure if this is correct
		gen_instr(PUSHM, addr, instr_table, instr_num);//ASSIGNMENT3 READ Unsure if this is correct
		getToken(counter, currentToken, mydata, datacounter, ofs);

		if (idsFactor(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
		else if (idsFactor(currentToken, counter, mydata, datacounter, ofs) == 3)
		{
			ofs << "<IDs Factor> ::= E\n";
			return 1;
		}


	}
	else
		return 2;
}


int idsFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == ",")
	{

		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (ids(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}

	}
	else
	{
		return 3;
	}

}

int statementList(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (statement(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		if (statementFactor(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
		else if (statementFactor(currentToken, counter, mydata, datacounter, ofs) == 3)
		{
			ofs << "<Statement Factor> ::= E\n";
			return 1;
		}
	}
	else
	{
		return 2;
	}
}

int statementFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (statementList(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else
	{
		return 3; // empty
	}

}

int statement(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs) //need to add more fuctions
{
	if (compound(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (assign(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (ifFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (returnFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (writeFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (readFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else if (whileFunc(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else
	{
		return 2;
	}

}

int compound(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "{")
	{
		ofs << "<Statement> -> <Compound>\n";
		ofs << "<Compound> -> { <Statement List> }\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		addr = get_addr(mydata.token[counter], symbtable, ofs);//Assignment3 ASSIGN
		gen_instr(PUSHM, addr, instr_table, instr_num);//Assignment3 ASSIGN
		if (statementList(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			if (currentToken == "}")
			{
				getToken(counter, currentToken, mydata, datacounter, ofs);
				{
					return 1;
				}
			}
		}
	}
	else
	{
		return 2;
	}

}

int ifFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{

	if (currentToken == "if")
	{
		addr = instr_num;//ASSIGNMENT3 IF
		ofs << "<Statement> -> <if>\n";
		ofs << "<if> -> if (<Condition> ) <Statement> fi <IF Factor>\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (currentToken == "(")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			if (condition(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				if (currentToken == ")")
				{
					getToken(counter, currentToken, mydata, datacounter, ofs);
					if (statement(currentToken, counter, mydata, datacounter, ofs) == 1)
					{
						back_patch(instr_num, instr_table, stackCounter, arr);//ASSIGNMENT3 IF

						if (ifFactor(currentToken, counter, mydata, datacounter, ofs) == 1)
						{
							ofs << "if Factor> ::= else <Statement> fi\n";
							return 1;
						}

					}
				}
			}
		}
	}
	else
	{
		return 2;
	}

}

int ifFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "else")
	{
		addr = instr_num;
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (statement(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			back_patch(instr_num, instr_table, stackCounter, arr);
			if (currentToken == "fi")
			{
				getToken(counter, currentToken, mydata, datacounter, ofs);
				return 1;

			}
		}

	}
	else if (currentToken == "fi")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else
		return 2;

}

int returnFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "return")
	{
		ofs << "<Statement> -> <Return>\n";
		ofs << "<Return> -> return<Expression>;\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			if (currentToken == ";")
			{
				getToken(counter, currentToken, mydata, datacounter, ofs);
				return 1;
			}
		}
	}
	else
	{
		return 2;
	}
}

int writeFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "write")
	{
		ofs << "<Statement> -> <Write>\n";
		ofs << "<Write> -> write(<Expression>);\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (currentToken == "(")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			addr = get_addr(mydata.token[counter], symbtable, ofs);//Assignment3 ASSIGN
			gen_instr(PUSHM, addr, instr_table, instr_num);//Assignment3 ASSIGN
			if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				if (currentToken == ")")
				{
					getToken(counter, currentToken, mydata, datacounter, ofs);
					if (currentToken == ";")
					{
						getToken(counter, currentToken, mydata, datacounter, ofs);
						return 1;
					}

				}
			}
		}
	}
	else
	{
		return 2;
	}

}

int readFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "read")
	{

		ofs << "<Statement> -> <Read>\n";
		ofs << "<Read> -> read(<Expression>);\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (currentToken == "(")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			if (ids(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				addr = get_addr(currentToken, symbtable, ofs);//Assignment3 ASSIGN
				gen_instr(PUSHM, addr, instr_table, instr_num);//Assignment3 ASSIGN
				if (currentToken == ")")
				{
					getToken(counter, currentToken, mydata, datacounter, ofs);
					if (currentToken == ";")
					{
						getToken(counter, currentToken, mydata, datacounter, ofs);
						return 1;
					}

				}
			}
		}
	}
	else
		return 2;
}

int whileFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "while")
	{
		addr = instr_num;//Assignment3 While
		gen_instr(LABEL, -999, instr_table, instr_num);//Assignment3 While
		ofs << "<Statement> -> <While>\n";
		ofs << "<While> -> while(<Condition> ) <Statement>\n";
		ofs << "<Condition> -> <Expression> <Relop> <Expression>\n";

		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (currentToken == "(")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			if (condition(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				if (currentToken == ")")
				{
					getToken(counter, currentToken, mydata, datacounter, ofs);
					if (statement(currentToken, counter, mydata, datacounter, ofs) == 1)
					{
						gen_instr(JUMP, addr, instr_table, instr_num);//Assignment3 While
						back_patch(instr_num, instr_table, stackCounter, arr);//Assignment3 While NEED TO IMPLEMENT THIS FUNCTION
						return 1;
					}

				}
			}
		}
	}
	else
		return 2;
}

int condition(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		if (relop(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				return 1;
			}

		}
	}
	else
	{
		return 2;
	}

}

int relop(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "==")
	{
		gen_instr(NEQ, -999, instr_table, instr_num);//Assignment3 WHILE
		push_jumpstack(arr, stackCounter, instr_num);//Assignment3 While
		gen_instr(JUMPZ, -999, instr_table, instr_num);//Assignment3 WHILE

		ofs << "<Relop> -> ==\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else if (currentToken == "!=")
	{
		gen_instr(EQU, -999, instr_table, instr_num);//Assignment3 WHILE
		push_jumpstack(arr, stackCounter, instr_num);//Assignment3 While
		gen_instr(JUMPZ, -999, instr_table, instr_num);//Assignment3 WHILE

		ofs << "<Relop> -> !=\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else if (currentToken == ">")
	{
		gen_instr(GRT, -999, instr_table, instr_num);//Assignment3 WHILE
		push_jumpstack(arr, stackCounter, instr_num);//Assignment3 While
		gen_instr(JUMPZ, -999, instr_table, instr_num);//Assignment3 WHILE

		ofs << "<Relop> -> >\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else if (currentToken == "<")
	{
		gen_instr(LES, -999, instr_table, instr_num);//Assignment3 WHILE
		push_jumpstack(arr, stackCounter, instr_num);//Assignment3 While
		gen_instr(JUMPZ, -999, instr_table, instr_num);//Assignment3 WHILE

		ofs << "<Relop> -> <\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		return 1;
	}
	else
	{
		return 2;
	}
}

int assign(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	string Save;//Assignment3 ASSIGN

	if (mydata.lexeme[counter] == "identifier")
	{
		Save = mydata.token[counter];//Assignment3 ASSIGN
		ofs << "<Statement> -> <Assign>\n";
		ofs << "<Assign> -> <Identifier> ::= <Expression;>\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);


		if (currentToken == ":=")
		{
			getToken(counter, currentToken, mydata, datacounter, ofs);
			ofs << "<Expression> ::= <Term><Expression Prime>\n";

			if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				addr = get_addr(Save, symbtable, ofs);//Assignment3 ASSIGN
				gen_instr(POPM, addr, instr_table, instr_num);//Assignment3 ASSIGN
				if (currentToken == ";")
				{
					getToken(counter, currentToken, mydata, datacounter, ofs);
					return 1;
				}

			}
		}
	}
	else
	{
		return 2;

	}

}

int expression(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (term(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
		else if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
		{
			ofs << "<Expression Prime> ::= E\n";
			return 1;
		}
	}
	else
		return 2;

}

int expressionPrime(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "+")
	{
		ofs << "<ExpressionPrime> ::= +<Term> <Expression Prime>\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (term(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			gen_instr(ADD, -999, instr_table, instr_num);//Assignment3 ASSIGN
			if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				return 1;
			}
			else if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
			{
				ofs << "<ExpressionPrime> :: = E\n";
				return 1;
			}
		}
	}
	else if (currentToken == "-")
	{
		ofs << "<ExpressionPrime> ::= -<Term> <Expression Prime>";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (term(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			gen_instr(SUB, -999, instr_table, instr_num);//Assignment3 ASSIGN
			if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				return 1;
			}
			else if (expressionPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
			{
				ofs << "<ExpressionPrime> :: = E\n";
				return 1;
			}
		}
	}
	else
	{
		return 3; // Empty 
	}

}

int term(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (factor(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
		else if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
		{
			ofs << "<Term Prime> ::= E\n";
			return 1;
		}
	}
	else
		return 2;
}

int termPrime(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "*")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (factor(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			gen_instr(MUL, -999, instr_table, instr_num);//Assignment3 ASSIGN
			if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				return 1;
			}
			else if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
			{
				return 1;
			}
		}
	}
	else if (currentToken == "/")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		if (factor(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			gen_instr(DIV, -999, instr_table, instr_num);//Assignment3 ASSIGN
			if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 1)
			{
				return 1;
			}
			else if (termPrime(currentToken, counter, mydata, datacounter, ofs) == 3)
			{
				return 1;
			}
		}
	}
	else
	{
		return 3; // term prime is empty
	}

}

int factor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "-")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		if (primary(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
	}
	else if (primary(currentToken, counter, mydata, datacounter, ofs) == 1)
	{
		return 1;
	}
	else
	{
		return 2;
	}

}

int primary(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (mydata.lexeme[counter] == "identifier")
	{
		addr = get_addr(mydata.token[counter], symbtable, ofs);//Assignment3 ASSIGN
		gen_instr(PUSHM, addr, instr_table, instr_num);//Assignment3 ASSIGN

		ofs << "<Expression> ::= <Term><Expression Prime>\n";
		ofs << "<Term> ::= <Factor><Term Prime>\n";
		ofs << "<Factor> ::= <Primary>\n";
		ofs << "<Primary> ::= <Identifier> <Primary Factor>\n";

		getToken(counter, currentToken, mydata, datacounter, ofs);

		if (primaryFactor(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			return 1;
		}
		else if (primaryFactor(currentToken, counter, mydata, datacounter, ofs) == 3)
		{
			ofs << "<Primary Factor> ::= E\n";
			return 1;
		}
	}
	else if (mydata.lexeme[counter] == "integer")
	{
		//addr = get_addr(mydata.token[counter], symbtable, ofs);//Assignment3 ASSIGN
		//gen_instr(PUSHM, addr, instr_table, instr_num);//Assignment3 ASSIGN

		ofs << "<Expression> ::= <Term><Expression Prime>\n";
		ofs << "<Term> ::= <Factor><Term Prime>\n";
		ofs << "<Factor> ::= <Primary>\n";
		ofs << "<Primary> ::= <integer>\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		return 1;
	}
	else if (currentToken == "(")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		if (expression(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			if (currentToken == ")")
			{
				getToken(counter, currentToken, mydata, datacounter, ofs);
				return 1;
			}
		}
	}
	else if (currentToken == "true")
	{
		ofs << "<Expression> ::= <Term><Expression Prime>\n";
		ofs << "<Term> ::= <Factor><Term Prime>\n";
		ofs << "<Factor> ::= <Primary>\n";
		ofs << "<Primary> ::= true\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		return 1;
	}
	else if (currentToken == "false")
	{
		ofs << "<Expression> ::= <Term><Expression Prime>\n";
		ofs << "<Term> ::= <Factor><Term Prime>\n";
		ofs << "<Factor> ::= <Primary>\n";
		ofs << "<Primary> ::= false\n";
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		return 1;
	}
	else
	{
		return 2;
	}
}

int primaryFactor(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	if (currentToken == "(")
	{
		getToken(counter, currentToken, mydata, datacounter, ofs);
		//ofs << currentToken; //output token
		if (ids(currentToken, counter, mydata, datacounter, ofs) == 1)
		{
			if (currentToken == ")")
			{
				getToken(counter, currentToken, mydata, datacounter, ofs);
				return 1;
			}

		}

	}
	else
		return 3;
}

void getToken(int& counter, string& currentToken, struct data mydata, int datacounter, ofstream &ofs)
{
	if (counter < datacounter, ofs)
	{
		counter++;
		currentToken = mydata.token[counter];
		ofs << "Token: " << currentToken << "         ";
		ofs << "Line Number: " << mydata.line[counter] << endl;
	}

}

int emptyFunc(string& currentToken, int& counter, struct data mydata, int datacounter, ofstream &ofs)
{
	return 1;
}

//end of syntax functions

void symtable(struct data mydata, int &counter, struct symb &symbtable, int &counter2, int &scope, int &memorynum, int datacounter, ofstream &ofs)
{
	while (counter < datacounter)
	{
		if (mydata.token[counter] == "int")
		{
			counter++;
			while (mydata.token[counter] != ";")
			{
				if (mydata.token[counter] == "{")
				{
					scope++;
				}
				if (mydata.token[counter] == "}")
				{
					scope--;
				}
				if (mydata.token[counter] == ",")
				{
					counter++;
				}
				symbtable.identifier[counter2] = mydata.token[counter];
				symbtable.memory[counter2] = memorynum;
				symbtable.scope[counter2] = scope;
				symbtable.type[counter2] = "integer";
				memorynum++;
				counter2++;
				counter++;
			}

		}
		else if (mydata.token[counter] == "bool")
		{
			counter++;
			while (mydata.token[counter] != ";")
			{
				if (mydata.token[counter] == "{")
				{
					scope++;
				}
				if (mydata.token[counter] == "}")
				{
					scope--;
				}
				if (mydata.token[counter] == ",")
				{
					counter++;
				}
				symbtable.identifier[counter2] = mydata.token[counter];
				symbtable.memory[counter2] = memorynum;
				symbtable.scope[counter2] = scope;
				symbtable.type[counter2] = "boolean";
				memorynum++;
				counter2++;
				counter++;
			}

		}
		else
		{
			counter++;
		}
	}
}

void gen_instr(string op, int oprnd, struct instr &instr_table, int &instr_num)
{
	instr_table.address[instr_num] = instr_num;
	instr_table.op[instr_num] = op;
	instr_table.oprnd[instr_num] = oprnd;
	++instr_num;
}

int get_addr(string token, struct symb symbtable, ofstream &ofs)
{
	for (int counter = 0; counter < 100; ++counter)
	{
		if (token.compare(symbtable.identifier[counter]) == 0)
		{
			return symbtable.memory[counter];
		}
	}

}

int push_jumpstack(int arr[], int &stackCounter, int addr)
{
	arr[stackCounter] = addr;
	++stackCounter;
	return 0;
}

int back_patch(int jump_addr, struct instr &instr_table, int stackCounter, int arry[])
{
	int address = 0;
	address = arry[stackCounter-1];
	instr_table.oprnd[address] = jump_addr;
	return 0;
}