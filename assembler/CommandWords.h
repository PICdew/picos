#ifndef COMMANDWORDS_CPP
#define COMMANDWORDS_CPP

#include <string>
#include <vector>

#include "libdnstd/DString.h"

typedef std::string arg_t;
typedef std::vector<arg_t> args_t;

class CommandWords : public args_t
{
    public:
            CommandWords(){this->push_back("quit");this->push_back("exit");}
            CommandWords(const args_t& validCommands);
            bool isCommand(const arg_t&);
            arg_t showAll();
};

#endif
