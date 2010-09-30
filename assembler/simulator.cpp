#include "opcodes.cpp"
#include "Parser.h"
#include "Help.h"

#define STACK_SIZE 8

typedef unsigned short mem_t;

using namespace std;

class Memory{
public:
	mem_t accumulator;
	mem_t exchange;
	vector<mem_t> stack;

	Memory : accumulator(0), exchange(0){stack.capacity(STACK_SIZE);}
};

class Program : private std::vector<mem_t>
{
public:
	iterator programCounter;
	
	void insert(const mem_t& newVal){bool wasEmpty = (this->size() == 0);this->push_back(newVal);if(wasEmpty){programCounter = this->begin();}}
	iterator& goto(const size_t& loc){programCounter = this->begin();
		for(size_t i = 0;i<loc;i++)
			programCounter++;
		return programCounter;
	}
	mem_t next(){return ++programCounter;}
	mem_t previous(){return --programCounter;}
	
	void reset(){this->clear();programCounter = this->begin();}
	
	bool loadHex(const std::string& fileName);
}

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
			currData >> buff;
			if(currData.fail())
			{
				std::cerr << "Invalid memory type: " << word << std::endl;
				return false;
			}
			gProg.insert(currData);
		}
	}
	return true;
}

string print(Command& command)
{
	size_t numArgs = command.getWords().size();

	if(numArgs == 0)
		return printHelp("");
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
		return gProg.continueProgram(gMemory,0);//program array, memory, programCounter
	}

}

string doOpcode(const opcode_t& op)
{
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
	Parser p;
	p.start(argc,argv);
	return 0;
}