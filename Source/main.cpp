#ifdef TEST

#include <sstream>

#include "graphics.h"
#include "ui.h"
#include "ui_3d_centerview.h"
#include "fps.h"

using namespace ui;
using namespace std;

class test_3d : public ui_3d_centerview {
private:
    fps _fps;

    bool reload_shader;
public:
    test_3d() : ui_3d_centerview(vec3(1, 1, 1).normalize(), vec3(0, 0, 1)), reload_shader(false) {
    }

    void paint(opengl & gl) {
        if (reload_shader) {
            reload_shader = false;
            gl.reload_shader();
        }

        ui_3d_centerview::paint(gl);

        {
            float constexpr size = .2;
            string a = "Hippopotomonstrose", b = "squippedaliophobia";
            float wa = 0, wb = 0;
            gl.getTextExtent(a, size, &wa, nullptr, nullptr);
            gl.getTextExtent(b, size, &wb, nullptr, nullptr);
            gl.setColor(255, 255, 255);
            gl.pushMatrix();
            gl.coordinate(vec3(wb / 2, 0, 0), vec3(0, 1, 0), vec3(0, 0, -1), vec3(-1, 0, 0));
            gl.drawText(a, size, false, h_align::center, v_align::center);
            gl.popMatrix();

            gl.pushMatrix();
            gl.coordinate(vec3(0, wa / 2, 0), vec3(-1, 0, 0), vec3(0, 0, -1), vec3(0, -1, 0));
            gl.drawText(b, size, false, h_align::center, v_align::center);
            gl.popMatrix();
        }

        gl.mode_2d();
        stringstream str;
        str << (float) _fps;
        string str_fps = str.str();
        float w = 0;
        const float size = 20;
        gl.getTextExtent(str_fps, size, &w, nullptr, nullptr);
        gl.setColor(0, 255, 255);
        gl.fillRect(0, 0, w, 20);
        gl.setColor(255, 255, 255);
        gl.drawText(str.str(), size, true, h_align::left, v_align::top);
        ++_fps;
    }

    void onKeyDown(int x, int y, vector<vk> const & keystroke) {
        if (keystroke.size() == 1) {
            int ch = keystroke[0];
            switch (ch) {
            case vk::esc:
                reload_shader = true;
                frame::repaint();
                break;
            }
        }
    }
};

int main(void) {
    frame main_frame("window");
    main_frame.ensureClientSize(800, 600);

    test_3d cp;
    main_frame.setContent(&cp);

    main_frame.setVisible(true);
    main_frame.setFocus();

    return frame::startMessageLoop();
}

#endif
