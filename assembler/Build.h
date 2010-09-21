#ifndef BUILD_CPP
#define BUILD_CPP

#include <string>

class Build
{
 public:
  static std::string getBuild(){return "9/20/2010 18:00";}
  static std::string getVersion(){return getMajorVersion() + "." + getMinorVersion() +"."+getRevision();}
  static std::string getMajorVersion(){return "0";}
  static std::string getMinorVersion(){return "1";}
  static std::string getRevision(){return "1";}
  static std::string todo;
};

#endif
