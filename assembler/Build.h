#ifndef BUILD_CPP
#define BUILD_CPP

#include "DString.h"

/**
4.5.4: Fixed bug in Function::doFunction(...) where random[] would try to retrieve an array of variable that does not exist.
4.5.3: Made Parser::getHistoryValue(...) and Parser::getSwapValue(...) public and const functions. Now,
    these functions are called by linalgui to retrieve history using "!", "!!" and swap using "^" in the
    style of bash.
4.5.2: Changed the result tree in linalgui to a QTreeWidget. Now the expression is a child of the result.
    This allows the user to see what expression lead to that result. Double clicking will still copy and
    paste the item to the input box.
4.5.1: Changed linal::Parser::start() to a public method, making linal::Parser::main() redundant.
4.5.0: Changes made to remain compatible with libdavid. Started revision for really small changes.
4.4: Added version functions to the UI. Finally combined command and function help routines.
4.3: Added a function to calculate redshift distance, if Cosmology.h is available. Otherwise, it is ignored. 
4.2: Added a function to LinAl to get built in paramters, such as PI. D_PI is defined in Double

4.1: For compatibility issues, Iterator was changed to DIterator in libdavid. This changes makes previous versions incompatible.

4.0: Completely new structure. Only thing that is new is the general setup and most class names. Templates are used (correctly). Also everything is organized with headers. There is also a liblinal library (static).

Changes (since 3.2)

3.2: Added roots of bessel functions of first and second kinds

3.6: Since 3.6, LinAl uses double instead of floating point precision
3.7: Complex Variable added(only complex numbers)
3.8: Added Save to console version. Fixed the bug in calliing functions without parameters
	Added conjugation of complex variables.
3.8.1: Fixed bug to prevent malformed expressions to crash the program
3.8.2: Fixed bug in FunctionsHelp to prevent a crash when help is requested for a Function that has not help information
3.8.3: Expanded functionality of Complex numbers, including returning an imaginary number when sqrt[-x] is called.
3.8.4: Changed the Help and FunctionHelp to use DHashMap instead of HashMap. This will save memory.
3.8.5: Added new function (cf Functions.cpp). Changed Command::define to better handle ans.
3.8.6: Added 2 new function (cf Functions.cpp).
3.9: Fixed AlPars to better handle negative values. neg[x] is still prefered.
3.9.1: Double will now catch 0^0 requests to exponent(double, double)
*/

class Build
{
 public:
  static DString getBuild(){return DString("5/31/2010 20:39");}
  static DString getVersion(){return getMajorVersion() + "." + getMinorVersion() +"."+getRevision();}
  static DString getMajorVersion(){return DString("4");}
  static DString getMinorVersion(){return DString("5");}
  static DString getRevision(){return DString("4");}
  static DString todo;
};

#endif
