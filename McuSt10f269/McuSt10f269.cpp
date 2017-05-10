#include "Logger.hpp"
#include "McuSt10f269.hpp"
#include "fw_st10f269.hpp"

using FwSt10f269::fw_st10f269;
using FwSt10f269::fw_st10f269_length;

#define ST10F269_FLASH_SIZE (256 * 1024)
#define ST10F269_IDMANUF 0x0401
#define ST10F269_IDCHIP 0x10D0
#define ST10F269_ERASE_TIMEOUT 15000 // Ms

#define RET_ERASE_ERROR 0x030
#define RET_WRITE_ERROR 0x031

bool
CMcuSt10f269::hasThisId(uint16_t idmanuf, uint16_t idchip)
{
    // Mask out revision bits 3 - 0 from idchip value
    return ((idmanuf == ST10F269_IDMANUF)
            && ((idchip & 0xfff0) == ST10F269_IDCHIP));
}

CMcuSt10f269::CMcuSt10f269()
{
    mBlockSizes.push_back(16);
    mBlockSizes.push_back(8);
    mBlockSizes.push_back(8);
    mBlockSizes.push_back(32);
    mBlockSizes.push_back(64);
    mBlockSizes.push_back(64);
    mBlockSizes.push_back(64);
}

CMcuSt10f269::~CMcuSt10f269()
{
    ;
}

string
CMcuSt10f269::getName()
{
    return string("ST10F269");
}

uint8_t *
CMcuSt10f269::getFirmware()
{
    return fw_st10f269;
}

int
CMcuSt10f269::getFirmwareLength()
{
    return fw_st10f269_length;
}

const list<uint32_t>
CMcuSt10f269::getBlockSizes()
{
    return mBlockSizes;
}

uint32_t
CMcuSt10f269::getFlashSize()
{
    return ST10F269_FLASH_SIZE; 
}

int
CMcuSt10f269::getEraseTimeout()
{
    return ST10F269_ERASE_TIMEOUT; 
}

string
CMcuSt10f269::getMessageForRetCode(uint16_t ret)
{
    string s;
    
    switch (ret) {
    case RET_ERASE_ERROR: 
        s = "Error while erasing memory";
        break;
    case RET_WRITE_ERROR: 
        s = "Error while writing memory";
        break;
    default: 
        s = "";
        break;
    }

    return s;
}

const list<uint16_t>
CMcuSt10f269::getConfigData(float mcuFrequency)
{
    list<uint16_t> ret;
    ret.push_back(0x00);   // Any byte value
    return ret;
}
