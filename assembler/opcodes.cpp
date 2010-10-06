/**
 * Provides functions to compile subroutines.
 */

#include "Help.h"
#include "Command.h"

typedef unsigned short int opcode_t;

const int opcodeVersion[] = {1,2,0};
Help assemblerHelp()
{
    using namespace std;
    cerr << "assemberhelp()" << endl;
    Help returnMe;
    returnMe["lda"] = "loads the value to the accumulator";
    returnMe["adda"] = "";
    returnMe["suba"] = "";
    returnMe["movaf"] = "";
    returnMe["pusha"] = "";
    returnMe["anda"] = "";
    returnMe["ora"] = "";
    returnMe["xora"] ="";
    returnMe["rra"] = "";
    returnMe["rla"] = "";
    returnMe["goto"] = "";
    returnMe["sleepa"] = "";
    returnMe["sett"] = "";
    returnMe["setd"] = "";
    returnMe["seta"] = "";
	returnMe["showclock"] = "Displays the clock, using time, date or alarm, depending on the input given to the controls.";
    return returnMe;
}

std::map<arg_t,opcode_t> assemblerTable()
{
    std::map<arg_t,opcode_t> returnMe;
    opcode_t opcode = 0;
    returnMe["lda"] = opcode++;
    returnMe["adda"] = opcode++;
    returnMe["suba"] = opcode++;
    returnMe["movaf"] = opcode++;
    returnMe["pusha"] = opcode++;
    returnMe["anda"] = opcode++;
    returnMe["ora"] = opcode++;
    returnMe["xora"] = opcode++;
    returnMe["rra"] = opcode++;
    returnMe["rla"] = opcode++;
    returnMe["inca"] = opcode++;
    returnMe["deca"] = opcode++;
    returnMe["bsa"] = opcode++;
    returnMe["bca"] = opcode++;
    returnMe["clra"] = opcode++;
    returnMe["goto"] = opcode++;
    returnMe["sett"] = opcode++;
    returnMe["setd"] = opcode++;
    returnMe["seta"] = opcode++;
    returnMe["sleep"] = opcode++;
	returnMe["showclock"] = opcode++;
    return returnMe;
}


