#include <iostream>
#include "timer.h"

using namespace std;

map<u32, timer *> timer::timers;

VOID CALLBACK timer::timer_proc(HWND hwnd, UINT umsg, UINT idevent, DWORD dwtime) {
    auto i = timers.find(idevent);
    if (i != timers.end()) {
        timer * t = i->second;
        UINT nextt = t->callback(dwtime);
        if (nextt == 0) {
            timers.erase(idevent);
            KillTimer(NULL, idevent);
            t->id = 0;
        } else {
            SetTimer(NULL, idevent, nextt, timer_proc);
        }
    }
}

timer::timer() : id(0), callback() {
}

timer::timer(UINT t, function<UINT (DWORD)> callback) : callback(callback) {
    id = SetTimer(NULL, 0, t, timer_proc);
    timers.insert(pair<u32, timer *>(id, this));
}

timer::~timer() {
    kill();
}

void timer::kill() {
    timers.erase(id);
    KillTimer(NULL, id);
    id = 0;
}
