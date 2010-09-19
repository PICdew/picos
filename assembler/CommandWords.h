#ifndef COMMANDWORDS_CPP
#define COMMANDWORDS_CPP

#include "DArray.h"
#include "DString.h"


namespace linal{
	class CommandWords
	{
		public:
			CommandWords();
			bool isCommand(DString&);
			DString showAll();
			//DArray getCommandWords();

		private: 
			utils::DArray<DString> * validCommands;
     };
}

#endif
