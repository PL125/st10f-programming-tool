#include "SerialPort.hpp"
#include "Logger.hpp"
#include "ExitException.hpp"
#include "ExitCodes.hpp"
#include <cstring>
#include <sstream>

using std::ostringstream;

#define READ_DEFAULT_TIMEOUT 3000 // ms

void
CSerialPort::writeWord(uint16_t w)
{
    uint8_t b;
    
    b = (w & 0xff);
    this->write(&b, 1, 1);
    
    b = (w & 0xff00) >> 8;
    this->write(&b, 1, 1);
}

uint16_t
CSerialPort::readWord()
{
    uint8_t b;
    uint16_t w;
    
    w = 0;
    
    this->read(&b, 1);
    w |= ((uint16_t) b);

    this->read(&b, 1);
    w |= ((uint16_t) b) << 8;

    return w;
}

uint32_t
CSerialPort::readDoubleWord()
{
    uint8_t b;
    uint32_t dw;

    dw = 0;
    
    this->read(&b, 1);
    dw |= ((uint32_t) b);

    this->read(&b, 1);
    dw |= ((uint32_t) b) << 8;

    this->read(&b, 1);
    dw |= ((uint32_t) b) << 16;
    
    this->read(&b, 1);
    dw |= ((uint32_t) b) << 24;

    return dw;
}

void
CSerialPort::setReadTimeout(int timetout)
{
    mReadTimeoutMs = timetout;
}

void
CSerialPort::setDefaultTimeout()
{
    mReadTimeoutMs = READ_DEFAULT_TIMEOUT;
}

void
CSerialPort::write(uint8_t *data, int data_length, int padd_to)
{    
    // Write data
    for (int i = 0; i < data_length; )
        i += writeSingle(data, data_length);
    // Write pad
    // TODO: odkial brat hodnotu pad byte??
    uint8_t p[512];
    memset(p, 0xFF, 512);

    for (padd_to -= data_length; padd_to > 0; ) {
        int w = 512;
        if (padd_to < 512)
            w = padd_to;
        padd_to -= writeSingle(p, w);
    }
}

void
CSerialPort::read(uint8_t *data, int data_length)
{
    for (int i = 0; i < data_length; ) {
        int old_i = i;
        i += readSingle(data + i, data_length - i);
        // TODO: pre Win32, pretoze tam nedetekujeme timeout a moze vracat 0 bajtov precitanych
        if (i == old_i) {
            CLogger::error("Timeout occured while reading data from serial port", EXIT_SERIAL_PORT);
        }
    }
}

void
CSerialPort::sendSafeByte(uint8_t b)
{
    uint8_t r;

    this->write(&b, 1, 1);
    this->read(&r, 1);
    if (r != b) {
        ostringstream os;
        os << "Bad echo when sending byte safely, expected " << CLogger::decToHex(b);
        os << " received " << CLogger::decToHex(r);
        CLogger::error(os.str(), EXIT_SERIAL_PORT);
    }
    this->write(&b, 1, 1);
    this->read(&r, 1);
    if (r != 0x00) {
        ostringstream os;
        os << "Cannot send byte safely, expected 0x00 received " << CLogger::decToHex(r);  
        CLogger::error(os.str(), EXIT_SERIAL_PORT);    
    }
}

void
CSerialPort::sendSafeWord(uint16_t w)
{
    this->writeWord(w);
    uint16_t r = this->readWord();
    if (r != w) {
        ostringstream os;
        os << "Bad echo when sending word safely, expected " << CLogger::decToHex(w);
        os << " received " << CLogger::decToHex(r);
        CLogger::error(os.str(), EXIT_SERIAL_PORT);
    }
    this->writeWord(w);
    r = this->readWord();
    if (r != 0x00) {
        ostringstream os;
        os << "Cannot send word safely, expected 0x0000 received " << CLogger::decToHex(r);  
        CLogger::error(os.str(), EXIT_SERIAL_PORT);    
    }
}

void
CSerialPort::sendSafeDoubleWord(uint32_t w)
{
    // Send low word
    sendSafeWord(w & 0x0000FFFF);
    // Send high word
    sendSafeWord((w & 0xFFFF0000) >> 16);
}
