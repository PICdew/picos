#ifndef COMMANDWORDS_CPP
#define COMMANDWORDS_CPP

typedef std::string arg_t;
typedef std::vector<arg_t> args_t;

class CommandWords : public args_t
{
    public:
            CommandWords();
            bool isCommand(const arg_t&);
            arg_t showAll();
};

#endif
