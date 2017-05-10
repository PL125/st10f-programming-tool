#include <Mcu.hpp>

#include "fw_stage_1.hpp"
#include "fw_ident.hpp"
#include "ExitCodes.hpp"
#include "Logger.hpp"
#include "ExitException.hpp"
#include "McuSt10f269.hpp"
#include "McuSt10f168.hpp"

#include <sstream>

using FwCommon::fw_stage_1;
using FwCommon::fw_stage_1_length;
using FwCommon::fw_ident;
using FwCommon::fw_ident_length;
using std::ostringstream;
using std::nothrow;

#define FW_1_MAX_LENGTH   32
#define FW_MAX_LENGTH     2048
#define BOOTSTRAP_ACK     0xD5
#define SHELL_ACK         0xAB

#define PAD_BYTE          0xFF

#define CMD_PING          0x00
#define CMD_ERASE_BLOCKS  0x01
#define CMD_READ          0x02
#define CMD_WRITE         0x03
#define CMD_IDENTIFY      0x04
#define CMD_ERASE_CHIP    0x05

#define RET_SERIAL_OVERRUN  0x20
#define RET_BAD_ECHO        0x21

#define JUNK_BYTE_COUNT   128


CMcu::CMcu(CSerialPort & serialPort, float mcuFrequency)
    : mSerialPort(serialPort), mMcuFrequency(mcuFrequency)
{
    // Check in which mode MCU is
    CLogger::info("Sending zero byte");	
    uint8_t ack = CMD_PING;
    // Send zero byte
    mSerialPort.write(&ack, 1, 1);
    // Read response
    mSerialPort.read(&ack, 1);

    // If some unknown data were received, try to read out junk bytes from
    // previous operations without collected return status
    for (int i = 0; (ack != BOOTSTRAP_ACK) && (ack != SHELL_ACK) && (i < JUNK_BYTE_COUNT); ++i) {
        mSerialPort.read(&ack, 1);
    }

    uint16_t idchip, idmanuf;
    uint8_t data[4];

    if (ack == SHELL_ACK) {
        CLogger::info("Received stage 2 firmware ACK byte " + CLogger::decToHex(ack));
        CLogger::info("Skipping MCU initialization, NOT LOADING stage 2 firmware");

        // Get IDCHIP and IDMANUF registers using Ident command for initialized MCU        
        CLogger::info("Getting IDMAUNF and IDCHIP registers of already initialized MCU");    
        
        mSerialPort.sendSafeByte(CMD_IDENTIFY);
        mSerialPort.read(data, 4);
        decodeIdentData(data, idchip, idmanuf);

        setMcuSpecificsById(idchip, idmanuf);
    } else if (ack == BOOTSTRAP_ACK) {
        // Ziskame identifikaciu a nahrame shell  mcuSpecfics
        CLogger::info("Received bootstrap loader ACK byte " + CLogger::decToHex(ack));

        // Write the first stage loader
        CLogger::info("Writing stage 1 firmware");
        mSerialPort.write(fw_stage_1, fw_stage_1_length, FW_1_MAX_LENGTH);
        
        // Get identification
        CLogger::info("Getting IDMAUNF and IDCHIP registers");
        mSerialPort.write(fw_ident, fw_ident_length, FW_MAX_LENGTH);
        mSerialPort.read(data, 4);

        decodeIdentData(data, idchip, idmanuf);    
        setMcuSpecificsById(idchip, idmanuf);

        // Load stage 2 firmware
        CLogger::info("Writing stage 2 firmware");
        mSerialPort.write(mMcuSpecifics->getFirmware(),
                          mMcuSpecifics->getFirmwareLength(), FW_MAX_LENGTH);
        // Wait for initialization end
        uint16_t r = mSerialPort.readWord();
        if (r != 0x00) {
            CLogger::error("Cannot initialize MCU: " + getMessageForRetCode(r), EXIT_MCU);
        }
    } else {
        CLogger::error("Received unknown ack byte " + CLogger::decToHex(ack) +
                       ", expected " + CLogger::decToHex(BOOTSTRAP_ACK) +
                       " or " + CLogger::decToHex(SHELL_ACK), EXIT_MCU);
    }
}

void
CMcu::decodeIdentData(uint8_t data[4], uint16_t & idmanuf, uint16_t & idchip)
{
    idmanuf = 0;
    idmanuf |= data[0];
    idmanuf |= (data[1]) << 8;

    idchip = 0;
    idchip |= data[2];
    idchip |= (data[3]) << 8;
}


string
CMcu::getMessageForRetCode(uint16_t ret)
{
    string s;
    
    switch (ret) {
    case RET_SERIAL_OVERRUN: 
        s = "MCU serial receiver buffer overrun, use lower serial speed";
        break;
    case RET_BAD_ECHO: 
        s = "MCU received corrupted double word value";
        break;
    default:
        if (mMcuSpecifics == 0 || (s = mMcuSpecifics->getMessageForRetCode(ret)).empty())
            s = "Unknown MCU return code: " + CLogger::decToHex(ret);
        break;
    }

    return s;
}

void
CMcu::setMcuSpecificsById(uint16_t idmanuf, uint16_t idchip)
{
    if (CMcuSt10f269::hasThisId(idmanuf, idchip)) {
	CLogger::info("ST10F269 MCU found");
	mMcuSpecifics.reset(new CMcuSt10f269());
    } else if (CMcuSt10f168::hasThisId(idmanuf, idchip)) {
        CLogger::info("ST10F168 MCU found");
        mMcuSpecifics.reset(new CMcuSt10f168());
    } else {
        ostringstream os;
        os << "Received unknown MCU identification registers values: ";
        os << "IDMANUF=" << CLogger::decToHex(idmanuf);
        os << "    IDCHIP=" << CLogger::decToHex(idchip);
        CLogger::error(os.str(), EXIT_MCU);
    }
}

// -----------------------------------------------------------------------------
//  Operacie
// -----------------------------------------------------------------------------

void
CMcu::sendShellCommand(uint8_t cmd)
{
    // Check needed frequency
    if ((mMcuSpecifics->getName() != "ST10F168") && (mMcuFrequency > 0))
        CLogger::info("Ignoring -f option for this MCU");
    if ((mMcuSpecifics->getName() == "ST10F168") && (mMcuFrequency == 0))
        CLogger::error("Missing -f option for MCU " + mMcuSpecifics->getName(), EXIT_MCU);
    
    mSerialPort.sendSafeByte(cmd);
    // Write configuration data
    list<uint16_t> d = mMcuSpecifics->getConfigData(mMcuFrequency);
    list<uint16_t>::const_iterator it;
    for (it = d.begin(); it != d.end(); ++it)
        mSerialPort.sendSafeWord(*it);
}

string
CMcu::ident()
{
    return mMcuSpecifics->getName();
}

void
CMcu::erase()
{
    uint16_t r;
    
    sendShellCommand(CMD_ERASE_CHIP);
    // Wait for return status of erase operation
    mSerialPort.setReadTimeout(mMcuSpecifics->getEraseTimeout());
    r = mSerialPort.readWord();
    mSerialPort.setDefaultTimeout();

    if (r != 0)
    	CLogger::error(getMessageForRetCode(r), EXIT_MCU);
}

void
CMcu::erase(uint32_t startAddr, uint32_t endAddr)
{
    list<uint32_t> bs = mMcuSpecifics->getBlockSizes();

    // Block address space
    typedef struct {
        uint32_t a;
        uint32_t b;
    } range_t;
    
    unique_ptr<range_t []> ba(new(nothrow) range_t[bs.size()]);

    if (!ba)
        CLogger::error("Cannot allocate memory", EXIT_MCU);

    list<uint32_t>::const_iterator it;
    unsigned int i;
    uint32_t a = 0;
    for (i = 0, it = bs.begin(); it != bs.end(); ++it, ++i) {
        ba[i].a = a;
        a = (*it * 1024);
        ba[i].b = ba[i].a + a - 1;
        a = ba[i].a + a;
    }

    uint32_t lastValidAddr = mMcuSpecifics->getFlashSize() - 1;

    if (startAddr > lastValidAddr) {
        ostringstream os;
        os << "Start erase address " << startAddr << " is out of range [0," << lastValidAddr << "]";
        CLogger::error(os.str(), EXIT_MCU);
    }

    if (endAddr > lastValidAddr) {
        ostringstream os;
        os << "End erase address " << endAddr << " is out of range [0," << lastValidAddr << "]";
        CLogger::error(os.str(), EXIT_MCU);
    }

    if (startAddr > endAddr) {
        ostringstream os;
        os << "Start erase address " << startAddr << " is greater than end erase address " << endAddr;
        CLogger::error(os.str(), EXIT_MCU);
    }

    // Construct block list
    list<unsigned int> l;
    // Find block containing startAddr
    for (i = 0; i < bs.size(); ++i) {
        if (startAddr >= ba[i].a && startAddr <= ba[i].b)
            break;
    }
    // Find blocks up to the block containing endAddr
    for ( ; i < bs.size(); ++i) {
        l.push_back(i);
        if (endAddr >= ba[i].a && endAddr <= ba[i].b)
            break;
    }

    // Erase blocks covering the range
    erase(l);
}

void
CMcu::erase(list<unsigned int> blockList)
{
    // Construct mask
    uint16_t mask = 0;
    list<unsigned int>::const_iterator it;
    unsigned int bc = mMcuSpecifics->getBlockSizes().size();

    for (it = blockList.begin(); it != blockList.end(); ++it) {
        if (*it >= bc) {
            ostringstream os;
            os << "Block number " << *it << " is out of range [0," << (bc - 1) << "]";
            CLogger::error(os.str(), EXIT_MCU);
        }
        mask |= 1 << *it;
    }
    ostringstream os;
    os << "Erasing blocks: ";
    for (it = blockList.begin(); it != blockList.end(); ++it) {
        if (it != blockList.begin())
            os << ",";
        os << *it;
    }
    os << " (mask = " << CLogger::decToHex(mask) << ")"; 
    CLogger::info(os.str());
    
    // Erase blocks
    sendShellCommand(CMD_ERASE_BLOCKS);
    mSerialPort.sendSafeWord(mask);
    // Read status
    mSerialPort.setReadTimeout(mMcuSpecifics->getEraseTimeout());
    uint16_t r = mSerialPort.readWord();

    if (r != 0x00)
    	CLogger::error(getMessageForRetCode(r), EXIT_MCU);

    mSerialPort.setDefaultTimeout();
}

void
CMcu::write(vector<uint8_t> data, bool printProgress)
{
    if ((data.size() < 1) || (data.size() > mMcuSpecifics->getFlashSize())) {
        ostringstream os;
        os << "Data length " << data.size() << " to write is not in range ";
        os << "[1-" << mMcuSpecifics->getFlashSize() << "]";
        CLogger::error(os.str(), EXIT_MCU);
    }
    // Number of bytes to be written
    uint32_t bw = ((data.size() % 2) == 1) ? data.size() + 1 : data.size();
    ostringstream os;
    os << "Writing " << data.size() << " bytes";
    if (bw != data.size())
        os << " + 1 byte pad";
    CLogger::info(os.str());
    // Write command
    sendShellCommand(CMD_WRITE);
    // Write number of bytes to read
    mSerialPort.sendSafeDoubleWord(bw);
    // Write by 1024 blocks and read return status after each one
    uint32_t i = 0;

    if (printProgress)
        CLogger::progress(i, data.size());

    while (i < bw) {
        uint32_t s = ((bw - i) > 1024) ? 1024 : (bw - i);        
        if (((i + s) >= bw) && (bw != data.size())) {
            // Going to write last block of size increased by pad
            mSerialPort.write(data.data() + i, s - 1, s - 1);
            uint8_t b = PAD_BYTE;
            mSerialPort.write(&b, 1, 1);
        } else {
            mSerialPort.write(data.data() + i, s, s);
        }
        i += s;

        if (printProgress)
            CLogger::progress(i, data.size());

        // Read status
        uint16_t r = mSerialPort.readWord();
        if (r == 0) {
            // Read position
            uint32_t u = mSerialPort.readDoubleWord();
            // Check position
            if ((bw - u) != i) {
                ostringstream os;
                os << "Serial communication error: position status mismatch, expected " << i << ", have " << (bw - u);
                CLogger::error(os.str(), EXIT_MCU);
            }
        } else {
            CLogger::error(getMessageForRetCode(r), EXIT_MCU);
        }
    }
}

vector<uint8_t>
CMcu::read(bool printProgress)
{
    return read(mMcuSpecifics->getFlashSize(), printProgress);
}

vector<uint8_t>
CMcu::read(uint32_t size, bool printProgress)
{
    uint32_t r;

    if ((size < 1) || (size > mMcuSpecifics->getFlashSize())) {
        ostringstream os;
        os << "Data length " << size << " to read is outside address range ";
        os << "[1-" << mMcuSpecifics->getFlashSize() << "]";
        CLogger::error(os.str(), EXIT_MCU);
    }

    // Check if size is odd or even number
    r = size;
    if ((size % 2) == 1)
        r++;
    // Write command
    sendShellCommand(CMD_READ);
    // Write number of bytes to read
    mSerialPort.sendSafeDoubleWord(r);
    // Get data from FLASH memory by 1024 byte blocks
    uint32_t i = 0;
    if (printProgress)
        CLogger::progress(i, size);

    vector<uint8_t> data;

    while (i < r) {
        uint32_t s = ((r - i) > 1024) ? 1024 : (r - i);
        // Read data
        data.resize(data.size() + s);
        mSerialPort.read(data.data() + i , s);
        i += s;

        if (printProgress)
            CLogger::progress(i, size);

        // Read status
        uint16_t t = mSerialPort.readWord();
        if (t == 0) {
            // Read position
            uint32_t u = mSerialPort.readDoubleWord();
            // Check position
            if ((r - u) != i) {
                ostringstream os;
                os << "Serial communication error: position status mismatch";
                os << ", expected " << i << " has " << (r - u);
        	CLogger::error(os.str(), EXIT_MCU);                
            }
        } else {
            CLogger::error(getMessageForRetCode(r), EXIT_MCU);
        }
    }

    // Discard possible rounding/pad byte
    data.resize(size);
    return data;
}
