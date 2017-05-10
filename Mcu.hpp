#ifndef MCU_HPP
#define MCU_HPP 1

#include "SerialPort.hpp"
#include "McuSpecifics.hpp"

#include <cstdint>
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class CMcu {
private:
    CSerialPort & mSerialPort;
    unique_ptr<IMcuSpecifics> mMcuSpecifics;
    float mMcuFrequency;

    void decodeIdentData(uint8_t data[4], uint16_t & idmanuf, uint16_t & idchip);
    void setMcuSpecificsById(uint16_t idmanuf, uint16_t idchip);
    string getMessageForRetCode(uint16_t ret);
    void sendShellCommand(uint8_t cmd);

public:
    CMcu(CSerialPort & serialPort, float mcuFrequency);

    void erase();
    void erase(list<unsigned int> blockList);
    void erase(uint32_t startAddr, uint32_t endAddr);
    void write(vector<uint8_t> data, bool printProgress);
    vector<uint8_t> read(bool printProgress);
    vector<uint8_t> read(uint32_t size, bool printProgress);
    string ident();
};

#endif
