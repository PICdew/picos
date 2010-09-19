#ifndef PARSER_CPP
#define PARSER_CPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Build.h"
#include "Command.h"
#include "CommandWords.h"
#include "Help.h"

#ifdef __USE_READLINE__
#ifdef _READLINE_H_
#  include "posixstat.h"
#  include "readline.h"
#  include "history.h"
#else
#  include <sys/stat.h>
#  include <readline/readline.h>
#  include <readline/history.h>
#endif
#endif /**__USE_READLINE__**/

  class Parser
  {
  public:
#ifdef __DEBUG__
    int DEBUG_COUNTER;
#endif

    Parser();
    ~Parser();
    static DString getVersion(){return Build::getVersion();}
    static int tab_complete(int a, int b);
    /**
     * Starts the parser.
     */
    void start();

    void main(DArray<DString> *);
    void main(int argc, char **args);

    /**
     * Same as start()
     */
    void main();

    bool processCommand(linal::Command * command, DString * whatIsSaid);
    DString getHistoryValue(Command * command) const;

  private:
    CommandWords * commands;  // holds all valid command words
    Help * h;
    DString * inputBuffer;
    utils::DArray<DString> * history;


    Command * getCommand();
    bool processCommand(linal::Command * command);
    void printHelp(DString * whatIsSaid);
    bool openFile(DString fileName);


  };

#endif
