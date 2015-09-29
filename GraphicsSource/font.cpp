#include "font.h"

#include <iostream>

using namespace std;

namespace ui {
    font::font() {
    }

    font::font(LOGFONT & lf) : lf(lf), f(CreateFontIndirect(&lf), DeleteObject) {
    }

    font::font(string typeface, int size) :
            lf {
                size,
                0,
                0,
                0,
                FW_NORMAL,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_TT_ONLY_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                "\0"
            } {
        char const * tmp = typeface.c_str();
        auto len = typeface.length();
        if (len > 31) len = 31;
        copy(tmp, tmp + len, lf.lfFaceName);
        lf.lfFaceName[len] = 0;
        f = shared_ptr<HFONT__>(CreateFontIndirect(&lf), DeleteObject);
    }
}
