#include "DString.h"


DString::DString()
{
	itsString.assign(std::string());
}

DString::DString(const char * const cString)
{
	itsString.assign(std::string(cString));
}

DString::DString(const std::string& cString)
{
	itsString.assign(cString);
}


DString::DString(const char ch)
{
	itsString.assign(std::string(1,ch));
}

DString::DString (const DString & rhs)
{
	itsString.assign(rhs.itsString);
}

DString::~DString()
{
	1;//nothing to do
}

DString& DString::operator=(const DString& rhs)
{
	if(this == &rhs)
		return *this;
	
	this->itsString.assign(rhs.itsString);
	return *this;
}

char & DString::operator[](unsigned short offset)
{
	if(offset > itsString.size())
		return itsString[itsString.size()-1]; //replace with Exception??
	else
		return itsString[offset];
}

char DString::operator[](unsigned short offset) const
{
	if(offset > itsString.size())
		return itsString[itsString.size()-1]; //replace with Exception??
	else
		return itsString[offset];

}

DString DString::operator+(const DString& rhs) const
{
	
	DString temp;
	temp.itsString.assign(itsString);
	temp.itsString.append(rhs.itsString);
	return temp;
}

DString DString::operator+(const char * string) const
{
	return *this+DString(string);
}

void DString::operator+=(const DString& rhs)
{
	itsString += rhs.itsString;
}

void DString::operator+=(const char ch)
{
	itsString += std::string(1,ch);
}

int DString::operator==(const DString& bean) const
{
	return itsString.compare(bean.itsString) == 0;
}

std::ostream& operator<<(std::ostream& theStream, const DString& bean)
{
	theStream << bean.itsString.c_str();
	return theStream;
}

char DString::charAt(unsigned short i) const
{
	if(i > itsString.size())
		throw "Invalid String location";
	else
		return itsString[i];
}

//Better version comming soon. (Hopefully)
DString DString::replaceFirst(const char * find, const char * replace) const
{
	if(replace == "")
	{
		return remove(find);
	}
	DString newString(itsString.c_str());
	ushort i = 0;
	while(i < newString.length())
	{
		if(newString[i] == find[0])
		{
			newString[i] = replace[0];
			break;
		}
		i++;
	}
	return newString;/**/
}

int DString::toInt()
{
	try
	{
		return (int) itsString[0];
	}
	catch(...)
	{
		throw "DString is not an Float";
	}
}

int * DString::intValueOfCharacters() const
{

  int * returnMe = new int[this->length()];

  try
    {
      for(int i = 0 ; i< this->length();i++)
		returnMe[i] = (int) itsString[i];

      return returnMe;
    }
  catch(...)
    {
      throw "DString is not a float";
    }
}

DString DString::substring(ushort beginning, ushort endExclusive) const
{
	if(endExclusive > itsString.size())
		throw "Invalid String location";
	DString returnMe;
	returnMe.itsString.assign(itsString.substr(beginning,endExclusive-beginning));
	return returnMe;
}

DString DString::substring(ushort beginning) const
{
	DString tmp;
	tmp.itsString.assign(itsString.substr(beginning));
	return tmp;
}

void DString::toLowerCase()
{
	for(int i =0;i<itsString.size();i++)
		itsString[i] = tolower(itsString[i]);
}

void DString::toUpperCase()
{
	for(int i =0;i<itsString.size();i++)
		itsString[i] = toupper(itsString[i]);
}

bool DString::contains(char ch)
{
	return itsString.find_first_of(ch)!=std::string::npos ;
}

int DString::indexOf(char ch) const
{
	return itsString.find_first_of(ch);
}

DString DString::remove(const char * find) const
{
	if(this->indexOf(find[0]) < 0)
		return *this;
	int i = this->indexOf(find[0]);
	DString temp = this->substring(0,i);
	if(i >= this->length())
		return temp;
	temp += this->substring(i+1);
	return temp;
}

DString DString::trim() const
{
  DString returnMe = *this;
  if(returnMe.equals(""))
    return returnMe;

  DString testme(returnMe[0]);

  int i = 0;
  while(testme.equals(" ") && i < returnMe.length())
    {
      testme = returnMe[++i];
    }

  if(!returnMe.substring(returnMe.length()-1).equals(" "))
    return returnMe.substring(i);

  int j = returnMe.length();
  testme = returnMe[returnMe.length() -1];
  while(testme.equals(" ") && j >= 0)
    {
      testme = returnMe[--j];
    }

  return returnMe.substring(i,j+1);
  

}

int DString::numberOfOccurrences(const char ch)
{
  int returnMe = 0;
  for(int i = 0;i<this->length();i++)
    if(this->charAt(i) == ch)
      returnMe++;

  return returnMe;
}

int DString::cmp(DString lhs,DString rhs)
{
  if(lhs == rhs)
    return 0;

  int smaller = lhs.length();

  if(rhs.length() < lhs.length())
    smaller = rhs.length();

  for(int i = 0;i<smaller;i++)
    if(lhs[i] != rhs[i])
      return lhs[i] - rhs[i];
  
  if(smaller == lhs.length())
    return -1;
  else
    return 1;

}

void DString::alphabetize(DString * words, int size)
{

  if(words == 0)
    return;

  int result = 0;

  for(int i = 0;i<size - 1; ++i)
    {
      for(int j = 0;j<size -1 -i;++j)
	{
	  result = cmp(words[j],words[j+1]);
	  
	  if(result > 0)
	    {
	      DString tmp = words[j];
	      words[j] = words[j+1];
	      words[j+1] = tmp;
	    }
	}
    }
}
