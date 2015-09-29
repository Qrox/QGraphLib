#ifndef __QROX_UI_3D_CENTERVIEW_H__
#define __QROX_UI_3D_CENTERVIEW_H__

#include "ui.h"
#include "vec.h"

namespace ui {
    class ui_3d_centerview : public component {
    protected:
        typedef vec<float, 3> vec3;

        bool show_axis, auto_repaint;
        float eye_dist;
        vec3 eye, stance;
    public:
        ui_3d_centerview();
        ui_3d_centerview(vec3 const & eye, vec3 const & stance);
        void onMouseMove(int oldx, int oldy, int newx, int newy, std::vector<vk> const & keystate);
        void paint(opengl & gl);

        void showAxis(bool show);
        void autoRepaint(bool repaint);
    };
}

#endif
