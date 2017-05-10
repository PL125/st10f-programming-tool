#ifndef LOGGER_HPP
#define LOGGER_HPP 1

#include <iostream>

using std::string;

#define LOGGER_PROGRESS_UNIT 3 // %

class CLogger {
private:
    static bool mLogInfo;
    static int mProgressLastPercent;
public:
    static void error(const string & msg, int returnValue);
    static void warning(const string & msg);
    static void info(const string & msg);
    static void progress(uint32_t b, uint32_t n);
    static void setLogInfo(bool b);
    static string decToHex(int dec);
};

#endif
