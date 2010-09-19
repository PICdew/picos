/** \mainpage Documentation of the LinAl 4
 * Created By David Coss, 2007
 */

#ifdef __DEBUG__
#define DEBUG_PRINT(x) std::cout << x << std::endl;
#else
#define DEBUG_PRINT(x)
#endif

//this prevents the GUI from being used.
#define GUI_CPP 1

#include "Parser.h"

#include "Build.h"

using namespace std;

int main(int argc, char **argv)
{
  //so that there aren't multiple main's
  #define __HAVE_MAIN__ 1

	if((argc > 1) && (DString(argv[1]) == "--todo"))
	{
		cout << Build::todo << endl;
		return 1;
	}
	if((argc > 1) && (DString(argv[1]) == "--version"))
	{
	  cout << "LinAl version: " << Build::getVersion() << endl;
		cout << "Last Build: " << Build::getBuild() << endl;
		return 1;
	}
	if(argc > 1)
	{
		try{
		  Parser p;
		  p.main(argc,argv);
		  return 0;
		}
		catch(DavidException de)
		{
			de.stdOut();
			return de.getCode();
		}
	}
	else{
		try{
			Parser p;
			p.main();
			return 0;
		}
		catch(DavidException de)
		{
			de.stdOut();
			return de.getCode();
		}
	}

}

