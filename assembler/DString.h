#ifndef DSTRING_CPP
#define DSTRING_CPP

#include <iostream>

#ifndef ushort
#include <cstdlib>
#define ushort unsigned short
#endif

#include <string.h>


/**
 * My own String Handler.
 * The string is saved as a std::string. This class was created for mostly academic reasons on my part. It really should not be used as a replacement for std::string.
 *
 * Created By David Coss, 2010
 * Copying and distributing are permitted under the terms of the GNU Public License version 3(see COPYING or http://www.gnu.org/licenses/gpl.txt).
 */
class DString
{
	public:
		
		/**
		 * Main constructor.
		 * A DString is created based on the provided std::string.
		 * @param cString std::string to be used.
		 */
		DString(const std::string& cstring);

		/**
		 * Alternate constructor for const char* 
		 */
		DString(const char * const cString);
        
		/**
		 * char constructor.
		 * This creates a DString from one character.
		 * @param ch
		 */
		DString(const char ch);

		/**
		 * Default constructor.
		 * Creates a "null" string.
		 */
		DString();

		/**
		 * Copy constructor.
		 * Creates a copy of the given string.
		 * @param rhs DString to be copied.
		 */
		DString(const DString & rhs);

		/**
		 * Destructor.
		 */
		~DString();
		
		//operators
		char & operator[](unsigned short offset);
		char operator[](unsigned short offset) const;
		DString operator+(const DString&) const;///< Note: Addition is the same as concatenation
		DString operator+(const char *) const;///< Note: Addition is the same as concatenation
		void operator+=(const DString&);///< Note: Addition is the same as concatenation
		void operator+=(const char);///< Note: Addition is the same as concatenation
		DString & operator=(const DString&);
		int operator==(const DString&) const;
		int operator!=(const DString& bean) const{return !(*this == bean);}
		int operator==(const char * bob){return *this == DString(bob);}
		int operator!=(const char * bob){return *this != DString(bob);}
		operator std::string()const{return getString();}
		friend std::ostream& operator<<( std::ostream&, const DString&);
		
		DString concat(const DString& bean){ DString tmp;tmp.itsString.append(bean.itsString);return tmp; }///< Note: Adds the new string to the right.

                /**
		 * Gives the length of the string.
		 * @return unsigned short
		 */
		unsigned short length() const { return itsString.size(); }

		/**
		 * Gives the length of the string.
		 */
		unsigned short size() const { return itsString.length(); }

		/**
		 * Gives the string as a std::string object.
		 * @see toCharArray()
		 */
		std::string getString() const { return itsString; }

		/**
		 * Gives the const char* equivalent of the string.
		 * Note: this is a constant pointer, cannot be changed (to be safe).
		 * @return const char*
		 * @see getString();
		 */
		const char * toCharArray() const { return itsString.c_str(); }

		/**
		 * Gives the character at the provided index (zero index).
		 * Note: if the index is not within the length of the string,
		 * an Exception is thrown.
		 * @returns char.
		 * @throws Exception.
		 */
		char charAt(unsigned short) const;

		/**
		 * Returns the result of replacing the first occurence of oldChar with newChar.
		 * If the new character is empty (ie ""), then the first occurance is removed.
		 * If the String does not contain the oldChar, nothing happens and the original string
		 * is returned.
		 * @param oldChar old character.
		 * @param newChar new character.
		 * @return DString.
		 * @see remove(const char * find)
		 */
		DString replaceFirst(const char * oldChar, const char * newChar) const;

		/**
		 * Determins whether or not the provided string equals the other
		 * @return int 0 = false, 1 = true.
		 */

		int equals (const DString& bean) const { return (*this == bean); }
		
		/**
		 * @see equals(const DString& bean)
		 */
		int equals (const char * bean) const { return (*this == DString(bean));}
		int toInt();///< Gives the ASCII int value of the first character.

		int * intValueOfCharacters() const;///< Gives the ASCII int value of all of the characters as an int array.

		/**
		 * Gives the substring starting at the provided index, zero index.
		 * Note: if the index is not within the length of the string,
		 * an Exception is thrown.
		 * @return DString.
		 * @throws Exception.
		 */
		DString substring(ushort) const;

		/**
		 * Gives the substring starting at the first index and ending at the second index (exclusive), zero index.
		 * Note: if the indices is not within the length of the string,
		 * an Exception is thrown.
		 * @return DString.
		 * @throws Exception.
		 */
		DString substring(ushort,ushort) const ;

		/**
		 * Removes any leading or trailing spaces.
		 * Note: does not edit the "parent" string. Instead it 
		 * returns the newly trimmed string.
		 */
		DString trim() const;
		
		 

		/**
		 * Converts the string to all lower case letters.
		 */
		void toLowerCase();
		
		/**
		 * Converts the string to all capitol letters.
		 */
		void toUpperCase();

		/** 
		 * Determines whether the string contains the given character.
		 * @param ch.
		 * @return bool.
		 */
		bool contains(char ch);

		/**
		 * Gives the number of times the character is in the string.
		 *
		 * @param ch
		 * @return int
		 */
		int numberOfOccurrences(const char ch);


		/**
		 * Gives the index of the first occurence of the given character.
		 * If the character is not in the string, -1 is returned.
		 * @param char
		 * @retur int
		 */
		int indexOf(char ch) const;
		
		/**
		 * Gives the index of the first occurence of the given character.
		 * If the character is not in the string, -1 is returned.
		 * Note: if the DString is more than one character, only the first character is used.
		 * @param DString
		 * @retur int
		 */
		int indexOf(DString& ch) const {return indexOf(ch.charAt(0));}
		
		/**
		 * Gives the index of the first occurence of the given character.
		 * If the character is not in the string, -1 is returned.
		 * Note: if the string is more than one character, only the first character is used.
		 * @param char *
		 * @retur int
		 */int indexOf(const char * ch) const {return indexOf(ch[0]);}

		/**
		 * Searches and removes the first occurence of the character, if it is found.
		 * The result is returned. The original object is unchanged.
		 * @param find const char* to be found.
		 * @return DString.
		 */
		DString remove(const char * find) const;

		/**
		 * Takes an array of DStrings and alphabetizes them.
		 * Note: the array given is returned. So if you don't want a list altered to pass it as an arguement; copy it!
		 *
		 * @param words DString*
		 * @param size int size of array.
		 */
		static void alphabetize(DString * words,int size);
		
		/**
		 * Goes through the string character by character. As soon as two corresponding characters are different, a positive number is given if the lhs character is greater, negative if the rhs character is greater and zero if the two string are equal.
		 * see also strcmp in c++ stl.
		 *
		 * @param lhs DString 
		 * @param rhs DString
		 * @return int;
		 */
		static int cmp(DString lhs,DString rhs);

	private:
		std::string itsString;	

};



#endif
