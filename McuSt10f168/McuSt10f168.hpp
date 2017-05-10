#ifndef MCU_ST10F168_HPP
#define MCU_ST10F168_HPP

#include "McuSpecifics.hpp"

class CMcuSt10f168 : public IMcuSpecifics {
private:
    list<uint32_t> mBlockSizes;

    uint16_t get2TclConst(float mcuFrequency);
public:
    static bool hasThisId(uint16_t idmanuf, uint16_t idchip);

    CMcuSt10f168();
    ~CMcuSt10f168();

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
