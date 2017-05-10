#include "ExitException.hpp"

CExitException::CExitException(const string & whatArg, int returnValue) :
    runtime_error(whatArg), mReturnValue(returnValue)
{
    ;
}

int
CExitException::getReturnValue()
{
    return mReturnValue;
}
