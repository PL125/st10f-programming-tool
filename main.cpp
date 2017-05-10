#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  /* strerror() */
#include <stdint.h>

#include <iostream>
#include <csignal>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

#include "ExitCodes.hpp"
#include "Logger.hpp"
#include "ExitException.hpp"
#include "UserConfig.hpp"
#include "SerialPortFactory.hpp"
#include "SerialPort.hpp"
#include "Mcu.hpp"

using std::cout;
using std::endl;
using std::string;
using std::ostringstream;
using std::vector;
using std::unique_ptr;

void opSpeeds(CUserConfig & uc);
void opRead(CUserConfig & uc, CMcu & mcu);
void opErase(CUserConfig & uc, CMcu & mcu);
void opWrite(CUserConfig & uc, CMcu & mcu);

vector<uint8_t> readDataFile(const string fpath);
void writeDataFile(const string fpath, vector<uint8_t> data);

// Global variable to be accessed in signal handler
unique_ptr<CSerialPort> sp;

// Uvolnime zdroje pri ukonceni na signal
void
signalHandler(int dummy)
{
    CLogger::info("Exiting on signal");
    sp->close();
    exit(EXIT_MAIN_SIGNAL);
}

int
main(int argc, char **argv)
{
    // Set signal handler
    signal(SIGINT, signalHandler);

    CSerialPortFactory serialPortFactory;
    sp = serialPortFactory.getSerialPort();
    
    try {
	// Parse user command line configuration
	CUserConfig uc(argc, argv);
	// Configure logging
	if (uc.isVerboseModeSet())
	    CLogger::setLogInfo(true);
	// Execute selected operation
	if (uc.isHelpSet()) {
            cout << uc.getHelpMessage(string(argv[0]));
	} else if (uc.isVersionSet()) {
            cout << PROGRAM_NAME << " " << PROGRAM_VERSION << endl;
	} else if (uc.isSpeedsSet()) {
	    opSpeeds(uc);
	} else if (uc.isIdentSet() || uc.isEraseSet() || uc.isReadSet() || uc.isWriteSet()) {
	    // Open serial port
	    sp->open(uc.getSerialPortName(), uc.getSerialSpeed());
	    // Get MCU model
	    unique_ptr<CMcu> mcu(new CMcu(*sp, uc.getMcuFrequency()));
	    // Execute requested operation
            if (!uc.isIdentSet()) {
                if (uc.isReadSet())
                    opRead(uc, *mcu);
                else if (uc.isEraseSet())
                    opErase(uc, *mcu);
                else if (uc.isWriteSet())
                    opWrite(uc, *mcu);
            } else {
                cout << mcu->ident() << endl;
            }
	} else {
	    CLogger::error("No operation requested. To get help type: " + string(argv[0]) + " help" , EXIT_MAIN_NOOP);
	}
	
	sp->close();
	return 0;
    } catch (CExitException & e) {
	return e.getReturnValue();
    }
}

void
opSpeeds(CUserConfig & uc)
{

    string s = sp->getSpeeds(uc.getSerialPortName());
    cout << "Baudrates supported by serial port " << uc.getSerialPortName() << ":" << endl;
    cout << s;
}

void
opRead(CUserConfig & uc, CMcu & mcu)
{
    CLogger::info("Reading memory");
    
    int rl = uc.getReadLength();
    vector<uint8_t> r;
    
    if (rl == -1)
	r = mcu.read(uc.isPrintProgressSet());
    else
    	r = mcu.read(rl, uc.isPrintProgressSet());    
    // Write data file musi dostat spravnu hodnotu
    writeDataFile(uc.getReadOutputFname(), r);
}

void
opErase(CUserConfig & uc, CMcu & mcu)
{
    if (uc.getEraseBlockList().size() != 0) {
	CLogger::info("Erasing memory by blocks");
	mcu.erase(uc.getEraseBlockList());
    } else {
	CLogger::info("Erasing whole memory");
	mcu.erase();
    }
}

void
opWrite(CUserConfig & uc, CMcu & mcu)
{
    vector<uint8_t> data;

    data = readDataFile(uc.getWriteInputFname());
    
    if (uc.getWriteEraseWholeMemory()) {
	CLogger::info("Erasing whole memory");
    	mcu.erase();
    } else {
	CLogger::info("Erasing memory by blocks");
    	mcu.erase(0, data.size() - 1);
    }

    CLogger::info("Writing memory");
    mcu.write(data, uc.isPrintProgressSet());

    if (uc.getWriteCheckByRead()) {
	CLogger::info("Checking result of write operation by reading");

	vector<uint8_t> rdata =	mcu.read(data.size());
	for (size_t i = 0; i < data.size(); i++) {
	    if (rdata[i] != data[i])
		CLogger::error("Write operation unsucessful", EXIT_MAIN_PROG_VERIFY);
	}

	CLogger::info("Write operation was successful");
    }
}


vector<uint8_t>
readDataFile(const string fpath)
{
    FILE *f;
    size_t size;    
#ifdef UNIX
    char mode[3] = "r";
#else
    char mode[3] = "rb";
#endif
    
    if ((f = fopen(fpath.c_str(), mode)) == NULL)
	CLogger::error("Cannot open file for reading: " + fpath, EXIT_MAIN_FILE_INOUT);

    // Determine file size
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
	
    vector<uint8_t> data;
    data.resize(size);
    
    size_t r = fread(data.data(), sizeof(uint8_t), size, f);
    fclose(f);

    if (r != size)
        CLogger::error("Cannot read from file: " + fpath, EXIT_MAIN_FILE_INOUT);
    
    return data;
}

void
writeDataFile(const string fpath, vector<uint8_t> data)
{
    FILE *out;
#ifdef UNIX
    char mode[3] = "w";
#else
    char mode[3] = "wb";
#endif
    
    if ((out = fopen(fpath.c_str(), mode)) == NULL) {
        ostringstream os;
        os << "Cannot open file for writing: " << fpath << ": " << strerror(errno);
	CLogger::error(os.str(), EXIT_MAIN_FILE_INOUT);
    }

    size_t r = fwrite(data.data(), sizeof(uint8_t), data.size(), out);
    fclose(out);

    if (r != data.size())
	CLogger::error("Cannot write to file: " + fpath, EXIT_MAIN_FILE_INOUT);
}
