#include <sstream>
#include <memory>
#include "ExitCodes.hpp"
#include "UserConfig.hpp"
#include "Logger.hpp"
#include "help_message.hpp"

using std::ostringstream;
using std::istringstream;
using std::endl;
using std::noskipws;


#define OPERATION_SPEEDS   "speeds"
#define OPERATION_ERASE    "erase"
#define OPERATION_READ     "read"
#define OPERATION_WRITE    "write"
#define OPERATION_HELP     "help"
#define OPERATION_VERSION  "version"
#define OPERATION_IDENT    "ident"


// Common options
#define OPTION_SERIAL_PORT     "-p"
#define OPTION_VERBOSE_MODE    "-v"
#define OPTION_SERIAL_SPEED    "-s"
#define OPTION_FREQUENCY       "-f"
#define OPTION_PRINT_PROGRESS  "-g"
// Options specific for an operation
#define OPTION_B            "-b"
#define OPTION_C            "-c"
#define OPTION_E            "-e"
#define OPTION_N            "-n"


CUserConfig::CUserConfig(int argc, char **argv)
{
    mSerialPortName = DEFAULT_SERIAL_PORT_NAME;
    mSerialSpeed = "0"; // Default serial speed for given device
    mVerboseMode = false;
    mSpeeds = false;
    mHelp = false;
    mVersion = false;
    mIdent = false;
    mErase = false;
    mRead = false;
    mReadOutputFilename = "";
    mReadLength = -1;
    mWrite = false;
    mWriteInputFilename = "";
    mWriteEraseWholeMemory = false;
    mWriteCheckByRead = false;
    mMcuFrequency = 0;
    mPrintProgress = false;

    vector<char *> args;

    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);

    parseCommandLine(args);
}

const string
CUserConfig::getHelpMessage(const string & execName) const
{
    return HelpMessage::getText(execName);
}

void
CUserConfig::parseCommandLine(vector<char *> & args)
{
    // Process options common for all operations. Create custom
    // argument vector without processed common options.
    vector<char *>::iterator it = args.begin();
    // Skip operation name from processing
    if (it != args.end())
        ++it;
    // Start at the second argument, first is the name of an
    // operation.
    while (it != args.end()) {
    	string a = *it;
        bool processed = false;
        bool with_argument = false;

    	if (!a.compare(OPTION_SERIAL_PORT)) {
    	    mSerialPortName = getArgument(it, args.end());
            processed = true;
            with_argument = true;
        } else if (!a.compare(OPTION_SERIAL_SPEED)) {
    	    mSerialSpeed = getArgument(it, args.end());
            processed = true;
            with_argument = true;
    	} else if (!a.compare(OPTION_VERBOSE_MODE)) {
    	    mVerboseMode = true;
            processed = true;
        } else if (!a.compare(OPTION_PRINT_PROGRESS)) {
    	    mPrintProgress = true;
            processed = true;
    	} else if (!a.compare(OPTION_FREQUENCY)) {
            istringstream is(getArgument(it, args.end()));
            string s = is.str();
            float f;
            is >> noskipws >> f;
            bool a = (is.fail() || (f <= 0));
            bool b = (is.peek() != EOF);
            if (a || b) {
                ostringstream os;
                os << "Argument for -f option '" << s << "' is not a positive real number";
        	CLogger::error(os.str(), EXIT_USER_CONFIG);
            }
            mMcuFrequency = f;
            processed = true;
            with_argument = true;
    	}
        
        if (processed) {
            it = args.erase(it);
            if (with_argument) // Urcite bude platne, to mame osetrene getArgument()
                it = args.erase(it);
        } else {            
            ++it;
        }
    }
    
    it = args.begin();
    if (it != args.end()) {
        it = args.begin();
        // Resolve operation name
        string a = *it;

        if (!a.compare(OPERATION_SPEEDS)) {
            mSpeeds = true;
        } else if (!a.compare(OPERATION_ERASE)) {
            mErase = true;
            parseEraseArguments(++it, args.end());
        } else if (!a.compare(OPERATION_READ)) {
            mRead = true;
            parseReadArguments(++it, args.end());
        } else if (!a.compare(OPERATION_WRITE)) {
            mWrite = true;
            parseWriteArguments(++it, args.end());
        } else if (!a.compare(OPERATION_HELP)) {
            mHelp = true;
        } else if (!a.compare(OPERATION_VERSION)) {
            mVersion = true;
        } else if (!a.compare(OPERATION_IDENT)) {
            mIdent = true;
        } else {
            CLogger::error("Unknown operation requested: " + a, EXIT_USER_CONFIG);
        }
    }
}

void
CUserConfig::parseEraseArguments(vector<char *>::const_iterator args, 
                                 vector<char *>::const_iterator end)
{
    while (args != end) {
    	string a = *args;
        if (!a.compare(OPTION_B)) {
            // Prepend comma for unified parsing
            istringstream is("," + getArgument(args, end));
            // Parse block numbers list
            bool e = false;
            for (;;) {
        	// Get comma
        	int c = is.get();
        	if (is.eof()) {
        	    break;
        	} else if (c != ',') {
        	    e = true;
        	    break;
        	}
        	// Get number
        	int n;
        	is >> n;
        	if (is.fail()) {
        	    e = true;
        	    break;
        	}
        	mEraseBlockList.push_back(n);
            }
            if (e || mEraseBlockList.size() == 0)
        	CLogger::error("Argument for -b option must be in format N[,N]...", EXIT_USER_CONFIG);
            ++args;
            ++args;
    	} else {
            string s = *args;
            CLogger::error("Unknown argument '" + s + "' for erase operation", EXIT_USER_CONFIG);
        }
    }
}

void
CUserConfig::parseReadArguments(vector<char *>::const_iterator args, 
                                vector<char *>::const_iterator end)
{
    while (args != end) {
    	string a = *args;
        if (!a.compare(OPTION_N)) {
            istringstream n(getArgument(args, end));
            string s = n.str();
            n >> noskipws >> mReadLength;
            bool a = (n.fail() || (mReadLength < 0));
            bool b = (n.peek() != EOF);
            if (a || b) {
                ostringstream os;
                os << "Argument for -n option '" << s << "' is not 0 or a positive number";
        	CLogger::error(os.str(), EXIT_USER_CONFIG);
            }
            ++args;
            ++args;
    	} else {
            // First occurence of this is the name of output file
            if (mReadOutputFilename.length() == 0) {
        	mReadOutputFilename = a;
                ++args;
            } else {
        	string s = *args;
        	CLogger::error("Unknown argument '" + s + "' for read operation", EXIT_USER_CONFIG);
            }
    	}
    }
    // Must have at least filename where to write output
    if (mReadOutputFilename.length() == 0)
        CLogger::error("Missing output filename for read operation", EXIT_USER_CONFIG);
}

void
CUserConfig::parseWriteArguments(vector<char *>::const_iterator args, 
                                 vector<char *>::const_iterator end)
{
    while (args != end) {
    	string a = *args;
        if (!a.compare(OPTION_E)) {
            mWriteEraseWholeMemory = true;
            ++args;
        } else if (!a.compare(OPTION_C)) {
            mWriteCheckByRead = true;
            ++args;
    	} else {
            // First occurence of this is the name of output file
            if (mWriteInputFilename.length() == 0) {
        	mWriteInputFilename = a;
                ++args;
            } else {
        	string s = *args;
        	CLogger::error("Unknown argument '" + s + "' for write operation", EXIT_USER_CONFIG);
            }
    	}
    }
    // Must have at least filename where to write output
    if (mWriteInputFilename.length() == 0)
        CLogger::error("Missing input filename for write operation", EXIT_USER_CONFIG);
}

string
CUserConfig::getArgument(vector<char *>::const_iterator args,
                         vector<char *>::const_iterator end)
{
    if (++args == end) {
        string s(*(--args));
        CLogger::error("Missing argument for option '" + s + "'", EXIT_USER_CONFIG);
    }
    return string(*args);
}

bool
CUserConfig::isEraseSet()
{
    return mErase;
}

list<unsigned int>
CUserConfig::getEraseBlockList()
{
    return mEraseBlockList;
}

bool
CUserConfig::isReadSet()
{
    return mRead;
}

bool
CUserConfig::isWriteSet()
{
    return mWrite;
}

bool
CUserConfig::isVerboseModeSet()
{
    return mVerboseMode;
}

bool
CUserConfig::isPrintProgressSet()
{
    return mPrintProgress;
}

bool
CUserConfig::isHelpSet()
{
    return mHelp;
}

bool
CUserConfig::isVersionSet()
{
    return mVersion;
}

bool
CUserConfig::isIdentSet()
{
    return mIdent;
}

string &
CUserConfig::getSerialPortName()
{
    return mSerialPortName;
}

string &
CUserConfig::getSerialSpeed()
{
    return mSerialSpeed;
}

string &
CUserConfig::getReadOutputFname()
{
    return mReadOutputFilename;
}

string &
CUserConfig::getWriteInputFname()
{
    return mWriteInputFilename;
}

int
CUserConfig::getReadLength()
{
    return mReadLength;
}

bool
CUserConfig::getWriteEraseWholeMemory()
{
    return mWriteEraseWholeMemory;
}

bool
CUserConfig::getWriteCheckByRead()
{
    return mWriteCheckByRead;
}

bool
CUserConfig::isSpeedsSet()
{
    return mSpeeds;
}

float
CUserConfig::getMcuFrequency()
{
    return mMcuFrequency;
}
