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
    returnMe["sett"] = 0x0;
    returnMe["setd"] = 0x1;
    returnMe["seta"] = 0x2;
    returnMe["lda"] = 0x3;
    returnMe["add"] = 0x4;
    returnMe["suba"] = 0x5;
    returnMe["movaf"] = 0x6;
    returnMe["pusha"] = 0x7;
    returnMe["anda"] = 0x8;
    returnMe["ora"] = 0x9;
    returnMe["xora"] = 0xa;
    returnMe["sleep"] = 0xb;
    return returnMe;
}

const map<arg_t,int> lookupTable = assemblerTable();

std::string print_function(const Command& command)
{
    if(lookupTable.find(command.getCommandWord()) == lookupTable.end())
        return "unknown command: " + command.getCommandWord();
    ostringstream os;
    os << "0x" << std::hex  << lookupTable.at(command.getCommandWord());
    os << " " << command.getSecondWord();
    return os.str();
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

