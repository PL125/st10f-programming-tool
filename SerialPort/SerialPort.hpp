#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H 1

#include <iostream>
#include <cstdint>

using std::string;

class CSerialPort {
protected:
    int mReadTimeoutMs; // Miliseconds

    virtual ssize_t readSingle(uint8_t *data, int data_length) = 0;
    virtual ssize_t writeSingle(uint8_t *data, int data_length) = 0;
public:
    virtual ~CSerialPort() { ; };
    
    virtual void open(string portName, string speed) = 0;
    virtual string getSpeeds(string portName) = 0;

    virtual void close() = 0;

    void setReadTimeout(int ms);
    void setDefaultTimeout();

    void writeWord(uint16_t w);
    uint16_t readWord();
    uint32_t readDoubleWord();
    void read(uint8_t *data, int data_length);
    void write(uint8_t *data, int data_length, int padd);
    void sendSafeByte(uint8_t b);
    void sendSafeWord(uint16_t w);
    void sendSafeDoubleWord(uint32_t w);

};

#endif
