#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "../define.h"
#include "../nsCompo/nsCompo.h"

#ifdef __sun
#define __cfmakeraw(tio) { \
    tio.c_iflag &= ~(IMAXBEL|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON); \
    tio.c_oflag &= ~OPOST; \
    tio.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN); \
    tio.c_cflag &= ~(CSIZE|PARENB); \
    tio.c_cflag |= CS8; \
}
#else
#define __cfmakeraw(tio) cfmakeraw(&tio)
#endif

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace test
{

class cCounter : private ps::app::mkcrd::nsCompo::SIMPLE_STATS
{
public:
    cCounter()
        : ps::app::mkcrd::nsCompo::SIMPLE_STATS(2), sStatId_("time:")
    {
        vStartElapse(sStatId_);
    }
    ~cCounter()
    {
        vStopElapse(sStatId_);
    }
private:
    tstring sStatId_;
};

} // ps::app::mkcrd::test
} // ps::app::mkcrd
} // ps::app
} // ps

int main(const int argc, const char* argv[])
{
    ps::app::mkcrd::test::cCounter oCounter;

    tcout << "please enter" << std::endl;

    struct termios tCookedTermIos_;
    struct termios tRawTermIos_;
    tcgetattr(STDIN_FILENO, &tCookedTermIos_);
    tRawTermIos_ = tCookedTermIos_;
    __cfmakeraw(tRawTermIos_);
    tRawTermIos_.c_iflag |= BRKINT;
    tcsetattr(STDIN_FILENO, 0, &tRawTermIos_);
    
    char c;
    c = getchar();

    tcsetattr(STDIN_FILENO, 0, &tCookedTermIos_);
    tcout << std::endl;
    
    return EXIT_SUCCESS;
}

