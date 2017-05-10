#ifndef USER_CONFIG_HPP
#define USER_CONFIG_HPP 1

#include <iostream>
#include <list>
#include <vector>

using std::string;
using std::list;
using std::vector;

class CUserConfig {
private:
    string mSerialPortName;
    string mSerialSpeed;
    bool mVerboseMode;
    bool mHelp;
    bool mVersion;
    bool mIdent;
    bool mSpeeds;
    float mMcuFrequency;
    bool mPrintProgress;
    // Erase    
    bool mErase;
    list<unsigned int> mEraseBlockList;
    // Read
    bool mRead;
    string mReadOutputFilename;
    int mReadLength;
    // Write
    bool   mWrite;
    bool   mWriteEraseWholeMemory;
    string mWriteInputFilename;
    bool   mWriteCheckByRead;

    string getArgument(vector<char *>::const_iterator args, vector<char *>::const_iterator end);
    void parseEraseArguments(vector<char *>::const_iterator args, vector<char *>::const_iterator end);
    void parseReadArguments(vector<char *>::const_iterator args, vector<char *>::const_iterator end);
    void parseWriteArguments(vector<char *>::const_iterator args, vector<char *>::const_iterator end);
    void parseCommandLine(vector<char *> & args);
    
public:
    CUserConfig(int argc, char **argv);

    const string getHelpMessage(const string & execName) const;

    string & getSerialPortName();
    string & getSerialSpeed();
    bool isSpeedsSet();
    bool isVerboseModeSet();
    bool isPrintProgressSet();
    bool isHelpSet();
    bool isVersionSet();
    bool isIdentSet();
    bool isEraseSet();
    list<unsigned int> getEraseBlockList();
    bool isReadSet();
    string & getReadOutputFname();
    bool isWriteSet();
    string & getWriteInputFname();
    int getReadLength();
    bool getWriteEraseWholeMemory();
    bool getWriteCheckByRead();
    float getMcuFrequency();
};

#endif
