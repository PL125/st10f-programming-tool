#ifndef SERIAL_PORT_UNIX_H
#define SERIAL_PORT_UNIX_H 1

#include <termios.h>

#include <vector>
#include "SerialPort.hpp"

using std::vector;
using std::pair;

class CSerialPortUnix : public CSerialPort {
private:
    int mSerialPortFd;
    string mPortName;
    vector< pair<string, speed_t> > mBaudrates;

    vector<pair<string, speed_t>> getDeviceSpeeds();
    pair<string, speed_t> findSpeed(string speed, const vector<pair<string, speed_t>> & list);
    void setSpeed(pair<string, speed_t> speed);
    void openPort(string portName);

    ssize_t readSingle(uint8_t *data, int data_length);
    ssize_t writeSingle(uint8_t *data, int data_length);
public:
    CSerialPortUnix();
    ~CSerialPortUnix();

    void open(string portName, string speed);
    string getSpeeds(string portName);

    void close();
};

#endif
