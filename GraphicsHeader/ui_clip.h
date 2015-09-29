#ifndef __QROX_UI_CLIP_H__
#define __QROX_UI_CLIP_H__

#include <vector>

namespace ui {
    class clip {
    public:
        struct rect {
            int x, y, w, h;
        };
    private:
        struct rect_trans {
            int tx, ty, x0, y0, x1, y1;
        };

        std::vector<rect_trans> clips;

        bool apply(int byte);
    public:
        void clear();
        bool push(rect const & r);
        bool push(class component const & c);
        void pop();
    };
}

#endif
