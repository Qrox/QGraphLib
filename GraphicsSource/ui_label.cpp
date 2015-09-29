#include "ui_label.h"

using namespace std;

namespace ui {
    label::label(string value, float size, h_align ha, v_align va) : content(value), size(size), ha(ha), va(va) {}

    void label::paint(opengl & gl) {
        component::paint(gl);
        float dx, dy;
        switch (ha) {
        case h_align::left:
        default:
            dx = 0;
            break;
        case h_align::center:
            dx = getWidth() * .5f;
            break;
        case h_align::right:
            dx = getWidth();
            break;
        }
        switch (va) {
        case v_align::top:
        default:
            dy = 0;
            break;
        case v_align::center:
            dy = getHeight() * .5f;
            break;
        case v_align::bottom:
            dy = getHeight();
            break;
        }
        gl.pushMatrix();
        gl.translate(dx, dy);
        gl.drawText(content, size, false, ha, va);
        gl.popMatrix();
    }
}
