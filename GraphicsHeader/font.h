#ifndef __QROX_FONT_H__
#define __QROX_FONT_H__

#include <memory>
#include <string>

#include <windows.h>

namespace ui {
    enum class h_align {
        left, center, right
    };

    enum class v_align {
        top, center, baseline, bottom
    };

    class font {
        friend class opengl;
    private:
        LOGFONT lf;
        std::shared_ptr<HFONT__> f;

        font();
    public:
        font(LOGFONT & lf);
        font(std::string typeface, int size);
    };
}

#endif
