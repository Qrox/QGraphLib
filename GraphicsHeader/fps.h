#ifndef __QROX_FPS_H__
#define __QROX_FPS_H__

#include <ctime>
#include <vector>

namespace ui {
    class fps {
    private:
        float per, summary;
        std::vector<clock_t> frames;

        void feed();
    public:
        fps(float per = 1, float summary = 1); // default: frames per second, summary every 1 second

        operator float(); // return fps;
        void operator ++(); // notify frame complete
        void operator ++(int dummy); // idem
    };
}

#endif
