/**
 * Provides functions to compile subroutines.
 */

#include "Help.h"
#include "Command.h"

#define PAGE_SIZE 5//in bytes
#define HEADER_SIZE 2//size of the binary header (in bytes)
#define STACK_SIZE 8
#define EEPROM_SIZE 0x3d

typedef unsigned short opcode_t;
typedef unsigned short addr_t;
typedef unsigned short mem_t;


#define EOP_STR "0xff"

const int opcodeVersion[] = {1,3,0};

Help assemblerHelp()
{
    using namespace std;
    Help returnMe;
    returnMe["lda"] = "loads the value to the accumulator";
returnMe["adda"] = "";
returnMe["suba"] = "";
returnMe["movaf"] = "";
returnMe["pusha"] = "";
returnMe["popa"] = "";
returnMe["anda"] = "";
returnMe["ora"] = "";
returnMe["xora"] = "";
returnMe["rra"] = "";
returnMe["rla"] = "";
returnMe["inca"] = "";
returnMe["deca"] = "";
returnMe["bsa"] = "";
returnMe["bca"] = "";
returnMe["clra"] = "";
returnMe["goto"] = " Goes to a specific program line. ";
returnMe["display"] = " Displays the next two arguments";
returnMe["clear_display"] = "";
returnMe["display_time"] = " Displays the clock, using time, date or alarm, depending on the input given to the controls.";
returnMe["sett"] = "";
returnMe["setd"] = "";
returnMe["btn_pressed"] = "";
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
returnMe["clear_display"] = opcode++;
returnMe["display_time"] = opcode++;
returnMe["sett"] = opcode++;
returnMe["setd"] = opcode++;
    return returnMe;
}


