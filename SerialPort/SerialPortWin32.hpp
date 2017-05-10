#ifndef SERIAL_PORT_WIN32_H
#define SERIAL_PORT_WIN32_H 1

#include <windows.h>
#include <list>
#include "SerialPort.hpp"

using std::pair;
using std::list;

class CSerialPortWin32 : public CSerialPort {
private:
    HANDLE mSerialPortH;

    struct s_speed {
        string name;
        DWORD key;
        DWORD value;

        s_speed(string n, DWORD k, DWORD v) {
            name = n;
            key = k;
            value = v;
        }
    };
    
    list<struct s_speed> mMaxBaudrates;
    

    void openPort(string portName);
    s_speed  getMaxSpeed(string portName);
    void setTimeouts(int ms);
    string getLastErrorAsString();
  
    ssize_t readSingle(uint8_t *data, int data_length);
    ssize_t writeSingle(uint8_t *data, int data_length);
public:
    CSerialPortWin32();
    ~CSerialPortWin32();
    
    void open(string portName, string speed);
    string getSpeeds(string portName);
  
    void close();
    void write(uint8_t *data, int data_length, int padd_to);
    void read(uint8_t *data, int data_length);
};

#endif
