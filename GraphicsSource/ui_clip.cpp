#include "ui_clip.h"
#include "ui.h"

using namespace std;

namespace ui {
    bool clip::apply(int byte) {
        if (clips.empty()) return false;
        rect_trans & r = clips.back();
        if (r.x1 <= r.x0 || r.y1 <= r.y0) return false;
        glStencilFunc(GL_ALWAYS, byte, 0xFF);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        glColorMask(0, 0, 0, 0);
        glDepthMask(0);
        glStencilMask(0xFF);
        glBegin(GL_QUADS);
        glVertex2f(r.x0, r.y0);
        glVertex2f(r.x0, r.y1);
        glVertex2f(r.x1, r.y1);
        glVertex2f(r.x1, r.y0);
        glEnd();
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glColorMask(1, 1, 1, 1);
        glDepthMask(1);
        glStencilMask(0);
        return true;
    }

    void clip::clear() {
        clips.clear();
    }

    bool clip::push(rect const & _r) {
        rect_trans r;
        if (clips.empty()) {
            r.tx = _r.x;
            r.ty = _r.y;
            r.x0 = 0;
            r.y0 = 0;
            r.x1 = _r.w;
            r.y1 = _r.h;
        } else {
            apply(0);
            rect_trans & l = clips.back();
            r.tx = _r.x;
            r.ty = _r.y;
            r.x0 = max(0, -_r.x);
            r.y0 = max(0, -_r.y);
            r.x1 = min(_r.w, l.x1 - l.x0 - _r.x);
            r.y1 = min(_r.h, l.y1 - l.y0 - _r.y);
        }
        clips.push_back(r);
        glPushMatrix();
        glTranslatef(r.tx, r.ty, 0);
        return apply(0xFF);
    }

    bool clip::push(component const & c) {
        rect r = {
            c.getX(),
            c.getY(),
            c.getWidth(),
            c.getHeight(),
        };
        return push(r);
    }

    void clip::pop() {
        if (!clips.empty()) {
            glPopMatrix();
            glPushMatrix();
            rect_trans const & r = clips.back();
            glTranslatef(r.tx, r.ty, 0);
            apply(0);
            glPopMatrix();
            clips.pop_back();   // r now hanging. do not access after this line.
            apply(0xFF);
        }
    }
}
