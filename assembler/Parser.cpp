#include "Parser.h"

  Parser::Parser() 
    {
#ifdef __USE_READLINE__
      rl_bind_key ('\t',Parser::tab_complete);
#endif
        commands = new CommandWords();
        h = new Help();
	inputBuffer = new DString("");
	history = new utils::DArray<DString>;
    }

  Parser::~Parser() 
    {
		delete commands;
		delete h;
		delete history;
        h = 0;
	history = 0;
    }

	void Parser::main()
	{
		this->start();
	}

	void Parser::main(int argc,char **args)
	{
		DArray<DString> * arrargs = new DArray<DString>();
		for(int i=1;i<argc;i++)
		{
			arrargs->put(DString(args[i]));
		}
		main(arrargs);
	}

	void Parser::main(DArray<DString> * args)
    {
		if(args->size() >=1 && args->get(0) == DString("--version"))
        {
	  David::say(DString("Version ")+Build::getVersion()+" built on "+Build::getBuild());
			return;
        }
		if(args->length() >=1 && args->get(0).equals("--build"))
        {
			//Build.main(new String[]{""});
			return;
        }
	
						       
		   
						       
		if(args->length() >= 1 && (args->get(0) == "--help" || (args->get(0)).equals("--usage")))
		{
			David::say(DString("LinAl ")+Build::getVersion());
			David::say(DString("Last built on: ")+Build::getBuild());
			David::say("Symbolic Calculator");
			David::say("Usage: linal [options]");
			David::say("Options:");
			David::say("--build, Shows the build number and versions of all classes");
			David::say("--calc <expression>, Calculates the given expression");
			David::say("--version, Shows the version");
			David::say("--gui, Starts LinAl graphically");
			David::say("--console, Start LinAll within a console.");
			David::say("--help, this");
			return;
		}
		else if(args->length() >=1 && ((args->get(0)).equals("--gui") || args->get(0).equals("-x")) )
	    {
	      David::say("Compiled without GUI.");
	    }
		else if(args->length() >= 2 && (args->get(0).equals("--calc")))
	    {
			LinAl la;
			David::say(linal::Command::print(Command(DString("print ")+args->get(1)),la));
	    }
		else if(args->length() >= 1 && (args->get(0).equals("--console")))
		  {
		    start();
		  }
        else if(args->length() >= 2 && (args->get(0).equals("--run")))
        {
	  DString fileName = args->get(1);
	  DArray<DString> * toBeRun = Command::load(fileName);
	  Command * tmpCommand = new Command(fileName);
	  for(int i = 0;i<toBeRun->size();i++)
	    {
	      *tmpCommand = Command(toBeRun->get(i));
	      processCommand(tmpCommand);
	    }
	  delete history;
	  history = toBeRun;
	  delete tmpCommand;
	  tmpCommand = 0;
	  start();
        }
        else 
        {
            start();
        }
    
    } 

	Command * Parser::getCommand() 
    {
		using namespace std;
        char inputLine[256];   // will hold the full input line
				
		try{
#ifndef _READLINE_H_
		  cout << "> ";     // print prompt
			cin.get(inputLine,256);
#else
			strcpy(inputLine,readline("> "));
			add_history(inputLine);
#endif
		}
		catch(...)
		{
			throw DavidException("Input error");
		}
		cout.flush();
		cin.ignore(255,'\n');
						
		//StringTokenizer * tokenizer = new StringTokenizer((DString) inputLine);
		//if(tokenizer.hasMoreTokens() && commands.isCommand(tokenizer.nextToken()))
		if(strlen(inputLine) == 0 || DString(inputLine) == "")
		{
			return new Command(DString("help"));
		}
		else
		{
			return new Command(DString(inputLine));
		}
    }

	/**
     * Print out a list of valid command words.
     */
  void Parser::printHelp(DString * whatIsSaid)
    {
      if(whatIsSaid == 0)
	whatIsSaid = new DString("");
      David::say(DString("List of commands: ") + (*whatIsSaid = commands->showAll().getString()));
      David::say("");
      David::say(utils::Functions::getHelp("list"));
    }

  void Parser::start()
  {
    David::say(DString("Welcome to linal, version:")+getVersion()+DString("\nFor help with the program, enter help"));
    // Enter the main command loop.  Here we repeatedly read commands and
    // execute them until the game is over.
    
    bool finished = false;
    while (! finished) {
      try{
	Command * command = getCommand();
	
	if(command->getCommandWord().length() >= 2)
	  if(command->getCommandWord().charAt(0) == '!')
	    {
	      DString newCommand = getHistoryValue(command);
	      David::say(newCommand);
	      *command = Command(newCommand);
	    }
	  else if(command->getCommandWord().charAt(0) == '^')
	    {
	      DString newCommand = getSwapValue(command);
	      David::say(newCommand);
	      *command = Command(newCommand);
	    }
	
	finished = processCommand(command);
	delete command;
	command = 0;
      }
      catch(DavidException e)
	{
	  David::say((DString) "Process Error: "+e.getMessage());
	}/**/
    }
    std::cout << "Thank you.  Good bye." << std::endl;
  }
  

  bool Parser::processCommand(linal::Command * command)
  {
    DString * dummyString = new DString;
    bool returnMe = processCommand(command,dummyString);
    delete dummyString;
    dummyString = 0;
    return returnMe;
  }

	bool Parser::processCommand(linal::Command * command, DString * whatIsSaid)
	{
		using linal::AlPars;
		bool wantToQuit = false;

		if(whatIsSaid == 0)
		  whatIsSaid = new DString("");
			
		if(!command->getCommandWord().equals("save") && !command->getCommandWord().equals("exit") && !command->getCommandWord().equals("quit") && !command->getCommandWord().equals("history"))
		  history->put(command->getWholeCommandString());


		if(command->isUnknown()) 
		{
			linal::Command newCommand(DString("print ")+command->getCommandWord());
			DString printString = linal::Command::print(newCommand,*la);
			David::say(printString);
			*whatIsSaid = printString;
			
            return false;
        }

        DString commandWord = command->getCommandWord();
        if (commandWord.equals("help") && command->getSecondWord() == "")
        {
	  printHelp(whatIsSaid);
        }
        else if(commandWord.equals("help") && command->getSecondWord() != "")
        {
				
			if(command->getSecondWord() == "functions")
			{
				DString extraPart = "list";
				if(command->getWords().size() >= 3)
					extraPart = command->getWords().get(2);
				*whatIsSaid = utils::Functions::getHelp(extraPart);
				David::say(*whatIsSaid);
			}
			else if(utils::Functions::isFunction(command->getSecondWord()))
			  {
			    *whatIsSaid = utils::Functions::getHelp(command->getSecondWord());
			    David::say(*whatIsSaid);
			  }
			else
			  {
			    *whatIsSaid = h->getHelp(command->getSecondWord());
				David::say(*whatIsSaid);
			  }
        }
        else if(commandWord.equals("list"))
        {
			*whatIsSaid = command->list(*la);
			David::say(*whatIsSaid);
        }
	else if(commandWord.equals("last"))
	  {
	    *whatIsSaid = la->ans->toDString();
	    David::say(*whatIsSaid);
	  }
	else if(commandWord.equals("about"))
	  {
	    if(la->numberOfVariables() == 0)
	      {
		David::say("There are no varibles.");
		return false;
	      }
	    DString lookMeUp = "all";
	    if(command->hasSecondWord() && command->getSecondWord() != "")
	      lookMeUp = command->getSecondWord();
	    David::say(la->aboutVariable(lookMeUp));
	    return false;
	  }
        else if (commandWord.equals("define"))
		{
		  Command::define(*command);
		}
        else if(commandWord.equals("load") || commandWord.equals("run") || commandWord.equals("open"))
        {
	  if(!command->hasSecondWord())
	    {
	      David::say("Uh, what do you want me to open?");
	      return false;
	    }
	  utils::DArray<DString> * toRun = 0;
	  Command * tmpguy = new Command("blah");
	  try
	    {
	      toRun = Command::load(command->getSecondWord());
	      for(int i = 0;i<toRun->size();i++)
		{
		  *tmpguy = Command(toRun->get(i));
		  processCommand(tmpguy);
		}
	    }
	  catch(DavidException de)
	    {
	      David::say(de.getMessage());
	    }
	      delete tmpguy;
	      delete toRun;
	      return false;
        }
        else if(commandWord.equals("clean"))
	  {
	    la->cleanMatrices();
	    David::say(*whatIsSaid = "The unnecessary matrices have been removed.");
	  }
	else if(commandWord.equals("version"))
	  {
	    David::say(DString("Version: ") + Build::getVersion());
	  }
        else if(commandWord.equals("print"))
		{
			using namespace std;
			*whatIsSaid = linal::Command::print(*command,*la);
			cout << *whatIsSaid << endl;
		}    
        else if(commandWord.equals("save"))
        {
			if(!command->hasSecondWord())
			{
				std::cout << "file name needed with save." << std::endl;
			}
			else
			{
				DString fileName = command->getSecondWord();
				Command::save(*command,fileName,history,*la);
				std::cout << "File saved as " << fileName << std::endl;
			}
         }
        else if(commandWord.equals("history"))
	  {
	    if(history->size() == 0)
	      {
		David::say("No commands have been entered yet.");
		return false;
	      }
	    David::say("Previous Commands:");
	    for(int i = 0;i<history->size();i++)
	      David::say(Double(1.0*i).toDString()+DString(": ")+history->get(i));
	  }
        else if(commandWord.equals("physics"))
        {
			/*Object[] objects = Command.physics(command,la);
			David.say((String) objects[0]);
			la = (LinAl) objects[1];/**/
        }
                
         else if(commandWord.equals("remove"))
         {
	   *la = Command::remove(*command,*la);/**/
         }

		else if(commandWord.equals("edit"))
		{
			//*la = Command.edit(*command,la);
		}
		
		else if(commandWord.equals("import"))
		{
			//*la = Command.importVariables(command,la);
		}

		else if(commandWord.equals("dir"))
		{
			//David.say(Command.dir(command,la));
		}
		else if(commandWord.equals("set"))
		{
		  *whatIsSaid = Command::set(*command,*la);
			David::say(*whatIsSaid);
		}
	            
		else if (commandWord.equals("quit") || commandWord.equals("exit")) {
			wantToQuit = true;
		}

		return wantToQuit;/**/

	   
	}
DString Parser::getHistoryValue(Command * command) const
{

  int historyCount = history->size() - 1;
  if(!command->getCommandWord().substring(0,2).equals("!!"))
    historyCount = (int) Double(command->getCommandWord().substring(1)).doubleValue();
  
  if(historyCount < 0)
    throw DavidException("I need a positive number for the place in history");
  
  if(historyCount >= history->size())
    throw DavidException("Ummm, that is in the future.");

  DString newCommand = history->get(historyCount);
  utils::DArray<DString> words = command->getWords();
  for(int i = 1; i< words.size();i++)
    newCommand += DString(" ") + words.get(i);
  return newCommand;
	 
}

DString Parser::getSwapValue(Command * command) const
{


  using utils::StringTokenizer;
  StringTokenizer tokie(command->getWholeCommandString(),"^");

  DString lhs,rhs;
  DavidException hopefullyNot("There need to be two ^ each with a string after them.");

  if(!tokie.hasMoreTokens())
    throw hopefullyNot;

  lhs = tokie.nextToken();

  if(!tokie.hasMoreTokens())
    throw hopefullyNot;

  rhs = tokie.nextToken();


  DString lastCommand;

  if(history->size() >= 1)
    lastCommand = history->get(history->size() - 1);
  else
    throw DavidException("There are no previous commands");



  StringTokenizer newTokie(lastCommand);

  DString newCommand;

  while(newTokie.hasMoreTokens())
    {
      DString curr = newTokie.nextToken();
      if(curr == lhs)
	curr = rhs;

      newCommand += DString(" ") + curr;
    }


  return newCommand.trim();

}

  DHashMap<DString> Parser::storedVariableArray(const LinAl * const la)
  {
    DHashMap<linal::LTree> variables = la->getCopyVariables();
    
    std::vector<DString> keys = variables.getKeys();
    std::vector<linal::LTree> objs = variables.getObjects();

    DHashMap<DString> returnMe;

    for(int i = 0;i<keys.size();i++)
      {
	returnMe.put(keys[i],objs[i].toDString());
      }
    
    return returnMe;
  }

  int Parser::tab_complete(int a, int b)
  {
    Parser p;
    p.printHelp(0);
    return 0;
  }
	
