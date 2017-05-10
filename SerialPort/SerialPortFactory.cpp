#include "SerialPortFactory.hpp"

#ifdef WIN32
#include "SerialPortWin32.hpp"
#else
#include "SerialPortUnix.hpp"
#endif

std::unique_ptr<CSerialPort>
CSerialPortFactory::getSerialPort()
{
    std::unique_ptr<CSerialPort> sp;

#ifdef WIN32
    sp.reset(new CSerialPortWin32());
#else
    sp.reset(new CSerialPortUnix());
#endif

    return sp; 
}
