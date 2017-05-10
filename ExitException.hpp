#ifndef EXIT_EXCEPTION_HPP
#define EXIT_EXCEPTION_HPP 1

#include <string>
#include <stdexcept>

using std::runtime_error;
using std::string;

class CExitException : public runtime_error
{
private:
    int mReturnValue;    
public:
    CExitException(const string & whatArg, int returnValue);
    int getReturnValue();
};

#endif
