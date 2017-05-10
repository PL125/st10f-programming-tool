#include <stdio.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h>
#include <sys/select.h>
#include <errno.h>

#include <sstream>
#include <iostream>
#include <iomanip>

using std::ostringstream;
using std::endl;
using std::setw;
using std::right;

#include "ExitCodes.hpp"
#include "SerialPortUnix.hpp"
#include "Logger.hpp"
#include "ExitException.hpp"

#define DEFAULT_SERIAL_SPEED "19200"

CSerialPortUnix::CSerialPortUnix()
{
    mSerialPortFd = -1;
    mPortName = "";
    // Construct vector of available system baudrates
#ifdef B50
    mBaudrates.push_back(pair<string, speed_t>("50", B50));
#endif
#ifdef B75
    mBaudrates.push_back(pair<string, speed_t>("75", B75));
#endif
#ifdef B110
    mBaudrates.push_back(pair<string, speed_t>("110", B110));
#endif
#ifdef B134
    mBaudrates.push_back(pair<string, speed_t>("134", B134));
#endif
#ifdef B150
    mBaudrates.push_back(pair<string, speed_t>("150", B150));
#endif
#ifdef B200
    mBaudrates.push_back(pair<string, speed_t>("200", B200));
#endif
#ifdef B300
    mBaudrates.push_back(pair<string, speed_t>("300", B300));
#endif
#ifdef B600
    mBaudrates.push_back(pair<string, speed_t>("600", B600));
#endif
#ifdef B1200
    mBaudrates.push_back(pair<string, speed_t>("1200", B1200));
#endif
#ifdef B1800
    mBaudrates.push_back(pair<string, speed_t>("1800", B1800));
#endif
#ifdef B2400
    mBaudrates.push_back(pair<string, speed_t>("2400", B2400));
#endif
#ifdef B4800
    mBaudrates.push_back(pair<string, speed_t>("4800", B4800));
#endif
#ifdef B9600
    mBaudrates.push_back(pair<string, speed_t>("9600", B9600));
#endif
#ifdef B19200
    mBaudrates.push_back(pair<string, speed_t>("19200", B19200));
#endif
#ifdef B38400
    mBaudrates.push_back(pair<string, speed_t>("38400", B38400));
#endif
#ifdef B57600
    mBaudrates.push_back(pair<string, speed_t>("57600", B57600));
#endif
#ifdef B115200
    mBaudrates.push_back(pair<string, speed_t>("115200", B115200));
#endif
#ifdef B230400
    mBaudrates.push_back(pair<string, speed_t>("230400", B230400));
#endif
}

CSerialPortUnix::~CSerialPortUnix()
{
    ;
}

void
CSerialPortUnix::openPort(string portName)
{
    if (mSerialPortFd == -1) {
	mSerialPortFd = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (mSerialPortFd == -1)
	    CLogger::error("Cannot open serial port " + portName, EXIT_SERIAL_PORT);
	mPortName = portName;
    }
}

void
CSerialPortUnix::open(string portName, string speed)
{
    this->setDefaultTimeout();

    openPort(portName);
    // Set parameters of serial communication
    pair<string, speed_t> s = findSpeed(speed, getDeviceSpeeds());
    setSpeed(s);
    
    struct termios options;
    
    tcgetattr(mSerialPortFd, &options);
    // Control options
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    // Turn off hardware flow controll
    options.c_cflag &= ~CRTSCTS;
    // Input Options - Turn off software flow controll, and CR <-> LF mapping
    options.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL);
    // Line options - Raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // Raw output
    options.c_oflag &= ~OPOST;
    tcsetattr(mSerialPortFd, TCSANOW, &options);
    // Blocking read
    fcntl(mSerialPortFd, F_SETFL, 0);
    
    CLogger::info("Serial port " + mPortName + " opened at speed " + s.first + " Bd");
}

void
CSerialPortUnix::close()
{
    // Close only valid descriptor
    if (mSerialPortFd != -1) {
	if (::close(mSerialPortFd) == -1)
	    CLogger::error("Cannot close serial port", EXIT_SERIAL_PORT);
	mSerialPortFd = -1;
	mPortName = "";
    }
}

vector<pair<string, speed_t>>
CSerialPortUnix::getDeviceSpeeds()
{
    vector<pair<string, speed_t>> ds;
    vector<pair<string, speed_t>>::const_iterator it;
    
    // Test which baudrates are supported by underlying device
    for (it = mBaudrates.begin(); it != mBaudrates.end(); ++it) {
	// Try to set the baudrate
	struct termios options;

	if (tcgetattr(mSerialPortFd, &options) != 0)
	    CLogger::error("Cannot get attributes of serial port " + mPortName, EXIT_SERIAL_PORT);
	
	if (!cfsetispeed(&options, it->second)
	    && !cfsetospeed(&options, it->second)
	    && !tcsetattr(mSerialPortFd, TCSANOW, &options)) {
	    ds.push_back(*it);
	}
    }

    return ds;
}

string
CSerialPortUnix::getSpeeds(string portName)
{
    ostringstream os;
    int c;
    vector<pair<string, speed_t>> s;
    vector<pair<string, speed_t>>::const_iterator it;

    openPort(portName);
    s = getDeviceSpeeds();
    for (c = 0, it = s.begin(); it != s.end(); ++c, ++it)
       	os << setw(7) << right << it->first << " Bd" << endl;
    // Close serial port
    close();

    if (c == 0)
	CLogger::error("Serial port device does not support any baudrates", EXIT_SERIAL_PORT);

    return os.str();
}

pair<string, speed_t>
CSerialPortUnix::findSpeed(string speed, const vector<pair<string, speed_t>> & list)
{
    vector<pair<string, speed_t>>::const_iterator it;
    vector<pair<string, speed_t>>::const_iterator defit;
    bool foundDef = false;

    for (it = list.begin(); it != list.end(); ++it) {
    	if (!(it->first).compare(speed))
	    return *it; // Exact match
	else if (!(it->first).compare(DEFAULT_SERIAL_SPEED)) {
	    defit = it; // We have found default serial speed
	    foundDef = true;
	}
    }
    
    if (foundDef)
	// We have not found exact speed and default speed. Choose
	// highest supported.
	return *defit;
    
    return *(it - 1);
}

void
CSerialPortUnix::setSpeed(pair<string, speed_t> speed)
{    
    struct termios options;
    
    if (tcgetattr(mSerialPortFd, &options)
	|| cfsetispeed(&options, speed.second)
	|| cfsetospeed(&options, speed.second)
	|| tcsetattr(mSerialPortFd, TCSANOW, &options)) {
	CLogger::error("Cannot set serial speed: " + speed.first + ": " + strerror(errno), EXIT_SERIAL_PORT);
    }
}


ssize_t
CSerialPortUnix::writeSingle(uint8_t *data, int data_length)
{
    ssize_t r = ::write(mSerialPortFd, data, data_length);

    if (r <= 0) {
	ostringstream os;
	os << "Cannot write to serial port";
	if (r < 0)
	    os << ": " << string(strerror(errno));
	CLogger::error(os.str(), EXIT_SERIAL_PORT);
    }
    
    return r;
}

ssize_t
CSerialPortUnix::readSingle(uint8_t *data, int data_length)
{
    ssize_t r = 0;
    int s;
    fd_set read_fds, write_fds, except_fds;
    struct timeval timeout;

    // Renew file descriptor sets
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(mSerialPortFd, &read_fds);
    // Set timeout
    timeout.tv_sec = mReadTimeoutMs / 1000;
    timeout.tv_usec = 0;
    // Wait for event
    s = select(mSerialPortFd + 1, &read_fds, &write_fds, &except_fds, &timeout);
    if (s == 1) {
        // One descriptor changed, ready to read
        r = ::read(mSerialPortFd, data, data_length);
    } else if (s == 0) {
        // Timeout occured
        CLogger::error("Timeout occured while reading data from serial port", EXIT_SERIAL_PORT);
    } 
    
    if ((s < 0) || (r <= 0)) {
        // Error occured in select or read
        ostringstream os;
        os << "Cannot read from serial port";
        if (r < 0) {
            os << ": " <<  strerror(errno);
            CLogger::error(os.str(), EXIT_SERIAL_PORT);
        }
    }

    return r;
}
