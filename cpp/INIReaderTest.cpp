// Example that shows simple usage of the INIReader class

#include <iostream>
#ifndef PROPOSED
#include "INIReader.h"
#else
#include "proposed/INIReader.h"
#endif

#ifndef WIN32
#include <ctime>
#else
#include <windows.h>
#endif

#define COUNT 1000000

#ifndef WIN32
static const clockid_t sClockId = CLOCK_MONOTONIC;
static const long sNsInSecond = 1000000000;
#define timespec2ns(A) (A.tv_sec * sNsInSecond + A.tv_nsec)
#endif

int main()
{
    INIReader reader("examples/test.ini");

    if (reader.ParseError() < 0) {
        std::cout << "Can't load 'test.ini'\n";
        return 1;
    }
    #ifndef WIN32
    timespec startTime;
    clock_gettime(sClockId, &startTime);
    #else
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency) == false) {
        return -1;
    }
    LARGE_INTEGER startTime;
    QueryPerformanceCounter(&startTime);
    #endif
    for (size_t i = 0; i < COUNT; ++i) {
        if (-1 == reader.GetInteger("protocol", "version", -1)) {
            return 1;
        }
        if ("UNKNOWN" == reader.Get("user", "name", "UNKNOWN")) {
            return 1;
        }
        if ("UNKNOWN" == reader.Get("user", "email", "UNKNOWN")) {
            return 1;
        }
        if (-1 == reader.GetReal("user", "pi", -1)) {
            return 1;
        }
        if (false == reader.GetBoolean("user", "active", false)) {
            return 1;
        }
    }
    #ifndef WIN32
    timespec currentTime;
    clock_gettime(sClockId, &currentTime);
    long delta = (timespec2ns(currentTime) - timespec2ns(startTime));
    std::cout << static_cast<double>(delta) / sNsInSecond << " seconds (" << delta << " nanoseconds)" << std::endl;
    #else
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    long delta = currentTime.QuadPart - startTime.QuadPart;
    std::cout << static_cast<double>(delta) / frequency.QuadPart << " seconds (" << delta << ")" << std::endl;
    #endif
    return 0;
}
