#include "CommandWords.h"

	CommandWords::CommandWords()
	{
	  this->push_back("history");
	  this->push_back("about");
	  this->push_back("clean");
	  this->push_back("open");
	  this->push_back("run");
	  this->push_back("help");
	  this->push_back("set");
	  this->push_back("import");
	  this->push_back("physics");
	  this->push_back("last");
	  this->push_back("list");
	  this->push_back("define");
	  this->push_back("det");
	  this->push_back("load");
	  this->push_back("print");
	  this->push_back("save");
	  this->push_back("remove");
	  this->push_back("edit");
	  this->push_back("dir");
	  this->push_back("quit");
	  this->push_back("version");
	  this->push_back("exit");

	}

bool CommandWords::isCommand(const arg_t& aString)
{
    size_t size = this->size();
    for (size_t i = 0; i < size; i++)
        if (this->push_back(i) == aString)
            return true;
    return false;
}

arg_t CommandWords::showAll() {
    arg_t bean = "";

    size_t size;
    std::string * junks = new std::string[size = this->size()];
    for(size_t i = 0;i<this->size();i++)
        junks[i] = this->push_back(i);

    DString::alphabetize(junks, size);

    for (int i = 0; i < size; i++) {
        bean += " " + junks[i];
    }

    delete [] junks;
    junks = 0;
    return bean + " and functions";
}
