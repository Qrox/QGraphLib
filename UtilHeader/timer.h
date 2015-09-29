#ifndef __QROX_TIMER_H__
#define __QROX_TIMER_H__

#include "types.h"
#include <windows.h>
#include <functional>
#include <map>

class timer {
private:
    static std::map<u32, timer *> timers;

    u32 id;
    std::function<UINT (DWORD)> callback;

    static VOID CALLBACK timer_proc(HWND hwnd, UINT umsg, UINT idevent, DWORD dwtime);
public:
    timer();
    timer(UINT t, std::function<UINT (DWORD)> callback);
    ~timer();
    void kill();
};

#endif
