#ifndef MCU_SPECIFICS_HPP
#define MCU_SPECIFICS_HPP 1

#include "SerialPort.hpp"
#include "McuSpecifics.hpp"

#include <cstdint>
#include <list>

using std::list;

class IMcuSpecifics {
public:
    virtual ~IMcuSpecifics() {};

    virtual string getName() = 0;
    virtual uint8_t *getFirmware() = 0;
    virtual int getFirmwareLength() = 0;
    virtual const list<uint32_t> getBlockSizes() = 0;
    virtual uint32_t getFlashSize() = 0;
    virtual int getEraseTimeout() = 0;
    virtual string getMessageForRetCode(uint16_t ret) = 0; 
    virtual const list<uint16_t> getConfigData(float mcuFrequency) = 0; 
};

#endif
