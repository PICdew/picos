#ifndef COMMAND_CPP
#define COMMAND_CPP

#include <iostream>
#include <fstream>

#include "AlPars.h"
#include "StringTokenizer.h"
#include "CommandWords.h"
#include "Help.h"

#ifdef __DEBUG__
#define DEBUG_PRINT(x) std::cout << x << std::endl;
#else
#define DEBUG_PRINT(x) ;
#endif


namespace linal{

	class Command
	{
		public:
			Command(const DString&);

			static DString getVersion(){return (DString) "2.0";}

			DString getCommandWord();
			DString getSecondWord();
			bool isUnknown();
			bool hasSecondWord();
			utils::DArray<DString>& getWords() const ;
			DString getWholeCommandString() const{return *words;}

			//Calculation methods
			static DString list(LinAl& la);
			static LinAl& define(const Command& command,LinAl& la);
			static LinAl& remove(const Command& command,LinAl& la);
			static DString print(const Command& command,LinAl& la);
	  static DString set(const linal::Command& command,LinAl& la);
	  static LinAl& save(const Command& command, DString& fileName, utils::DArray<DString> * output,LinAl& la);
	  static utils::DArray<DString> * load(DString fileName);

	  //Helpers
	  static utils::DArray<utils::DArray<linal::LTree> > prepareForFunctions(utils::DArray<utils::DArray<DString> > junks, DString * functionName, LinAl& la);

		private:
			DString * words;
			DString * commandWord;
			DString * secondWord;
	        DArray<DString> * args;
	};
}

#endif
