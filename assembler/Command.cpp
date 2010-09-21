#include "Command.h"

	/**
	* Creates a new Command with the specified words. 
	*
	* @param words String command supplied.
	*/
	Command::Command(const arg_t& _words)
	{
	  commandWord = "";
	  secondWord = "";
	  this->words = "";
	  
	  if(!words.size() != 0)
	    {
	      utils::StringTokenizer toke = utils::StringTokenizer(_words);
	      	while(toke->hasMoreTokens())
			{
			  std::string currString = toke->nextToken();
			  args.push_back(currString);
			}
                commandWord = args.at(0);
			if(args.size() >= 2)
                            *secondWord = args.at(1);
			delete toke;
			toke = 0;
	    }
	  else
	    {
	      words += "help me";
	      commandWord += "help";
	    }

	    
	}
  
    /**
     * Return true if this command was not a known command.
     */
    bool Command::isUnknown()
    {
      return !(new CommandWords())->isCommand(commandWord);
    }

    /**
     * Determines whether or not a command has a second word, which is usually 
     * the expression or parameter of the command
     */
	bool Command::hasSecondWord()
    {
        return (secondWord.size() > 0);
    }
    
    
    /**
     * Loads a file as a variable in the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
  std::vector<std::string> Command::load(DString fileName)
    {

      using namespace std;
      vector<string> returnMe();

      ifstream file (fileName.toCharArray());
      string line;
      if (file.is_open())
	{
          line = "";
	  while(!file.eof())
	    {
	      line << file;
              returnMe.push_back(line);
	    }
	}
      else
	throw DavidException("Could not open: " + fileName);

      return returnMe;
      
		
    }/**/

  
  arg_t Command::print(const Command& command)
  {
    return "finish print";
  }

