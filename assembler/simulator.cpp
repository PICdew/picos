#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "opcodes.cpp"
#include "Parser.h"
#include "Help.h"

#define STACK_SIZE 8

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

        Memory() : accumulator(0), exchange(0){stack.clear();ram.resize(0x3d,0);}
}gMemory;

std::ostream& operator<<(std::ostream& stream, Memory& mem)
{
    using std::endl;
    stream.setf(std::ios::hex,std::ios::basefield);
    stream << "A: " << mem.accumulator << endl;
    stream << "X: " << mem.exchange << endl;
    stream << "RAM: " << endl;
    std::vector<mem_t>::const_iterator it;
    int counter = 0;
    for(it = mem.ram.begin();it != mem.ram.end();it++)
    {
        if(counter % 16 == 0)
            stream << counter << ": ";
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
	
	void reset(){this->clear();programCounter = this->begin();}
	
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
		// ":" + byte size + 4-byte addres + byte record type + n-byte data + checksum
		currLine.erase(0,11);
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
                        buff >> currData;
                        if(buff.fail())
			{
				std::cerr << "Invalid memory type: " << word << std::endl;
				return false;
			}
			gProg.insert(currData);
		}
	}
	return true;
}

void Program::simCommand(const mem_t& memval, Memory& mem)
{
    switch(memval)
    {
    case 0:
        {
            mem.accumulator = *(++programCounter);
            break;
        }
    case 1:
        {
            mem.accumulator += *(++programCounter);
            break;
        }
    case 2:
        {
            mem.accumulator -= *(++programCounter);
            break;
        }
    case 3:
        {
            mem_t val = *(++programCounter);
            if(val >= mem.ram.size())
            {
                std::cerr << "Value exceeded memory size" << std::endl;
                return;
            }
            mem.ram[val] = mem.accumulator;
            break;
        }
    case 4:
        {
            mem.stack.push_back(*(++programCounter));
            break;
        }
    case 5:
        {
            mem.accumulator &= *(++programCounter);
            break;
        }
    case 6:
        {
            mem.accumulator |= *(++programCounter);
            break;
        }
    case 7:
        {
            mem.accumulator ^= *(++programCounter);
            break;
        }
    case 8:
        {
            mem.accumulator >> *(++programCounter);
            break;
        }
    case 9:
        {
            mem.accumulator << *(++programCounter);
            break;
        }
    case 10:
        {
            std::cout << "sleeping" << std::endl;
            int a = 0;
            for(int i = 0;i<1e9;i++)
            {
                a++;
            }
            break;
        }
    case 11:
        {
            mem.accumulator++;
            break;
        }
    case 12:
        {
            mem.accumulator--;
            break;
        }
    case 13:
        {
            mem_t val = *(++programCounter);
            mem_t bitmask = 1;
            bitmask << val;
            mem.accumulator |= bitmask;
            break;
        }
    case 14:
        {
            mem_t val = *(++programCounter);
            mem_t bitmask = 1;
            bitmask << val;
            bitmask ^= 0xff;
            mem.accumulator &= bitmask;
            break;
        }
    case 15:
        {
            mem.accumulator = 0;
            break;
        }
    case 16:
        {
            mem.time[0] = *(++programCounter);
            mem.time[1] = *(++programCounter);
            break;
        }
    case 17:
        {
            mem.date[0] = *(++programCounter);
            mem.date[1] = *(++programCounter);
            break;
        }
    case 18:
        {
            mem.alarm[0] = *(++programCounter);
            mem.alarm[1] = *(++programCounter);
            break;
        }
    default:
        break;
    }
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
            std::cout << gMemory << std::endl;
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
