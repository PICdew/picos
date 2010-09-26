#include "Help.h"

Help::Help() {
    (*this)["about"] = "Usage: about x\nDescribes x.";
    (*this)["list"] = "Usage: list\nLists all variables defined.";
    (*this)["define"] = "Usage: defines x y\n Defines x to be the expression y.";
    (*this)["det"] = "Usage: det A\nGives the determinate of A.";
    (*this)["load"] = "Usage: load x <filename>\nLoads the saved Variable as x.";
    (*this)["print"] = "Usage: print x\nPrints x.";
    (*this)["save"] = "Usage: save <filename>\nSaves the previous commands as <filename>.";
    (*this)["remove"] = "Usage: remove x\nRemoves x.";
    (*this)["edit"] = "Usage: edit A m n a,b,...\nEdits the A matrix of size mxn as elements a,b,c,..";
    (*this)["help"] = "Usage: help <topic>\nOffers help on the topic";
    (*this)["last"] = "Usage: last\nPrints the last,  expression used. The last expression is represented by the variable $last$";
    (*this)["dir"] = "Usage: dir [directory]\nlists the contents of the directory.";
    (*this)["quit"] = "Usage: quit\nExits linal.";
    (*this)["exit"] = (*this)["quit"];
    (*this)["physics"] = "Usage: physics <equation> parameter\nGives the output of <equations with the give parameters.";
    (*this)["import"] = "Usage: import\nimports constants.";
    (*this)["set"] = "Usage: set parameter <value>\nSets the parameter to the value <value>.";
    (*this)["run"] = "Usage: run scriptname\n Runs the script of commands which must end with a semicolon.";
    (*this)["functions"] = "List of Functions: ";
    (*this)["history"] = "Lists the past commands.";
}

std::string Help::getHelp(const arg_t& keyWord) {
    if (this->find(keyWord) != this->end()) {
        return (*this)[keyWord];
    } else {
        return "No help for " + keyWord + ".";
    }
}

