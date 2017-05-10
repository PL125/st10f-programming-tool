#ifndef MCU_ST10F269_HPP
#define MCU_ST10F269_HPP

#include "McuSpecifics.hpp"

class CMcuSt10f269 : public IMcuSpecifics {
private:
    list<uint32_t> mBlockSizes;
public:
    static bool hasThisId(uint16_t idmanuf, uint16_t idchip);

    CMcuSt10f269();
    ~CMcuSt10f269();

    string getName();
    uint8_t *getFirmware();
    int getFirmwareLength();
    const list<uint32_t> getBlockSizes();
    uint32_t getFlashSize();
    int getEraseTimeout();
    string getMessageForRetCode(uint16_t ret);
    const list<uint16_t> getConfigData(float mcuFrequency);
};
    
#endif
