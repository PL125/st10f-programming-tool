#include "Logger.hpp"
#include "ExitCodes.hpp"
#include "McuSt10f168.hpp"
#include "fw_st10f168.hpp"

#include <sstream>

using FwSt10f168::fw_st10f168;
using FwSt10f168::fw_st10f168_length;

#define ST10F168_FLASH_SIZE (256 * 1024)
#define ST10F168_IDMANUF 0x0400
#define ST10F168_IDCHIP 0x0A80
#define ST10F168_ERASE_TIMEOUT 15000 // Ms
#define ST10F168_MIN_PERIOD 30  // ns
#define ST10F168_MAX_PERIOD 199 // ns
#define ST10F168_MAX_FREQ   25  // MHz

using std::ostringstream;

bool
CMcuSt10f168::hasThisId(uint16_t idmanuf, uint16_t idchip)
{
    // Mask out revision bits 3 - 0 from idchip value
    return ((idmanuf == ST10F168_IDMANUF)
            && ((idchip & 0xfff0) == ST10F168_IDCHIP));
}

CMcuSt10f168::CMcuSt10f168()
{
    mBlockSizes.push_back(16);
    mBlockSizes.push_back(48);
    mBlockSizes.push_back(96);
    mBlockSizes.push_back(96);
}

CMcuSt10f168::~CMcuSt10f168()
{
    ;
}

string
CMcuSt10f168::getName()
{
    return string("ST10F168");
}

uint8_t *
CMcuSt10f168::getFirmware()
{
    return fw_st10f168;
}

int
CMcuSt10f168::getFirmwareLength()
{
    return fw_st10f168_length;
}

const list<uint32_t>
CMcuSt10f168::getBlockSizes()
{
    return mBlockSizes;
}

uint32_t
CMcuSt10f168::getFlashSize()
{
    return ST10F168_FLASH_SIZE; 
}

int
CMcuSt10f168::getEraseTimeout()
{
    return ST10F168_ERASE_TIMEOUT; 
}

string
CMcuSt10f168::getMessageForRetCode(uint16_t ret)
{
    string s;

    switch (ret) {
    case 0x00: 
        s = "Operation was successful";
        break;
    case 0x01: 
        s = "Flash Protection is active";
        break;
    case 0x02: 
        s = "Vpp voltage not present";
        break;
    case 0x03: 
        s = "Programming operation failed";
        break;
    case 0x04: 
        s = "Address value (R1) incorrect; not in Flash address area or odd";
        break;
    case 0x05: 
        s = "CPU period out of range (must be between 30 ns and 199 ns)";
        break;
    case 0x06:
        s = "Not enough free space on system stack for proper operation";
        break;
    case 0x07: 
        s = "Incorrect bank number (R2,R3) specified";
        break;
    case 0x08: 
        s = "Erase operation failed (phase 1)";
        break;
    case 0x09: 
        s = "Bad source address for Multiple Word programming command";
        break;
    case 0x0A: 
        s = "Bad number of words to be copied in Multiple Word programming command; one destination will be out of FLASH";
        break;
    case 0x0B: 
        s = "PLL Unlocked or Oscilator watchdog overflow occured during programming or erasing the FLASH";
        break;
    case 0x0C: 
        s = "Erase operation failed (phase 2)";
        break;
    case 0x0D: 
        s = "MCU serial buffer overrun, use lower serial speed";
        break;
    case 0xFF: 
        s = "Unknown or bad command";
        break;
    default: 
        s = "";
        break;
    }

    return s;
}

const list<uint16_t>
CMcuSt10f168::getConfigData(float mcuFrequency)
{
    list<uint16_t> ret;
    ret.push_back(get2TclConst(mcuFrequency));
    return ret;
}

uint16_t
CMcuSt10f168::get2TclConst(float mcuFrequency)
{
    if (mcuFrequency > ST10F168_MAX_FREQ) {
        ostringstream os;
        os << "User-entered CPU frequency " << mcuFrequency << " MHz is outside range ";
        os << "(0, " << ST10F168_MAX_FREQ << "]";
        CLogger::error(os.str(), EXIT_MCU);
    }
    uint16_t t = ((uint16_t) (1000.0 / mcuFrequency)) + 1;
    if (t > 200) {
        ostringstream os;
        os << "For user-entered CPU frequency " << mcuFrequency << "MHz";
        os << " the period is " << t << " ns which is greater than maximal allowed ";
        os << "period " << ST10F168_MAX_PERIOD << " ns. Setting period to ";
        os << ST10F168_MAX_PERIOD << " ns";
        CLogger::warning(os.str());
        t = ST10F168_MAX_PERIOD;
    } else if (t < 30) {
        ostringstream os;
        os << "For user-entered CPU frequency " << mcuFrequency << "MHz";
        os << " the period is " << t << " ns which is lower than minmal allowed ";
        os << "period " << ST10F168_MIN_PERIOD << " ns. Setting period to ";
        os << ST10F168_MIN_PERIOD << " ns";
        CLogger::warning(os.str());
        t = ST10F168_MIN_PERIOD;
    }
    return t;
}
