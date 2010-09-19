#include "Command.h"

namespace linal{
	/**
	* Creates a new Command with the specified words. 
	*
	* @param words String command supplied.
	*/
	Command::Command(const DString& _words)
	{
	  args = new utils::DArray<DString>;
	  commandWord = new DString("");
	  secondWord = new DString("");
	  this->words = new DString(_words.getString());
	  //*(this->words) = words;
	  
	  DString test("");
	  bool emptyCommand = words->equals(test);
	  if(!emptyCommand)
	    {
	      utils::StringTokenizer * toke = new utils::StringTokenizer(_words);
	      	while(toke->hasMoreTokens())
			{
			  DString currString = toke->nextToken();
			  args->put(currString);
			}
	      *commandWord = (DString) args->get(0);
			if(args->size() >= 2)
				*secondWord = (DString) args->get(1);
			delete toke;
			toke = 0;
	    }
	  else
	    {
	      *words += "help me";
	      *commandWord += "help";
	    }

	    
	}
  
  DString Command::getCommandWord()
  {
    return *commandWord;
  }

    /**
     * Returns the second word of this command.
     */
	DString Command::getSecondWord()
    {
		return (secondWord == 0) ? DString("") : *secondWord;
    }

    /**
     * Return true if this command was not a known command.
     */
	bool Command::isUnknown()
    {
      return !(new CommandWords())->isCommand(*commandWord);
    }

    /**
     * Determines whether or not a command has a second word, which is usually 
     * the expression or parameter of the command
     */
	bool Command::hasSecondWord()
    {
        return (secondWord != 0);
    }
    
    /**
     * Returns an ArrayList of the words of the supplied command.
     */
	DArray<DString>& Command::getWords() const
    {
    	return *args;
    }
    
    /**
     * Lists the variables stored in the LinAl object
     * 
     * @param la LinAl Stores variables
     */
	DString Command::list(LinAl& la)
    {
		if(la.numberOfVariables() == 0)
		{
			DString returnMe("There are no variables defined.");
			return returnMe;
		}
		return DString(la.printVariables());
    }

    /**
     * Defines a variable in the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
  LinAl& Command::define(const linal::Command& command,LinAl& la)
  {
		DArray<DString> args = command.getWords();
		if(args.size() >= 3)
		{
			DString name = args.get(1);
			DString expr = args.get(2);
			bool resolveValue = false;
			if(args.size() >= 4)
			  {
			    DString arg3 = args.get(3);
			    arg3.toLowerCase();
			    resolveValue =  arg3.equals("--resolve");
			  }

			if(expr.equals("ans"))
			  {
			    expr = la.getANS().toDString();
			  }

			LTree tree = AlPars::infixLTree(expr,la);

			if(tree.getItem().isFunction())
			  {
			    using utils::DArray;
			    
			    DString * functionName = new DString;
			    DArray<DArray<LTree> > junks = Command::prepareForFunctions(tree.getItem().getFunction(), functionName,la);
			    
			    LTree * recentResult = utils::Functions::doFunction(*functionName,junks,la);
			    if(recentResult == 0)
			      recentResult = new LTree(tree);
			    tree = AlPars::simplify(*recentResult,la);
			    delete recentResult;
			    recentResult = 0;
			  }

			try{
			  DString treeString = tree.toDString();

			  if(tree.getItem().isMatrix())
			    {
			      Matrix m = la.getMatrix(DString("matrix")+Double((double) la.getMatrixID()).toDString());
			      la.addMatrix(name,m);
			    }
			  else if(treeString.length() >= 6 && treeString.substring(0,6).equals("matrix"))
			    {
			      if(la.containsVariable(treeString))
				{
				  Matrix m = la.getMatrix(treeString);
				  la.addMatrix(name,m);
				}
			    }
			  else if(resolveValue && treeString.contains('{') && treeString.contains('}'))
			    {
			      std::cout << "resolving" << std::endl;
			      if(!treeString.contains(','))
				throw DavidException("Matrix element require a row and column.",DavidException::FORMAT_ERROR_CODE);
			      
			      DString matrixName = treeString.substring(0,treeString.indexOf('{'));
			      if(!la.containsVariable(name))
				{
				  la.addVariable(name,tree);
				  return la;
				}
			      
			      LTree variable = la.getVariable(matrixName);
			      
			      if(!variable.getItem().isMatrix())
				{
				  la.addVariable(name,tree);
				  return la;
				}
			      
			      Matrix m = la.getMatrix(variable.toDString());
			      
			      int x,y;
			      x = y = -1;
			      
			      DString X = treeString.substring(treeString.indexOf('{')+1,treeString.indexOf(','));
			      DString Y = treeString.substring(treeString.indexOf(',')+1,treeString.indexOf('}'));
			      
			      x = (int) Double(X).doubleValue();
			      y = (int) Double(Y).doubleValue();
			      
			      LTree addMe = m.getElement(x,y);
			      
			      la.addVariable(name,addMe);
			      
			    }
			  else
			    {
			      la.addVariable(name,tree);
			    }
			}
			catch(DavidException de){
			  de.stdOut();
			}
			la.setANS(tree);
		}
		else
		  {
		    std::cout << ("Too few arguments") << std::endl;
		  }
		return la;
  }

    
    /**
     * Loads a file as a variable in the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
  utils::DArray<DString> * Command::load(DString fileName)
    {

      utils::DArray<DString> * returnMe = new utils::DArray<DString>;

      using namespace std;
      ifstream file (fileName.toCharArray());
      int buffersize = 200;
      char str[buffersize];
      if (file.is_open())
	{
	  while(file.getline(str,buffersize))
	    {
	      returnMe->put(str);
	    }
	}
      else
	throw DavidException(DString("Could not open: ") + fileName);


      return returnMe;
      
		
    }/**/

  
  DString Command::print(const linal::Command& command,LinAl& la)
  {
    using linal::AlPars;
    
    DArray<DString> args = command.getWords();
    DString returnMe;
    DString expression = args.get(0);

    if(args.size() > 1)
      expression = args.get(1);

    LTree result;
    
    if(expression == "ans")
      expression = la.getANS().toDString();


    result = AlPars::infixLTree(expression,la);

    if(la.containsVariable(result.toDString()))
      result = la.getVariable(result.toDString());
    

    result = AlPars::simplify(result,la); 

    if(result.getItem().isFunction())
      {
	using utils::DArray;
	
	DString * functionName = new DString;
	DArray<DArray<LTree> > junks = Command::prepareForFunctions(result.getItem().getFunction(), functionName,la);
	
	LTree * recentResult = utils::Functions::doFunction(*functionName,junks,la);
	if(recentResult == 0)
	  recentResult = new LTree(result);
	result = AlPars::simplify(*recentResult,la);
	delete recentResult;
	recentResult = 0;
      }


    if(result.getItem().isMatrix())
      {
	Matrix m = la.getMatrix(result.toDString());
	m = AlPars::matrixSimplify(m,la);
	if(m.numberOfRows() == m.numberOfColumns() && m.numberOfRows() == 1)
	  returnMe = m.toDString();
	else
	  returnMe = DString("matrix[")+m.toDString()+"]";
      }
    else
      returnMe = result.toDString();

    la.setANS(result);

    return returnMe;
  }

  
  /**
   * Defines a variable in the LinAl object
   *
   * @param command Command Command to be Processed
   * @param la LinAl Stores variables
   */
  LinAl& Command::save(const Command& command, DString& fileName, utils::DArray<DString> * output,LinAl& la)
  {
    using namespace std;
    // verify that the file length is correct (it wasn't under Win95)
    ofstream myfile(fileName.toCharArray(), ios::out);
    DEBUG_PRINT(fileName);
    
    if(myfile.is_open())
      {
	
	for(int i = 0;i<output->size();i++)
	  myfile << output->get(i) << endl;
	myfile.close();
      }
    else
      {
	DEBUG_PRINT(DString("Could not open: ") + fileName);
      }
    return la;
  }

    /**
     * Removes a variable from the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
	LinAl& Command::remove(const linal::Command& command,LinAl& la)
    {
		/*if(command.args != 0)
			delete command.args;
		else
			command.args = new DArray<DString>;
		*(command.args) = command.getWords(); /* I'm not quite sure why this is here. */
        if(command.args->size() >= 2)
        {
			if(((DString) command.args->get(1)).equals("all"))
			{
				la.removeAll();
			}
			else
			{
			  la.removeMatrix(command.args->get(1));
			  la.removeVariable(command.args->get(1));
			}
        }
	    return la;
    }

    
	/**
     * Edits a matrix in the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
	/*
    static LinAl& edit(Command& command,LinAl& la)
    {
		DArray args = command.getWords();
        if(args.size() >= 4)
            {
            String name = (String) args.get(1);
            int m = AlPars.toInteger((String) args.get(2));
            int n = AlPars.toInteger((String) args.get(3));
            String elements = (String) args.get(4);
            StringTokenizer tokie = new StringTokenizer(elements,",");
            int product = m*n;
            LTree[][] matrix = new LTree[m][n];
	    
	    try{

            while(tokie.cppasMoreTokens())
                {
                	for(int i=0;i<m;i++)
                	{
                		for(int j=0;j<n;j++)
                		{
				    matrix[i][j] = new LTree(tokie.nextToken());
                		}
                	}
                
                }
	    }
	    catch(NoSuchElementException e)
		{
		    David.say("No such element error: check size.");
		}
            Matrix m2 = new Matrix(m,n,matrix);
            la.removeVariable(name);
            try{
                la.addVariable(name,new LTree(m2));
            }
            catch(DavidException de){
                de.stdout();
            }
	    la.ans = (LTree) la.getVariable(name);
            }
		return la;
    }/**/
    
    /**
     * Imports variables stored in the LinAl object
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     *//*
    public static LinAl importVariables(Command command,LinAl la)
    {
	Set constants = linal.physics.Constants.listConstants();
	DIterator it = constants.iterator();
	while(it.cppasNext())
	    {
		String constant = (String) it.next();
		try{
                    la.addVariable(constant,new LTree(linal.physics.Constants.getConstant(constant)));
                }
                catch(DavidException de){
                    de.stdout();
                }
	    }
	return la;

    }/**/

    /**
     * List the files in a directory
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     */
	/*
    public static String dir(Command command,LinAl la)
    {
	ArrayList args = command.getWords();
	String returnMe = "";
		if(args.size() >= 2)
		    {
			
			String[] files = David.dir((String) args.get(1));
			if(files != null)
			    {
				for(int i=0;i<files.length;i++)
				    {
					returnMe += (files[i]+" ");
				    }
				returnMe += ("\n");
				    }
			else{
			    return "No such directory.";
			}
		    }
                else if(args.size() <2 && args.size() >=1)
                {
                    String[] files = David.dir(".");
			if(files != null)
			    {
				for(int i=0;i<files.length;i++)
				    {
					returnMe += (files[i]+" ");
				    }
				returnMe += ("\n");
				    }
			else{
			    return "No such directory";
			}
                }
		return returnMe;
    }/**/
    
    /**
     * Carries out physics formulas
     *
     * @param command Command Command to be Processed
     * @param la LinAl Stores variables
     *//*
    public static Object[] physics(Command command,LinAl la)
    {
            ArrayList args = command.getWords();
	    String returnString = "";
            String strUnits = la.getParameter("units");
	    LTree returnTree = new LTree();
            if(args.size() >= 3)
            {
                String function = (String) args.get(1);
                String param = (String) args.get(2);
                if(function.equals("weight"))
                {
                    if(strUnits.equals("SI"))
                    {
                        returnTree = linal.physics.Formulas.weight(new Variable(param));
		        strUnits = "N";
                    }
                    else
                    {
                        returnTree = linal.physics.Formulas.weight(new Variable(param), new Variable("32"));
		        strUnits = "lb";
                    }
                    la.ans = returnTree;
                    
		}
                else if(function.toLowerCase().equals("force"))
                {
                    if(args.size() < 4)
                        throw DavidException("force requires mass and acceleration",DavidException.FORMAT_ERROR);
                     if(strUnits.equals("SI"))
                    {
                        returnTree = linal.physics.Formulas.weight(new Variable(param),new Variable((String) args.get(3)));
		        strUnits = "N";
                    }
                    else
                    {
                        returnTree = linal.physics.Formulas.weight(new Variable(param), new Variable("32"));
		        strUnits = "lb";
                    }
                    la.ans = returnTree;
                }
                else
                {
                    returnString = "Equation "+function+"not defined in linal.Physics.";
                }
            }
            else
            {
               return new Object[]{"Equations in linal.Physics: weight, force",la};
            }
	    if(!AlPars.calculateable(returnTree,la))
		{
		    returnString = ((String) args.get(1)+"="+returnTree.toString(la)+strUnits);
		}
	    else
		{
		    try
			{
			    returnString = ((String) args.get(1)+"="+AlPars.doIt(returnTree,la)+strUnits);
			}
		    catch(DavidException de)
			{
			    de.stdout();
			}
		}
	    return  new Object[]{returnString,la};
    }/**/
    
    /**
     * Sets a parameter of the program
     *
     * @param command Command to be Processed
     * @param la LinAl Stores variables
     */
	DString Command::set(const linal::Command& command,LinAl& la)
    {
        DArray<DString> args = command.getWords();
        if(args.size() < 2)
        {
			linal::Help h;
			return h.getHelp("set");
        }
        if(args.size() == 2)
        {
	  DString arg1 = args.get(1);
	  arg1.toLowerCase();
			if(arg1 == "binary")
			{
				bool oldBin = la.useBinary();
				if(oldBin)
					la.clearBinary();
				else
					la.setBinary();
				return (!oldBin) ? DString("Binary mode has been set") : DString("Binary mode has been cleared");
			}
            try
            {
                return la.getParameter(args.get(1));
            }
            catch(DavidException de)
            {
				if(de.getCode() == (DavidException::HASHMAP_ERROR_CODE))
                {
                    return "Parameter Not Found";
                }
            }
        }
        try
        {
	  DString returnMe = DString("Parameter changed. Old value of ")+args.get(1)+ " is ";
	  int oldy = la.setParameter(args.get(1),args.get(2));
	  returnMe += Double((double) oldy).toDString();
	  return returnMe;
        }
        catch(DavidException de)
        {
            return de.getMessage();
        }

	
    }
    
    /**
     * Runs and script of commands which need to end with a semicolon.
     * 
     * @param command Command to be used
     * @param la Linal Stores variables
     *//*
	LinAl& open(const Command& command, DString& fileName, DString& output,LinAl& la)
	{
		using namespace std;
		std::ifstream file(fileName);
		DEBUG_PRINT(DString("Opening")+fileName);

		char * text = new char[file.Length()];
		file.Read(text,file.Length());
		file.Close();
		DString input(text);
		delete text;
		text = 0;

  StringTokenizer tokie(script,"#",false);
  
  while(tokie.hasMoreTokens())
    {
      DString curr = tokie.nextToken();
      while(curr.indexOf("\n") > -1)
	{
	  if(curr.indexOf("\n") == 0)
	    curr = curr.substring(1,curr.length());
	  else if(curr.indexOf("\n") == (curr.length() - 1 ))
	    curr = curr.substring(0,curr.indexOf("\n"));
	  else
	    curr = curr.substring(0,curr.indexOf("\n"))+curr.substring(curr.indexOf("\n")+1,curr.length());
	}
      txtInput->SetValue(curr.getString());
      wxCommandEvent event;
      OntxtInputEnter(event);
    
        
    }/**/




  utils::DArray<utils::DArray<linal::LTree> > Command::prepareForFunctions(utils::DArray<utils::DArray<DString> > junks, DString * functionName,LinAl& la)
  {

    using utils::DArray;
    using linal::LTree;

    DEBUG_PRINT("Preparing function");

    if(functionName == 0)
      functionName = new DString;

    int counter = 0;

    *functionName = junks.get(counter).get(0);
    counter++;

    DArray<DArray<LTree> > returnMe;

    for(counter;counter<junks.size();counter++)
      {
	DArray<LTree> currArray;
	for(int i = 0;i<junks.get(counter).size();i++)
	  {
	    LTree curr = AlPars::infixLTree(junks.get(counter).get(i),la);
	    curr = AlPars::simplify(curr,la);
	    DEBUG_PRINT(DString("currstring: ")+curr.toDString());
	    currArray.put(curr);
	  }
	returnMe.put(currArray);
      }

    DEBUG_PRINT("function prepared");
	
    return returnMe;

  }

}//end namespace linal

