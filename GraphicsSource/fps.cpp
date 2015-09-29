#include "fps.h"

using namespace std;

namespace ui {
    void fps::feed() {
        clock_t t = clock();
        for (auto i = frames.begin(); i < frames.end();) {
            if (t - *i > (int) (CLOCKS_PER_SEC * summary)) {
                i = frames.erase(i);
            } else break;
        }
        frames.push_back(t);
    }

    fps::fps(float per, float summary) : per(per), summary(summary) {
    }

    fps::operator float() {
        clock_t t = clock();
        for (auto i = frames.begin(); i < frames.end();) {
            if (t - *i > (int) (CLOCKS_PER_SEC * summary)) {
                i = frames.erase(i);
            } else break;
        }
        return frames.size() / summary * per;
    }

    void fps::operator ++() {
        feed();
    }

    void fps::operator ++(int dummy) {
        feed();
    }
}
