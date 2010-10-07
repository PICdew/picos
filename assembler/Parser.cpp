#include "Parser.h"

Parser::Parser() {
#ifdef __USE_READLINE__
    rl_bind_key('\t', Parser::tab_complete);
#endif
    commands = new CommandWords();
    h = new Help();
    inputBuffer = new arg_t("");
    history = new args_t();
}

Parser::~Parser() {
    delete commands;
    delete h;
    delete history;
    h = 0;
    history = 0;
}

void Parser::main() {
    this->start();
}

void Parser::main(int argc, char **args) {
    args_t arrargs;
    for (int i = 1; i < argc; i++) {
        arrargs.push_back(args[i]);
    }
    main(arrargs);
}

void Parser::main(args_t& args) {
    if (args.size() >= 1 && args.at(0) == "--version") {
        std::cout << "Version " + Build::getVersion() + " built on " + Build::getBuild() << std::endl;
        return;
    }
    if (args.size() >= 1 && args.at(0) == "--build") {
        return;
    }

    if (args.size() >= 1 && (args.at(0) == "--help" || args.at(0) == "--usage")) {
        std::cout << DString("LinAl ") <<  Build::getVersion() << std::endl;
        std::cout << DString("Last built on: ") << Build::getBuild() << std::endl;
        std::cout << "Symbolic Calculator"<< std::endl;
        std::cout << "Usage: linal [options]"<< std::endl;
        std::cout << "Options:"<< std::endl;
        std::cout << "--build, Shows the build number and versions of all classes"<< std::endl;
        std::cout << "--calc <expression>, Calculates the given expression"<< std::endl;
        std::cout << "--version, Shows the version"<< std::endl;
        std::cout << "--gui, Starts LinAl graphically"<< std::endl;
        std::cout << "--console, Start LinAll within a console."<< std::endl;
        std::cout << "--help, this"<< std::endl;
        return;
    } else if (args.size() >= 1 && args.at(0) == "--console") {
        start();
    } else {
        start();
    }

}

Command * Parser::getCommand() {
    using namespace std;
    char inputLine[256]; // will hold the full input line

    try {
#ifndef _READLINE_H_
        cout << "> "; // print prompt
        cin.get(inputLine, 256);
#else
        strcpy(inputLine, readline("> "));
        add_history(inputLine);
#endif
    } catch (...) {
        throw DavidException("Input error");
    }
    cout.flush();
    cin.ignore(255, '\n');

    return new Command(inputLine);

}

/**
 * Print out a list of valid command words.
 */
void Parser::printHelp(arg_t& whatIsSaid) {
    if(whatIsSaid.size() != 0)
      {
	whatIsSaid = h->getHelp(whatIsSaid);
	return;
      }
    whatIsSaid = "For help, enter help <command>\nCommands:\n";
    for(Help::const_iterator it = h->begin();it != h->end();it++)
      whatIsSaid += it->first + " ";
}

void Parser::start() {
    std::cout << DString("Welcome to linal, version:") << getVersion() << "\nFor help with the program, enter help" << std::endl;
    // Enter the main command loop.  Here we repeatedly read commands and
    // execute them until the game is over.

    bool finished = false;
    while (!finished) {
        try {
            Command * command = getCommand();
            finished = processCommand(*command);
            delete command;
            command = 0;
        } catch (DavidException e) {
            std::cout << "Process Error: " << e.getMessage() << std::endl;
        }
    }
    std::cout << "Thank you.  Good bye." << std::endl;
}

bool Parser::processCommand(Command& command) {
    std::string dummyString = "";
    bool returnMe = processCommand(command, dummyString);
    return returnMe;
}

bool Parser::processCommand(Command& command, std::string& whatIsSaid) {
    bool wantToQuit = false;
    
    if (command.getCommandWord() != "save"  && command.getCommandWord() != "exit" && command.getCommandWord() != "quit" && command.getCommandWord() != "history")
        history->push_back(command.getWholeCommandString());

    std::string commandWord = command.getCommandWord();
    if (commandWord== "help"){
      whatIsSaid = command.getSecondWord();
        printHelp(whatIsSaid);
        std::cout << whatIsSaid << std::endl;
    } else if (commandWord == "version") {
        std::cout << "Version: " << Build::getVersion() << std::endl;
    } else if (commandWord == "history") {
        if (history->size() == 0) {
            std::cout << "No commands have been entered yet." << std::endl;
            return false;
        }
        std::cout << "Previous Commands:" << std::endl;
        int counter = 0;
        for (args_t::iterator it = history->begin();
                it != history->end(); it++)
                {
                std::cout << counter++ << ": " << *it << std::endl;
                }
    }else if (commandWord == "quit" || commandWord == "exit") {
        wantToQuit = true;
    }
    else
    {
      whatIsSaid = Command::print(command.getWholeCommandString());
      std::cout << whatIsSaid << std::endl;
    }

    return wantToQuit; /**/
}

arg_t Parser::getHistoryValue(Command& command) const {

    size_t historyCount = history->size() - 1;
    if (command.getCommandWord().substr(0, 2) != "!!")
        historyCount = (int) Double(command.getCommandWord().substr(1)).doubleValue();

    if (historyCount < 0)
        throw DavidException("I need a positive number for the place in history");

    if (historyCount >= history->size())
        throw DavidException("Ummm, that is in the future.");

    DString newCommand = history->at(historyCount);
    args_t words = command.getWords();
    for (size_t i = 1; i < words.size(); i++)
        newCommand += " " + words.at(i);
    return newCommand;

}

void Parser::setHelp(const Help& newHelp)
{
  delete h;
  h = new Help(newHelp);
}

#ifdef __USE_READLINE__
extern Help makeHelp();
int Parser::tab_complete(int a, int b) {
    Parser p;
    p.setHelp(makeHelp());
    arg_t blah = "";
    p.printHelp(blah);
    std::cout << blah << std::endl;
    return 0;
}
#endif
