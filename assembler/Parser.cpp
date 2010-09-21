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
    arg_t arrargs();
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

    if (strlen(inputLine) == 0 || std::string(inputLine) == "") {
        return new Command("help");
    } else {
        return new Command(inputLine);
    }
}

/**
 * Print out a list of valid command words.
 */
void Parser::printHelp(DString& whatIsSaid) {
    if (whatIsSaid.size() == 0)
        return;
    std::cout << DString("List of commands: ") << (whatIsSaid = commands->showAll()) << std::endl;
    std::cout << std::endl;
}

void Parser::start() {
    std::cout << DString("Welcome to linal, version:") << getVersion() << "\nFor help with the program, enter help" << std::endl;
    // Enter the main command loop.  Here we repeatedly read commands and
    // execute them until the game is over.

    bool finished = false;
    while (!finished) {
        try {
            Command * command = getCommand();

            if (command->getCommandWord().size() >= 2)
                if (command->getCommandWord().charAt(0) == '!') {
                    DString newCommand = getHistoryValue(command);
                    std::cout << newCommand);
                    *command = Command(newCommand);
                } else if (command->getCommandWord().charAt(0) == '^') {
                    DString newCommand = getSwapValue(command);
                    std::cout << newCommand);
                    *command = Command(newCommand);
                }

            finished = processCommand(command);
            delete command;
            command = 0;
        } catch (DavidException e) {
            std::cout << (DString) "Process Error: " + e.getMessage());
        }/**/
    }
    std::cout << "Thank you.  Good bye." << std::endl;
}

bool Parser::processCommand(linal::Command * command) {
    DString * dummyString = new DString;
    bool returnMe = processCommand(command, dummyString);
    delete dummyString;
    dummyString = 0;
    return returnMe;
}

bool Parser::processCommand(linal::Command * command, DString * whatIsSaid) {
    using linal::AlPars;
    bool wantToQuit = false;

    if (whatIsSaid == 0)
        whatIsSaid = new DString("");

    if (!command->getCommandWord().equals("save") && !command->getCommandWord().equals("exit") && !command->getCommandWord().equals("quit") && !command->getCommandWord().equals("history"))
        history->put(command->getWholeCommandString());


    if (command->isUnknown()) {
        linal::Command newCommand(DString("print ") + command->getCommandWord());
        DString printString = linal::Command::print(newCommand, *la);
        std::cout << printString);
        *whatIsSaid = printString;

        return false;
    }

    DString commandWord = command->getCommandWord();
    if (commandWord.equals("help") && command->getSecondWord() == "") {
        printHelp(whatIsSaid);
    } else if (commandWord.equals("help") && command->getSecondWord() != "") {

        if (command->getSecondWord() == "functions") {
            DString extraPart = "list";
            if (command->getWords().size() >= 3)
                extraPart = command->getWords().get(2);
            *whatIsSaid = utils::Functions::getHelp(extraPart);
            std::cout << *whatIsSaid);
        } else if (utils::Functions::isFunction(command->getSecondWord())) {
            *whatIsSaid = utils::Functions::getHelp(command->getSecondWord());
            std::cout << *whatIsSaid);
        } else {
            *whatIsSaid = h->getHelp(command->getSecondWord());
            std::cout << *whatIsSaid);
        }
    } else if (commandWord.equals("list")) {
        *whatIsSaid = command->list(*la);
        std::cout << *whatIsSaid);
    } else if (commandWord.equals("last")) {
        *whatIsSaid = la->ans->toDString();
        std::cout << *whatIsSaid);
    } else if (commandWord.equals("about")) {
        if (la->numberOfVariables() == 0) {
            std::cout << "There are no varibles.");
            return false;
        }
        DString lookMeUp = "all";
        if (command->hasSecondWord() && command->getSecondWord() != "")
            lookMeUp = command->getSecondWord();
        std::cout << la->aboutVariable(lookMeUp));
        return false;
    } else if (commandWord.equals("define")) {
        Command::define(*command);
    } else if (commandWord.equals("load") || commandWord.equals("run") || commandWord.equals("open")) {
        if (!command->hasSecondWord()) {
            std::cout << "Uh, what do you want me to open?");
            return false;
        }
        utils::DArray<DString> * toRun = 0;
        Command * tmpguy = new Command("blah");
        try {
            toRun = Command::load(command->getSecondWord());
            for (int i = 0; i < toRun->size(); i++) {
                *tmpguy = Command(toRun->get(i));
                processCommand(tmpguy);
            }
        } catch (DavidException de) {
            std::cout << de.getMessage());
        }
        delete tmpguy;
        delete toRun;
        return false;
    } else if (commandWord.equals("clean")) {
        la->cleanMatrices();
        std::cout << *whatIsSaid = "The unnecessary matrices have been removed.");
    } else if (commandWord.equals("version")) {
        std::cout << DString("Version: ") + Build::getVersion());
    } else if (commandWord.equals("print")) {
        using namespace std;
        *whatIsSaid = linal::Command::print(*command, *la);
        cout << *whatIsSaid << endl;
    }
    else if (commandWord.equals("save")) {
        if (!command->hasSecondWord()) {
            std::cout << "file name needed with save." << std::endl;
        } else {
            DString fileName = command->getSecondWord();
            Command::save(*command, fileName, history, *la);
            std::cout << "File saved as " << fileName << std::endl;
        }
    } else if (commandWord.equals("history")) {
        if (history->size() == 0) {
            std::cout << "No commands have been entered yet.");
            return false;
        }
        std::cout << "Previous Commands:");
        for (int i = 0; i < history->size(); i++)
            std::cout << Double(1.0 * i).toDString() + DString(": ") + history->get(i));
    } else if (commandWord.equals("physics")) {
        /*Object[] objects = Command.physics(command,la);
        David.say((String) objects[0]);
        la = (LinAl) objects[1];/**/
    }
    else if (commandWord.equals("remove")) {
        *la = Command::remove(*command, *la); /**/
    }
    else if (commandWord.equals("edit")) {
        //*la = Command.edit(*command,la);
    }
    else if (commandWord.equals("import")) {
        //*la = Command.importVariables(command,la);
    }
    else if (commandWord.equals("dir")) {
        //David.say(Command.dir(command,la));
    } else if (commandWord.equals("set")) {
        *whatIsSaid = Command::set(*command, *la);
        std::cout << *whatIsSaid);
    }
    else if (commandWord.equals("quit") || commandWord.equals("exit")) {
        wantToQuit = true;
    }

    return wantToQuit; /**/


}

DString Parser::getHistoryValue(Command * command) const {

    int historyCount = history->size() - 1;
    if (!command->getCommandWord().substring(0, 2).equals("!!"))
        historyCount = (int) Double(command->getCommandWord().substring(1)).doubleValue();

    if (historyCount < 0)
        throw DavidException("I need a positive number for the place in history");

    if (historyCount >= history->size())
        throw DavidException("Ummm, that is in the future.");

    DString newCommand = history->get(historyCount);
    utils::DArray<DString> words = command->getWords();
    for (int i = 1; i < words.size(); i++)
        newCommand += DString(" ") + words.get(i);
    return newCommand;

}

DString Parser::getSwapValue(Command * command) const {


    using utils::StringTokenizer;
    StringTokenizer tokie(command->getWholeCommandString(), "^");

    DString lhs, rhs;
    DavidException hopefullyNot("There need to be two ^ each with a string after them.");

    if (!tokie.hasMoreTokens())
        throw hopefullyNot;

    lhs = tokie.nextToken();

    if (!tokie.hasMoreTokens())
        throw hopefullyNot;

    rhs = tokie.nextToken();


    DString lastCommand;

    if (history->size() >= 1)
        lastCommand = history->get(history->size() - 1);
    else
        throw DavidException("There are no previous commands");



    StringTokenizer newTokie(lastCommand);

    DString newCommand;

    while (newTokie.hasMoreTokens()) {
        DString curr = newTokie.nextToken();
        if (curr == lhs)
            curr = rhs;

        newCommand += DString(" ") + curr;
    }


    return newCommand.trim();

}

DHashMap<DString> Parser::storedVariableArray(const LinAl * const la) {
    DHashMap<linal::LTree> variables = la->getCopyVariables();

    std::vector<DString> keys = variables.getKeys();
    std::vector<linal::LTree> objs = variables.getObjects();

    DHashMap<DString> returnMe;

    for (int i = 0; i < keys.size(); i++) {
        returnMe.put(keys[i], objs[i].toDString());
    }

    return returnMe;
}

int Parser::tab_complete(int a, int b) {
    Parser p;
    p.printHelp(0);
    return 0;
}

