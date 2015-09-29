#include "ui_layout.h"
#include "ui.h"

#undef min
#undef max

namespace ui {
    layout::layout() : con(nullptr) {}

    grid_layout::grid_layout(int x_cnt, int y_cnt, int x_pad, int y_pad)
        : x_cnt(std::max(1, x_cnt)), y_cnt(std::max(1, y_cnt)),
          x_pad(std::max(0, x_pad)), y_pad(std::max(0, y_pad)) {}

    void grid_layout::onSizeChange(int oldw, int oldh, int neww, int newh) {
        if (con != nullptr) {
            auto comps = con->getComponents();
            neww = std::max(0, neww - (x_cnt - 1) * x_pad);
            newh = std::max(0, newh - (y_cnt - 1) * y_pad);
            int x = 0, y = 0, max = x_cnt * y_cnt;
            auto begin = comps.begin();
            auto end = std::min(comps.end(), begin + max);
            for (auto i = begin; i < end; ++i) {
                int compx = neww * x / x_cnt, compy = newh * y / y_cnt, // without pad
                    compw = neww * (x + 1) / x_cnt - compx, comph = newh * (y + 1) / y_cnt - compy;
                (**i).setPosition(compx + x_pad * x, compy + y_pad * y);
                (**i).setSize(compw, comph);
                if (++x >= x_cnt) {
                    x = 0;
                    ++y;
                }
            }
        }
    }
}
