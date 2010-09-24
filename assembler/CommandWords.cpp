#include "CommandWords.h"

bool CommandWords::isCommand(const arg_t& aString)
{
    size_t size = this->size();
    for (size_t i = 0; i < size; i++)
        if (this->at(i) == aString)
            return true;
    return false;
}

arg_t CommandWords::showAll() {
    arg_t bean = "";

    size_t size;
    DString * junks = new DString[size = this->size()];
    for(size_t i = 0;i<this->size();i++)
        junks[i] = (*this).at(i);

    DString::alphabetize(junks, size);

    for (int i = 0; i < size; i++) {
        bean += " " + std::string(junks[i].toCharArray());
    }

    delete [] junks;
    junks = 0;
    return bean + " and functions";
}
