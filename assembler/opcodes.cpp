/**
 * Provides functions to compile subroutines.
 */

#include "Help.h"
#include "Command.h"

typedef unsigned short int opcode_t;

#define EOP_STR "0xff"

const int opcodeVersion[] = {1,3,0};

Help assemblerHelp()
{
    using namespace std;
    cerr << "assemberhelp()" << endl;
    Help returnMe;
    returnMe["lda"] = "loads the value to the accumulator";
    returnMe["adda"] = "";
    returnMe["suba"] = "";
    returnMe["movaf"] = "";
    returnMe["pusha"] = "Pushes a to the stack";
    returnMe["popa"] = "Pops the stack into a";
    returnMe["anda"] = "";
    returnMe["ora"] = "";
    returnMe["xora"] ="";
    returnMe["rra"] = "";
    returnMe["rla"] = "";
    returnMe["goto"] = "Goes to a specific program line.";
    returnMe["display"] = "Displays the top two values in the stack, top of stack is on the left.";
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
    returnMe["popa"] = opcode++;
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
    returnMe["display"] = opcode++;
    returnMe["sett"] = opcode++;
    returnMe["setd"] = opcode++;
    returnMe["seta"] = opcode++;
    returnMe["sleep"] = opcode++;
    returnMe["showclock"] = opcode++;
    return returnMe;
}


