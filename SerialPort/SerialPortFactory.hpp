#ifndef SERIAL_PORT_FACTORY_HPP
#define SERIAL_PORT_FACTORY_HPP 1

#include <memory>

#include "SerialPort.hpp"

class CSerialPortFactory {
public:
    std::unique_ptr<CSerialPort> getSerialPort();
};

#endif
