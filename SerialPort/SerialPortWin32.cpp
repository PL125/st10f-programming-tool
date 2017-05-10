#include <sstream>
#include <cstdio>

#include "ExitCodes.hpp"
#include "SerialPortWin32.hpp"
#include "Logger.hpp"

using std::ostringstream;
using std::istringstream;

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
string
CSerialPortWin32::getLastErrorAsString()
{
    DWORD e = GetLastError();
    if (e == 0)
        return string();

    LPSTR msgBuff = nullptr;
    size_t size = FormatMessageA(
       FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
       NULL,
       e,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPSTR) &msgBuff,
       0,
       NULL);

    string m(msgBuff, size);

    LocalFree(msgBuff);

    return m;
}




CSerialPortWin32::CSerialPortWin32()
{
    mSerialPortH = INVALID_HANDLE_VALUE;

    mMaxBaudrates.push_back(s_speed("User defined", BAUD_USER, 0));
    mMaxBaudrates.push_back(s_speed("75", BAUD_075, 75));
    mMaxBaudrates.push_back(s_speed("110", BAUD_110, 110));
    mMaxBaudrates.push_back(s_speed("134.5", BAUD_134_5, 134));
    mMaxBaudrates.push_back(s_speed("150", BAUD_150, 150));
    mMaxBaudrates.push_back(s_speed("300", BAUD_300, 300));
    mMaxBaudrates.push_back(s_speed("600", BAUD_600, 600));
    mMaxBaudrates.push_back(s_speed("1200", BAUD_1200, 1200));
    mMaxBaudrates.push_back(s_speed("1800", BAUD_1800, 1800));
    mMaxBaudrates.push_back(s_speed("2400", BAUD_2400, 2400));
    mMaxBaudrates.push_back(s_speed("4800", BAUD_4800, 4800));
    mMaxBaudrates.push_back(s_speed("7200", BAUD_7200, 7200));
    mMaxBaudrates.push_back(s_speed("9600", BAUD_9600, 9600));
    mMaxBaudrates.push_back(s_speed("14400", BAUD_14400, 14400));
    mMaxBaudrates.push_back(s_speed("19200", BAUD_19200, 19200));
    mMaxBaudrates.push_back(s_speed("38400", BAUD_38400, 38400));
    mMaxBaudrates.push_back(s_speed("56000", BAUD_56K, 56000));
    mMaxBaudrates.push_back(s_speed("57600", BAUD_57600, 576000));
    mMaxBaudrates.push_back(s_speed("115200", BAUD_115200, 115200));
    mMaxBaudrates.push_back(s_speed("128000", BAUD_128K, 128000));
}

CSerialPortWin32::~CSerialPortWin32()
{
  ;
}

void
CSerialPortWin32::openPort(string portName)
{
    if (mSerialPortH == INVALID_HANDLE_VALUE) {
	// Construct Windows serial port name
	ostringstream sn;
	sn << "\\\\.\\" << portName;
	// Open serial port  
	mSerialPortH = CreateFile(sn.str().c_str(),             // Port name
				  GENERIC_READ | GENERIC_WRITE, // Read/Write
				  0,                            // No Sharing
				  NULL,                         // No Security
				  OPEN_EXISTING,// Open existing port only
				  0,            // Non Overlapped I/O
				  NULL);        // Null for Comm Devices

	if (mSerialPortH == INVALID_HANDLE_VALUE) {
	    CLogger::error("Cannot open port " + portName + ": " + getLastErrorAsString(),
                           EXIT_SERIAL_PORT);
        }
  }
}

void
CSerialPortWin32::open(string portName, string speed)
{
    this->setDefaultTimeout();

    openPort(portName);
    
    DWORD ms = getMaxSpeed(portName).value;
    
    // Konfiguracia
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if (GetCommState(mSerialPortH, &dcbSerialParams) == 0) {
        CLogger::error("Cannot get state for port " + portName + ": " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }
    // Set speed
    istringstream is(speed);
    DWORD n;
    is >> n;
    if (n == 0) {
        // User has requested default baudrate
        n = ((ms == 0) || (19200 <= ms)) ? 19200 : ms;
    } else if ((ms != 0) && (n > ms)) {
        ostringstream os;
        os << "Baudrate " << n << " Bd is higher than max. baudrate " << ms << " Bd supported by " << portName;
        CLogger::error(os.str(), EXIT_SERIAL_PORT);
    }

    dcbSerialParams.BaudRate = n;
    dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
    dcbSerialParams.Parity   = NOPARITY;  // Setting Parity = None
    
    if (SetCommState(mSerialPortH, &dcbSerialParams) == 0) {
        CLogger::error("Cannot set state for port " + portName + ": " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }

    ostringstream os;
    os << n;
    CLogger::info("Serial port " + portName + " opened at speed " + os.str() + " Bd");
}

CSerialPortWin32::s_speed
CSerialPortWin32::getMaxSpeed(string portName)
{
    COMMPROP cp;
    
    if (GetCommProperties(mSerialPortH, &cp) == 0) {
        CLogger::error("Cannot get properties of " + portName + " port: " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }

    // Convert key to baudrate value
    list<s_speed>::const_iterator it;
    
    for (it = mMaxBaudrates.begin(); it != mMaxBaudrates.end(); ++it) {
        if (it->key == cp.dwMaxBaud)
            break;
    }

    return *it;
}

string
CSerialPortWin32::getSpeeds(string portName)
{ 
    openPort(portName);
    
    s_speed m = getMaxSpeed(portName);

    ostringstream os;
    os << "User defined baudrate";
    if (m.key != BAUD_USER)
        os << " up to the " << m.name << " Bd";

    close();
    return os.str();
}

void
CSerialPortWin32::close()
{
    if (mSerialPortH != INVALID_HANDLE_VALUE) {
        if (CloseHandle(mSerialPortH) == 0) {
            CLogger::error("Cannot close serial port: " + getLastErrorAsString(),
                           EXIT_SERIAL_PORT);
        }
        mSerialPortH = INVALID_HANDLE_VALUE;
    }
}

void
CSerialPortWin32::setTimeouts(int ms)
{
    COMMTIMEOUTS timeouts;
    
    if (!GetCommTimeouts(mSerialPortH, &timeouts)) {
        CLogger::error("Cannot get timeouts for serial port: " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }
    
    timeouts.ReadIntervalTimeout = ms; 
    timeouts.ReadTotalTimeoutMultiplier = ms;
    timeouts.ReadTotalTimeoutConstant = 0;
    
    timeouts.WriteTotalTimeoutMultiplier = ms;
    timeouts.WriteTotalTimeoutConstant = 0;
    
    if (!SetCommTimeouts(mSerialPortH, &timeouts)) {
        CLogger::error("Cannot set timeouts for port: " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }
}
    
ssize_t
CSerialPortWin32::writeSingle(uint8_t *data, int data_length)
{   
    DWORD written = 0;

    setTimeouts(mReadTimeoutMs);
    // TODO: Rozlisovat medzi timeoutom a chybou
    if (!WriteFile(mSerialPortH, data, data_length, &written, NULL)) {
        CLogger::error("Cannot write data to the serial port: " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }
    
    return written;
}

ssize_t
CSerialPortWin32::readSingle(uint8_t *data, int data_length)
{
    DWORD read;

    setTimeouts(mReadTimeoutMs);
    
    if (!ReadFile(mSerialPortH, data, data_length, &read, NULL)) {
        CLogger::error("Cannot read data from serial port: " + getLastErrorAsString(),
                       EXIT_SERIAL_PORT);
    }
    
    return read;
}
