#include "Logger.hpp"
#include "ExitException.hpp"
#include <sstream>

using std::endl;
using std::cout;
using std::cerr;
using std::flush;
using std::hex;
using std::uppercase;
using std::ostringstream;

bool CLogger::mLogInfo = false;
int CLogger::mProgressLastPercent = -1;

void
CLogger::error(const string & msg, int returnValue)
{
    cerr << "ERROR:  " << msg << endl;
    throw CExitException(msg, returnValue);
}

void
CLogger::warning(const string & msg)
{
    cerr << "WARNING:  " << msg << endl; 
}

void
CLogger::info(const string & msg)
{
    if (!CLogger::mLogInfo)
 	return;
    cout << "INFO:  " << msg << endl; 
}

void
CLogger::progress(uint32_t b, uint32_t n)
{
    if (b > n)
        b = n;

    double d = n;
    int p = (int) (b / (d / 100.0));
    if ((p == 100) && (b != n))
        p = 99;
    else if (p > 100)
        p = 100;

    if (((p == 0) && (p != mProgressLastPercent))
        || ((p == 100) && (p != mProgressLastPercent))
        || ((p - mProgressLastPercent) >= LOGGER_PROGRESS_UNIT)) {
        cout << p << "% (" << b << " B / " << n << " B)" << endl;
        mProgressLastPercent = p;
    }
    
    if (p == 100)
        mProgressLastPercent = -1;
}

void
CLogger::setLogInfo(bool b)
{
    mLogInfo = b;
}

string
CLogger::decToHex(int dec)
{
    ostringstream os;
    os << "0x" << hex << uppercase << dec;
    return os.str();
}
