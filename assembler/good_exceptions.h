/* 
 * Various exceptions (hopefully not) thrown by mmpbsa classes.
 *
 * Created by David Coss <David.Coss@stjude.org> 2010
 */
#include <stdexcept>
#include <sstream>

#ifndef GOOD_EXCEPTIONS_H
#define	GOOD_EXCEPTIONS_H

//GoodErrorTypes could be used as return values. Therefore, zero is not used.
enum GoodErrorTypes {UNKNOWN_ERROR = 1, /*<Avoid, as this is vague.*/
    FILE_IO_ERROR,/*<IO problem reading file.*/
    BROKEN_PRMTOP_FILE,/*<prmtop file is improperly formatted or missing data.*/
    BROKEN_TRAJECTORY_FILE,/*<trajectory file is improperly formatted or missing data.*/
    INVALID_PRMTOP_DATA,/*<Data which was loaded into an array is incorrect based on what is expected.*/
    DATA_FORMAT_ERROR,/*<Use this error, when data *within* the program no long matches what it should due to formatting problems.*/
    INVALID_ARRAY_SIZE,
		     UNEXPECTED_EOF,
		     COMMAND_LINE_ERROR,/*<Program supplied an invalid argument in the command line.*/
		     UNDECLARED_VARIABLE,/*<Doing an operation on a varible that was not declared.*/
    BAD_XML_TAG
    };

class GoodException : public std::runtime_error
{
    public:
        /**
         * Very General GoodException. Use more specific exception if one is
         * available.
         *
         * @param error
         */
    GoodException( const std::string& error) : runtime_error(error) {errorType = UNKNOWN_ERROR;}

    /**
     * Creates an exception, with a specified error type.
     *
     * @param error
     * @param errorType
     */
    GoodException(const std::string& error, const GoodErrorTypes& errorType) : runtime_error(error){this->errorType = errorType;}
    GoodException(const std::ostringstream& error, const GoodErrorTypes& errorType) : runtime_error(error.str()){this->errorType = errorType;}
    /**
     * Returns the error type, corresponding to the error types listed below.
     * These should be returned if the exception is caught and the program
     * dies gracefully.
     *
     * @return int error type.
     */
    const GoodErrorTypes& getErrType()const{return errorType;}

    virtual const char* identifier(){return "General MMPBSA Error";}

private:
    GoodErrorTypes errorType;

};


std::ostream& operator<<(std::ostream& theStream,GoodException& me);

#endif	/* GOOD_EXCEPTIONS_H */

