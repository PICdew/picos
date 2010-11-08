#include "good_exceptions.h"

std::ostream& operator<<(std::ostream& theStream,GoodException& ge)
{
    theStream << ge.identifier() << "(" << ge.getErrType() << "): " << ge.what() << std::endl;
    return theStream;
}

