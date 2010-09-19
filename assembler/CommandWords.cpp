#include "CommandWords.h"

namespace linal{
	
	CommandWords::CommandWords()
	{
	  validCommands = new utils::DArray<DString>;
	  validCommands->put("history");
	  validCommands->put("about");
	  validCommands->put("clean");
	  validCommands->put("open");
	  validCommands->put("run");
	  validCommands->put("help");
	  validCommands->put("set");
	  validCommands->put("import");
	  validCommands->put("physics");
	  validCommands->put("last");
	  validCommands->put("list");
	  validCommands->put("define");
	  validCommands->put("det");
	  validCommands->put("load");
	  validCommands->put("print");
	  validCommands->put("save");
	  validCommands->put("remove");
	  validCommands->put("edit");
	  validCommands->put("dir");
	  validCommands->put("quit");
	  validCommands->put("version");
	  validCommands->put("exit");

	}

	bool CommandWords::isCommand(DString& aString)
    {
        for(ushort i = 0; i < validCommands->size(); i++) 
            if(validCommands->get(i) == aString)
                return true;
		return false;
    }

	DString CommandWords::showAll() 
    {
    	DString bean = "";
	
	DString * junks = validCommands->getArray();
	
	DString::alphabetize(junks,validCommands->size());
	
        for(int i = 0; i < validCommands->size(); i++) {
            bean += DString(" ") + junks[i];
        }

	delete [] junks;
	junks = 0;
        return bean + " and functions";
    }
}
