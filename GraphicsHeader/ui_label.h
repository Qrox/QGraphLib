#ifndef __QROX_UI_LABEL_H__
#define __QROX_UI_LABEL_H__

#include <string>

#include "ui.h"
#include "graphics.h"

namespace ui {
    class label : public component {
    private:
        std::string content;
        float size;
        h_align ha;
        v_align va;

    public:
        label(std::string value, float size, h_align ha = h_align::left, v_align va = v_align::top);

        void paint(opengl & gl);
    };
}

#endif
