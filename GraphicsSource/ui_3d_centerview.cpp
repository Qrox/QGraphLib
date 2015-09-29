#include "ui_3d_centerview.h"

using namespace std;

float constexpr pi = 3.14159265358979323846f;

namespace ui {
    ui_3d_centerview::ui_3d_centerview() : show_axis(DEBUG), auto_repaint(true), eye_dist(1), eye(1, 0, 0), stance(0, 0, 1) {
    }

    ui_3d_centerview::ui_3d_centerview(vec3 const & eye, vec3 const & stance) : show_axis(DEBUG), auto_repaint(true), eye_dist(eye.mod()), eye(eye / eye_dist), stance(eye.cross(stance.cross(eye)).normalize()) {
    }

    void ui_3d_centerview::onMouseMove(int oldx, int oldy, int newx, int newy, vector<vk> const & keystate) {
        if (keystate.empty() && (newx != oldx || newy != oldy)) {
            int w = std::min(getWidth(), getHeight()) / pi / 2;
            vec3 horizon = stance.cross(eye);
            vec3 movement = (horizon * (oldx - newx) + stance * (oldy - newy)) / w;
            vec3 rotation = eye.cross(movement);
            eye = eye.rotate(rotation); //.normalize() if cannot keep unit length
            stance = stance.rotate(rotation); //.normalize() if cannot keep unit length
            //stance = eye.cross(stance.cross(eye)); // if cannot keep perpendicular
            if (auto_repaint) frame::repaint();
        }
    }

    void ui_3d_centerview::paint(opengl & gl) {
        component::paint(gl);
        gl.mode_frustum(eye[0] * eye_dist, eye[1] * eye_dist, eye[2] * eye_dist, 0, 0, 0, stance[0], stance[1], stance[2]);
        if (show_axis) {
            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            glColor3f(0, 1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);
            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
            glEnd();
        }
    }

    void ui_3d_centerview::showAxis(bool show) {
        show_axis = show;
    }

    void ui_3d_centerview::autoRepaint(bool repaint) {
        auto_repaint = repaint;
    }
}
