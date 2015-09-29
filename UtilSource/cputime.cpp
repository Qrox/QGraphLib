#include "cputime.h"

#ifdef _WIN32
#include <windows.h>
#include <math.h>

double get_wall_time() {
    LARGE_INTEGER time, freq;
    if (!QueryPerformanceFrequency(&freq)) return NAN;
    if (!QueryPerformanceCounter(&time)) return NAN;
    return (double) time.QuadPart / freq.QuadPart;
}

double get_cpu_time() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        return double(d.dwLowDateTime | ((unsigned long long) d.dwHighDateTime << 32)) * 0.0000001;
    } else return NAN;
}

#else
#include <sys/time.h>
#include <math.h>

double get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL)) return NAN;
    return double(time.tv_sec) + double(time.tv_usec) * .000001;
}

double get_cpu_time() {
    return double(clock()) / CLOCKS_PER_SEC;
}

#endif
