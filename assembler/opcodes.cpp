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
returnMe["memset"] = " Usage: memset x val  --- sets the memory at x equal to val	goto store ; Usage: store d  --- moves the accumulator to the d location in page memory.";
returnMe["load"] = " Usage: load d  --- loads the page memory data, d, into the accumulator";
returnMe["memcpy"] = " Usage: memcpy d s --- copies the data in page memory, s, to page memory, d.";
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
returnMe["btn_pressed"] = opcode++;
returnMe["memset"] = opcode++;
returnMe["load"] = opcode++;
returnMe["memcpy"] = opcode++;
    return returnMe;
}

/**
 * Converts the int to a two-byte two's compliment picos
 * integer.
 * If the number exceeds 16-bits, an exception is thrown
 */
std::vector<mem_t> cast_int(const int& num) throw (GoodException)
{
  std::vector<mem_t> returnMe;
  if(num > 0x7fff || num < -0x7fff)
    throw GoodException("cast_int: integers can only use 16-bits.",DATA_FORMAT_ERROR);
  
  int the_byte = (num & 0xff00) >> 8;
  returnMe.push_back(the_byte);
  the_byte = num & 0xff;
  returnMe.push_back(the_byte);
 
  return returnMe;
}
std::vector<mem_t> cast_int(const float& num) throw (GoodException)
{
  return cast_int((int) num);
}
