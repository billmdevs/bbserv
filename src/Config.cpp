// Filename: Config.cpp

#include "Config.h"
 
pid_t gettid()
{
    return syscall(SYS_gettid);
}

