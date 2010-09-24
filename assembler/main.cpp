/** \mainpage Documentation of the LinAl 4
 * Created By David Coss, 2007
 */

#ifdef __DEBUG__
#define DEBUG_PRINT(x) std::cout << x << std::endl;
#else
#define DEBUG_PRINT(x)
#endif

#include <map>
#include <string>
#include <iostream>
#include <strstream>

//this prevents the GUI from being used.
#define GUI_CPP 1
#include "libdnstd/StringTokenizer.h"
#include "Parser.h"

#include "Build.h"

using namespace std;

map<arg_t,int> assemblerTable()
{
    map<arg_t,int> returnMe;
    int opcode = 0;
    returnMe["lda"] = opcode++;
    returnMe["adda"] = opcode++;
    returnMe["suba"] = opcode++;
    returnMe["movaf"] = opcode++;
    returnMe["pusha"] = opcode++;
    returnMe["anda"] = opcode++;
    returnMe["ora"] = opcode++;
    returnMe["xora"] = opcode++;
    returnMe["sleep"] = opcode++;
    returnMe["sett"] = opcode++;
	returnMe["setd"] = opcode++;
    returnMe["seta"] = opcode++;
    return returnMe;
}

const map<arg_t,int> lookupTable = assemblerTable();
map<arg_t,int> equs;
vector<string> precompiledCode;

std::string checkAndInsert(const Command& command)
{
	const args_t& tokens = command.getWords();
	const string& op = tokens.at(0);
	string errorMessage = "Incorrect number of arguments: " + command.getWholeCommandString();
	switch(tokens.size() - 1)//number of arguments
	{
	case 1:
	{
		if(op == "sett" || op == "setd" || op == "seta")
			return errorMessage;
	}
	case 2:
	{
		if(op == "sett" || op == "setd" || op == "seta")
			break;
		return errorMessage;
	}
	}
	for(args_t::const_iterator it = tokens.begin();it != tokens.end();it++)
		precompiledCode.push_back(*it);
	return "";
}

void compile(const args_t& precompiled, const string& filename)
{
	fstream file(filename.c_str(),std::ios::out);
	if(!file.is_open())
	{
		std::cerr << "Could not open " << filename << " for writting" << std::endl;
		return;
	}
	args_t compiled;
	for(args_t::const_iterator it = precompiled.begin();it != precompiled.end();it++)
	{
		if(lookupTable.find(*it) != lookupTable.end())
		{
			file << lookupTable.at(*it) << endl;
			continue;
		}
		if(equs.find(*it) != equs.end())
		{
			file << equs.at(*it) << endl;
			continue;
		}
		file << *it << endl;
	}
	std::cerr << "Verify format." << endl;
}

std::string print_function(const Command& command)
{
	if(command.getSecondWord() == "equ")
	{
		if(command.getWords().size() < 3)
			return "Usage: <variable> equ <literal>";
		istringstream is(command.getWords().at(2));
		int literalVal = -1;
		is >> literalVal;
		if(is.fail())
			return "equ requires an integer";
		equs[command.getCommandWord()] = literalVal;
		return command.getCommandWord() + " = " + command.getWords().at(2); 
	}
	else if(command.getCommandWord() == "compile")
	{
		if(command.getWords().size() != 2)
		{
			return "Usage: compile <filename>";
		}
		compile(precompiledWords,command.getSecondWord());
		return "Compiled. Saved as " + command.getSecondWord();
	}
	else if(lookupTable.find(command.getCommandWord()) == lookupTable.end())
        return "unknown command: " + command.getCommandWord();
	
    return checkAndInsert(command);
}

int main(int argc, char **argv)
{
  //so that there aren't multiple main's
  #define __HAVE_MAIN__ 1

	if((argc > 1) && (DString(argv[1]) == "--todo"))
	{
		cout << Build::todo << endl;
		return 1;
	}
	if((argc > 1) && (DString(argv[1]) == "--version"))
	{
	  cout << "LinAl version: " << Build::getVersion() << endl;
		cout << "Last Build: " << Build::getBuild() << endl;
		return 1;
	}
	if(argc > 1)
	{
		try{
		  Parser p;
		  p.main(argc,argv);
		  return 0;
		}
		catch(DavidException de)
		{
			de.stdOut();
			return de.getCode();
		}
	}
	else{
		try{
			Parser p;
			p.main();
			return 0;
		}
		catch(DavidException de)
		{
			de.stdOut();
			return de.getCode();
		}
	}

}

