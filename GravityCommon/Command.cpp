#include "Command.h"

namespace bali
{

//uint64_t cmd::Command::nextSeq = 0;

Command::Command()
{

}




void Timestamp::start()
{
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&s);
}

void Timestamp::end()
{
    QueryPerformanceCounter(&e);
}

void Timestamp::update()
{
}

LARGE_INTEGER Timestamp::getTime()
{
    LARGE_INTEGER result;
    result.QuadPart = e.QuadPart - s.QuadPart;
    result.QuadPart *= 1000000;
    result.QuadPart /= f.QuadPart;
    return result;
}








}

