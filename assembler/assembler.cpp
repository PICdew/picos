/** \Mainpage Assembler for the compilation of programs
 * for the PIC microcontroller.
 *
 * Created By David Coss, 2010
 * Free Software under the terms of the GNU Public License version 3.
 */

#include <map>
#include <string>
#include <iostream>
#include <strstream>
#include <stdlib.h>
#include <cmath>

#include "opcodes.cpp"

//this prevents the GUI from being used.
#define GUI_CPP 1
#include "StringTokenizer.h"
#include "Parser.h"

#include "Build.h"

#include "args_parser.cpp"

#define GENERAL_ERROR 1
#define IO_ERROR 2
#define SYNTAX_ERROR 3

using namespace std;
ios::fmtflags radix;
map<arg_t,int> equs;
map<arg_t,addr_t> free_store;//maps variable names to the number of elements in its array. addr_t = 1 for a single variable (instead of array); this is the default.
vector<string> precompiledCode;

Help makeHelp(){
  Help returnMe = assemblerHelp();
  returnMe["malloc"] = "Allocates memory for the specified type. Can optionally supply a number of variable spaces to allocate. Default type: ushort\n Possible Types: int (2bytes), float (2bytes), char(1byte), ushort(1byte)";
  return returnMe;
}//for tab complete
Help readline_help_function(){return assemblerHelp();}
const map<arg_t,opcode_t> lookupTable = assemblerTable();

string formatHex(const int& unformatted,ios::fmtflags currRadix)
{
  ostringstream os;
  os.precision(2);
  os.setf(currRadix,ios::basefield);
  os << unformatted;
  string returnMe = os.str();
  while(returnMe.size() < 2)
    returnMe = "0" + returnMe;
  return returnMe;
}

string formatHex(const string& unformatted)
{
  string semiformatted = unformatted;
  istringstream is;
  ios::fmtflags currRadix = radix;

  //check if format of the string overrides the default radix.
  if(semiformatted.substr(0,2) == "0x" || semiformatted.substr(0,2) == "0X")
    {
      currRadix = ios::hex;//because we've already formatted it into hex. So the "decimals" are literally in hex.
      semiformatted.erase(0,2);
    }
  else
    {
      switch(semiformatted.at(0))
	{
	case '.': case 'd': case 'D':
	  {
	    currRadix = ios::dec;
	    semiformatted.erase(0,1);
	    break;
	  }
	case 'o': case 'O':
	  {
	    currRadix = ios::oct;
	    semiformatted.erase(0,1);
	    break;
	  }
	default:
	  break;
	}
    }
  is.clear();is.str(semiformatted);is.setf(currRadix,ios::basefield);
  int val = -1;
  is >> val;
  
  if(is.fail())
    throw GoodException("Could not compile: " + unformatted);
  
  return formatHex(val,ios::hex);
}

std::string checkAndInsert(const Command& command)
{
  //if the command is eop (end of program), load that value
  if(command.getCommandWord() == "eop")
    {
      precompiledCode.push_back(EOP_STR);
      return "";
    }

	const args_t& tokens = command.getWords();
	const string& op = tokens.at(0);
	
	if(op == "malloc")//don't use an opcode for malloc it's used by the compiler only.
	  return "";

	ostringstream os;
	os << "Incorrect number of arguments for "  << command.getWholeCommandString() << ": " << tokens.size()-1;
	string errorMessage = os.str();
	switch(tokens.size() - 1)//number of arguments
	{
	case 1:
	{
		if(op == "sett" || op == "setd" || op == "seta")
			return errorMessage;
		break;
	}
	case 2:
	{
		if(op == "sett" || op == "setd" || op == "seta" || op == "memset" || op == "memcpy")
			break;
		return errorMessage;
	}
	}//end of switch

	if(op == "memset")
	  {
	    precompiledCode.push_back(tokens[0]);
	    precompiledCode.push_back(tokens[1]);
	    int val;
	    std::ostringstream buff;
	    std::istringstream int_buff(tokens[2]);
	    int_buff >> val;
	    if(int_buff.fail())
	      throw GoodException("check_and_insert: Invalid data value "+tokens[2],DATA_FORMAT_ERROR);
	    vector<mem_t> two_bytes = cast_int(val);
	    buff << hex << two_bytes.at(0);
	    precompiledCode.push_back(buff.str());buff.str("");
	    buff << hex << two_bytes.at(1);
	    precompiledCode.push_back(buff.str());
	    return "";
	  }

	for(args_t::const_iterator it = tokens.begin();it != tokens.end();it++)
		precompiledCode.push_back(*it);
	return "";
}

int total_num_pages(const std::map<arg_t,addr_t>& page_reqs)
{
  if(page_reqs.size() == 0)
    return 0;
 
  //calculate total memory requested (in bytes)
  int total_memory = 0;
  std::map<arg_t,addr_t>::const_iterator array;
  for(array = page_reqs.begin();array != page_reqs.end();array++)
    {
      total_memory += array->second;
    }
  return (int) ceil((float) total_memory/PAGE_SIZE) ;
}
	
//calculate the total byte count of the program binary
int total_binary_byte_count(const std::vector<std::string>& precompiled)
{
  return precompiled.size() + HEADER_SIZE;
}

addr_t free_store_resolve(const std::string& addr)
{
  addr_t memory_loc = 0;
  std::string variable = addr;
  std::istringstream buff;
  addr_t offset = 0;
  if(variable.find('[') != std::string::npos && variable.find('[') != variable.size() - 1)
    {
      if(variable.find(']') != std::string::npos)
	variable.erase(variable.find(']'),1);
      buff.str(variable.substr(variable.find('[')+1));
      variable = variable.substr(0,variable.find('['));
      buff >> offset;
      if(buff.fail())
	throw GoodException("free_store_resolve: " + addr + " does not represent a proper variable.",DATA_FORMAT_ERROR);
    }
  if(free_store.find(variable) == free_store.end())
    throw GoodException("free_store_resolve: " + addr + " was not declared.",UNDECLARED_VARIABLE);

  map<arg_t,addr_t>::const_iterator array;
  for(array = free_store.begin(); array != free_store.end();array++)
    {
      if(array->first == variable)
	{
	  if(offset >= array->second)
	    throw GoodException("free_store_resolve: The index " + addr + " exceeeded the bounds of the array.");
	  memory_loc += offset;//location of the byte if the free store was one long vector. Now convert to <BLOCK:INDEX> format.
	  offset = memory_loc;
	  memory_loc = ((addr_t)offset/PAGE_SIZE) << 4;
	  memory_loc += offset % PAGE_SIZE;
	  break;
	}
      memory_loc += array->second;
    }
  return memory_loc;
}

void compile(const args_t& precompiled, const string& filename)
{
	fstream file(filename.c_str(),std::ios::out);
	if(!file.is_open())
	{
		std::cerr << "Could not open " << filename << " for writting" << std::endl;
		return;
	}

	file << ":020000040000FA" << std::endl;//beginning line for 32-bit extension. Google Intel Hex for details.
	int checksum = 0;
	int counter = 0;
	int address = 0x4200;
	int hex;
	string currLine;

	currLine = formatHex(address,std::ios_base::hex);
	currLine += "00";
	checksum += (address & 0xff);
	checksum += ((address >> 8) & 0xff);
	
	//calculate the amount of pages needed in free store
	hex = total_num_pages(free_store);
	std::cout << "requesting " << hex << " page blocks." << std::endl;
	currLine += formatHex(hex,radix) + "00";
	checksum += hex;counter++;
	
	//calculate the total byte count of the program binary
	hex = total_binary_byte_count(precompiled);
	currLine += formatHex(hex,radix) + "00";
	checksum += hex;counter++;
	
	for(args_t::const_iterator it = precompiled.begin();it != precompiled.end();it++)
	{
		if(counter % 8 == 0)
		{
			currLine += formatHex(address,std::ios_base::hex);
			currLine += "00";
			checksum += (address & 0xff);
			checksum += ((address >> 8) & 0xff);
			address += 0x10;
		}
		if(lookupTable.find(*it) != lookupTable.end())
		{
			hex = lookupTable.at(*it);
			currLine += formatHex(hex,radix);
			checksum += hex;
		}
		else if(equs.find(*it) != equs.end())
		{
			hex = equs.at(*it);
			currLine += formatHex(hex,radix);
			checksum += hex;
		}
		else if(free_store.find(*it) != free_store.end() || it->find('[') != std::string::npos)
		  {
		    hex = free_store_resolve(*it);
		    currLine += formatHex(hex,radix);
		    checksum += hex;
		  }
		else
		{
			string _val;
			currLine += formatHex(*it);
			_val = *it;
			if(_val.substr(0,2) == "0x")
				_val.erase(0,2);
			istringstream is(_val);
			is.setf(radix,ios_base::basefield);
			is >> hex;
			if(is.fail())
			{
			  throw GoodException("Invalid variable: " + *it);
			}
			checksum += hex;
		}
		currLine += "00";
		counter++;
		if(counter % 8 == 0)
		{
			checksum &= 0xff;
			checksum ^= 0xff;
			checksum++;
			int numBytes = (currLine.size()-4)/4;
			file << ":" << ((numBytes < 10) ? "0" : "") << numBytes << currLine << formatHex(checksum,ios_base::hex) << endl;
			currLine = "";
			checksum = 0;
		}
	}
	if(counter % 8 != 0)
	{
		checksum &= 0xff;
		checksum ^= 0xff;
		checksum++;
		currLine +=  (checksum < 0x10) ? "0" : "";
		int numBytes = (currLine.size()-4)/4;
		file << ":" << ((numBytes < 10) ? "0" : "") << numBytes << currLine << formatHex(checksum,ios_base::hex) << endl;
		currLine = "";
		checksum = 0;
	}
	
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
		compile(precompiledCode,command.getSecondWord());
		return "Compiled. Saved as " + command.getSecondWord();
	}
	else if(command.getCommandWord() == "eop")
	  {
	    return checkAndInsert(command);
	  }
	else if(command.getCommandWord() == "radix")
	  {
	    if(command.getWords().size() != 2)
	      {
		switch(radix)
		  {
		  case ios::hex:
		    return "hex";
		  case ios::dec:
		    return "dec";
		  case ios::oct:
		    return "oct";
		  }
	      }
	    const string& newRadix = command.getSecondWord();
	    if(newRadix == "dec")
	      {
		radix = ios::dec;
	      }
	    else if(newRadix == "hex")
	      {
		radix = ios::hex;
	      }
	    else if(newRadix == "oct")
	      {
		radix = ios::oct;
	      }
	    else
	      {
		return "Usage: radix [dec | hex | oct]";
	      }
	    return "Radix set to " + newRadix;	    
	  }
	else if(command.getCommandWord() == "print")
	  {
	    vector<string>::const_iterator line;
	    int counter = 0;
	    ostringstream buff;
	    for(line = precompiledCode.begin();line < precompiledCode.end();line++)
	      buff << ++counter << ": " << *line << endl;
	    return buff.str();
	  }
	else if(command.getCommandWord() == "malloc")
	  {
	    const args_t& args = command.getWords();
	    string usage_statement = "Usage: malloc <Variable Type> <Variable Name> [size. Default = 1]";
	    if(args.size() < 3 || args.size() > 4)
	      return usage_statement;
	    addr_t array_size = 1;
	    size_t variable_size = 1;
	    std::string variable_type = args[1],variable_name = args[2];
	    if(variable_type == "int" || variable_type == "float")
	      variable_size = 2;
	    
	    if(args.size() == 4)
	      {
		istringstream size_buff(args[3]);
		size_buff >> array_size;
		if(size_buff.fail())
		  return usage_statement;
	      }
	    free_store[variable_name] = array_size*variable_size;
	  }
	else if(lookupTable.find(command.getCommandWord()) == lookupTable.end())
	  return "unknown command: " + command.getCommandWord();
	
    return checkAndInsert(command);
}

int doCommandLineCompile(struct assembler_arguments& args)
{
  std::string& outFilename = args.output_filename;
  std::string& inFilename = args.source_filename;
  std::string curr;

  std::fstream inFile;
  inFile.open(inFilename.c_str(),std::ios::in);
  if(!inFile.is_open())
    {
      std::cerr << "Could not open " << inFilename << std::endl;
      return IO_ERROR;
    }
  Parser p;
  Command *currLine;
  std::string response;
  while(!inFile.eof())
    {
      std::getline(inFile,curr);
      currLine = new Command(curr);
      p.processCommand(*currLine,response);
      if(response.find("unknown command:") != std::string::npos)
	{
	  delete currLine;
	  std::cerr << "Syntax Error: " << curr << std::endl;
	  return SYNTAX_ERROR;
	}
      if(args.verbosity > 0)
	std::cout << response << std::endl;
      delete currLine;
      currLine = 0;
    }
  if(currLine != 0)
    delete currLine;

  currLine = new Command("compile " + outFilename);
  p.processCommand(*currLine,response);
  if(args.verbosity > 0)
    std::cout << response << std::endl;
  delete currLine;
  currLine = 0;
  
  return 0;
}

int main(int argc, char **argv)
{
  radix = ios::hex;
  //so that there aren't multiple main's
  #define __HAVE_MAIN__ 1

  struct assembler_arguments args;
  args.source_filename = "";
  args.output_filename = "a.hex";

  parse_args(argc,argv,args);
  
  try{
    if(args.source_filename.size() > 0)
      return doCommandLineCompile(args);
  }
  catch(GoodException de)
    {
      std::cout << de << std::endl;
      return de.getErrType();
    }

  if(argc > 1)
	{
		try{
		  Parser p;
		  p.program_name = argv[0];
                  p.setHelp(makeHelp());
		  p.main(argc,argv);
		  return 0;
		}
		catch(GoodException de)
		{
			std::cout << de << std::endl;
			return de.getErrType();
		}
	}
	else{
		try{
			Parser p;
			p.program_name = argv[0];
			p.setHelp(assemblerHelp());
			p.main();
			return 0;
		}
		catch(GoodException de)
		{
			std::cout << de << std::endl;
			return de.getErrType();
		}
	}

}

