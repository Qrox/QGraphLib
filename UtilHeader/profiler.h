#ifndef __QROX_PROFILER_H__
#define __QROX_PROFILER_H__

#include "cputime.h"

class profiler {
private:
    double start;
public:
    profiler();

    operator double();
};

#endif
