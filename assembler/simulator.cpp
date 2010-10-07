#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "opcodes.cpp"
#include "Parser.h"
#include "Help.h"

#define STACK_SIZE 8
#define EEPROM_SIZE 0x3d
typedef unsigned short int mem_t;

using namespace std;

Parser gParser;


class Memory{
public:
	mem_t accumulator;
	mem_t exchange;
        mem_t time[2];
        mem_t date[2];
        mem_t alarm[2];
	vector<mem_t> stack;
        vector<mem_t> ram;

        Memory() : accumulator(0), exchange(0){stack.clear();ram.resize(EEPROM_SIZE,0);}
  void clear();
}gMemory;

void Memory::clear()
{
  accumulator =  exchange = 0;
  for(size_t i = 0;i<2;i++)
    {
      time[i] = date[i] = alarm[i] = 0;
    }
  stack.clear();
  ram.clear();
  ram.resize(EEPROM_SIZE,0);
}

std::ostream& operator<<(std::ostream& stream, Memory& mem)
{
    using std::endl;
    stream.setf(std::ios::dec,std::ios::basefield);
    stream << "Time: " << mem.time[0] << ":" << mem.time[1] << std::endl;
    stream << "Date: " << mem.date[0] << "/" << mem.date[1] << std::endl;
    stream << "Alarm: " << mem.alarm[0] << ":" << mem.alarm[1] << std::endl;
    stream.setf(std::ios::hex,std::ios::basefield);
    stream << "A: " << mem.accumulator << endl;
    stream << "X: " << mem.exchange << endl;
    stream << "RAM: " << endl;
    std::vector<mem_t>::const_iterator it;
    int counter = 0;
    stream << "xxh 0      4         8       C" << endl;
    for(it = mem.ram.begin();it != mem.ram.end();it++)
    {
        if(counter % 16 == 0)
            stream << counter << ": ";
	if(counter == 0)
	  stream << " ";
	if(*it < 0x10)
	  stream << "0";
        stream << *it;
        counter++;
        if(counter % 8 == 0)
            stream << " ";
        if(counter % 16 == 0)
            stream << endl;
    }
    if(counter % 16 != 0)
        stream << endl;
    stream << "Stack: " << endl;
    if(mem.stack.size() == 0)
      stream << "(empty)";
    else
      for(it = mem.stack.begin();it != mem.stack.end();it++)
        stream << *it << endl;

    return stream;
}

class Program : private std::vector<mem_t>
{
public:
    typedef std::vector<mem_t>::iterator pcl;
    std::vector<mem_t>::iterator programCounter;
	
	void insert(const mem_t& newVal){bool wasEmpty = (this->size() == 0);this->push_back(newVal);if(wasEmpty){programCounter = this->begin();}}
        const mem_t& jmp(const size_t& loc){programCounter = this->begin();
		for(size_t i = 0;i<loc;i++)
			programCounter++;
                return *programCounter;
	}
        const mem_t& next(){return *(++programCounter);}
        const mem_t& previous(){return *(--programCounter);}

        pcl first(){return this->begin();}
        pcl eop(){return this->end();}
	
  void restart(){programCounter = this->begin();}
  void reset(){this->clear();restart();}
	
	bool loadHex(const std::string& fileName);
        pcl continueProg(Memory& mem, pcl startLoc, pcl endLoc);
        void simCommand(const mem_t& memval, Memory& mem);
}gProg;

bool Program::loadHex(const std::string& fileName)
{
	gProg.reset();
	std::string currLine;
	std::fstream file;
	file.open(fileName.c_str(),std::ios::in);
	if(!file.is_open())
	{
		std::cerr << "Could not open " << fileName << std::endl;
		return false;
	}
	file >> currLine;//don't need 32-bit tag.
	while(file.is_open() && !file.eof())
	{
		file >> currLine;
		// ":" + byte size + 2-byte addres + byte record type + n-byte data + checksum
		currLine.erase(0,7);
		std::string recordType = currLine.substr(0,2);
		currLine.erase(0,2);
		if(recordType != "00")
			continue;
		currLine.erase(currLine.size()-2,2);
		while(currLine.size() > 0)
		{
			std::string word = currLine.substr(0,2);
			currLine.erase(0,4);
			mem_t currData;
			std::istringstream buff(word);
			buff.setf(std::ios_base::hex,std::ios_base::basefield);
			buff >> currData;
			if(buff.fail())
			{
				std::cerr << "Invalid memory type: " << word << std::endl;
				return false;
			}
			gProg.insert(currData);
		}
	}
	gProg.restart();
	return true;
}

void Program::simCommand(const mem_t& memval, Memory& mem)
{
  std::string command = "";
  std::map<std::string,mem_t> opcodes = assemblerTable();
  for(std::map<std::string,mem_t>::const_iterator it = opcodes.begin();
      it != opcodes.end();it++)
    {
      if(it->second == memval)
	command = it->first;
    }
  if(command.size() == 0)
    return;

  if(command == "lda")
        {
            mem.accumulator = *(++programCounter);
            return;
        }
  if(command == "adda")
        {
            mem.accumulator += *(++programCounter);
	    return;
        }
  if(command == "suba")
        {
            mem.accumulator -= *(++programCounter);
            return;
        }
  if(command == "movaf")
        {
            mem_t val = *(++programCounter);
            if(val >= mem.ram.size())
            {
                std::cerr << "Value exceeded memory size" << std::endl;
                return;
            }
            mem.ram[val] = mem.accumulator;
            return;
        }
  if(command == "pusha")
        {
            mem.stack.push_back(*(++programCounter));
	    return;
	}
  if(command == "popa")
    {
      mem.accumulator = mem.stack.at(mem.stack.size() - 1);
      return;
    }
  if(command == "anda")
        {
            mem.accumulator &= *(++programCounter);
            return;
        }
  if(command == "ora")
        {
            mem.accumulator |= *(++programCounter);
            return;
        }
  if(command == "xora")
        {
            mem.accumulator ^= *(++programCounter);
            return;
        }
  if(command == "rra")
        {
            mem.accumulator >> *(++programCounter);
            return;
        }
  if(command == "rla")
        {
            mem.accumulator << *(++programCounter);
            return;
        }
  if(command == "inca")
        {
            mem.accumulator++;
            return;
        }
  if(command == "deca")
        {
            mem.accumulator--;
            return;
        }
  if(command == "bsa")
        {
            mem_t val = *(++programCounter);
            mem_t bitmask = 1;
            bitmask << val;
            mem.accumulator |= bitmask;
            return;
        }
  if(command == "bca")
        {
            mem_t val = *(++programCounter);
            mem_t bitmask = 1;
            bitmask << val;
            bitmask ^= 0xff;
            mem.accumulator &= bitmask;
            return;
        }
  if(command == "clra")
      {
            mem.accumulator = 0;
            return;
        }
  if(command == "goto")
      {
	size_t gotoVal = size_t(*(++programCounter));
	programCounter = this->begin();
	for(size_t i = 0;i<gotoVal + 1;i++)
	  programCounter++;
	return;
      }
  if(command == "display")
      {
	std::cout << "Displaying: " << mem.stack[0] << " " << mem.stack[1] << std::endl;
	return;
      }
  if(command == "sett")
        {
            mem.time[0] = *(++programCounter);
            mem.time[1] = *(++programCounter);
            return;
        }
  if(command == "setd")
        {
            mem.date[0] = *(++programCounter);
            mem.date[1] = *(++programCounter);
            return;
        }
  if(command == "seta")
        {
            mem.alarm[0] = *(++programCounter);
            mem.alarm[1] = *(++programCounter);
            return;
        }
  if(command == "sleep")
      {
	mem_t sleepVal = *(++programCounter);
	std::cout << (mem.time[0] += (sleepVal / 60)) << "m " << (mem.time[1] += sleepVal % 60) << "s" << std::endl;
	return;
      }
  if(command == "showclock")
      {
	std::cout << "Time: " << mem.time[0] << ":" << mem.time[1] << std::endl;
	return;
      }
  return;
}

Program::pcl Program::continueProg(Memory& mem, pcl startLoc, pcl endLoc)
{
    this->programCounter = startLoc;
    for(;this->programCounter != endLoc;programCounter++)
    {
        simCommand(*(this->programCounter),mem);
    }
    return this->programCounter;
}

string print_function(const Command& command)
{
	size_t numArgs = command.getWords().size();

	if(numArgs == 0)
        {
            return "";
        }
	arg_t arg = command.getCommandWord();
	args_t words = command.getWords();
	if(arg == "load" && numArgs == 2)
	{
		if(gProg.loadHex(words[1]))
			return "loaded " + words[1];
		else
			return "could not load " + words[1];
	}
	else if(arg == "run")
	{
                gProg.continueProg(gMemory,gProg.first(),gProg.eop());//program array, memory, programCounter
                return "done.";
	}
	else if(arg == "print")
	{
	  ostringstream buff;
	  buff.setf(ios_base::hex,ios_base::basefield);
	  buff << gMemory;
	  return buff.str();
	}
	else if(arg == "step")
	{
		gProg.continueProg(gMemory,gProg.programCounter,gProg.programCounter+1);
		return "stepped forward 1 function.";
	}
	else if(arg == "dump")
	{
		if(numArgs < 2)
		{
			return "need variable to dump";
		}
		if(words[1] == "program")
			{
				Program::pcl placeHolder = gProg.programCounter;
				gProg.restart();
				int lineCounter = 0;
				while(gProg.programCounter != gProg.eop())
					std::cout << lineCounter++ << ": " <<  gProg.next() << std::endl;
			}

		return "";
	}
	else if(arg == "clear")
	  {
	    gProg.reset();
	    gMemory.clear();
	    return "Program reset.";
	  }
    return "unknown command.";
}


Help makeHelp()
{
    Help returnMe;
    returnMe["load"] = "Loads hex file.\nUsage: load <filename>";
    returnMe["run"] = "Runs the program.";
    returnMe["print"] = "Prints memory.";
    return returnMe;
}

opcode_t getMemoryVal(const arg_t& arg, int& state)
{
	istringstream is(arg);
	opcode_t memval;
	is >> memval;
	if(is.fail())
	{
		state = 1;
		return 0xff;
	}
	state = 0;
	return memval;
}

int main(int argc, char** argv)
{
    gParser.setHelp(makeHelp());
        gParser.main(argc,argv);
	return 0;
}
