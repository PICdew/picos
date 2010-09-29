/**
 * Provides functions to compile subroutines.
 */
const int opcodeVersion[] = {1,1,0};
Help assemblerHelp()
{
  cerr << "assemberhelp()" << endl;
  Help returnMe;
  returnMe["lda"] = "loads the value to the accumulator";
    returnMe["adda"] = "";
    returnMe["suba"] = "";
    returnMe["movaf"] = "";
    returnMe["pusha"] = "";
    returnMe["anda"] = "";
    returnMe["ora"] = "";
    returnMe["xora"] ="";
    returnMe["rra"] = "";
    returnMe["rla"] = "";
    returnMe["sleep"] = "";
    returnMe["sett"] = "";
	returnMe["setd"] = "";
    returnMe["seta"] = "";
    return returnMe;
}

map<arg_t,int> assemblerTable()
{
    map<arg_t,int> returnMe;
    int opcode = 0;
    returnMe["lda"] = opcode++;
    	returnMe["adda"] = opcode++;
    	returnMe["suba"] = opcode++;
    	returnMe["movaf"] = opcode++;
    	returnMe["pusha"] = opcode++;		
    	returnMe["anda"] = opcode++;		
    	returnMe["ora"] = opcode++;		
    	returnMe["Xora"] = opcode++;
    	returnMe["rra"] = opcode++;
    	returnMe["rla"] = opcode++;
    	returnMe["sleep"] = opcode++;
    	returnMe["inca"] = opcode++;
    	returnMe["deca"] = opcode++;
    	returnMe["bsa"] = opcode++;
    	returnMe["bca"] = opcode++;
    	returnMe["clra"] = opcode++;
    	returnMe["sett"] = opcode++;
    	returnMe["setd"] = opcode++;
    	returnMe["seta"] = opcode++;
    return returnMe;
}


