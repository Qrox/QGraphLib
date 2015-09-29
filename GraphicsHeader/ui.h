#ifndef __QROX_UI_H__
#define __QROX_UI_H__

#include <windows.h>

#include <vector>
#include <map>

#include "graphics.h"
#include "vk.h"
#include "ui_layout.h"

namespace ui {
    class component {
        friend class container;
    private:
        int x, y, w, h;

        virtual bool contains(component const *) const;
    public:
        component();
        virtual ~component() {}

        virtual void paint(opengl & gl);
        virtual void onKeyStroke(int x, int y, std::vector<vk> const & keystroke) {}
        virtual void onKeyDown(int x, int y, std::vector<vk> const & keystroke) {}
        virtual void onKeyUp(int x, int y, std::vector<vk> const & keystroke) {}
        virtual void onMouseMove(int oldx, int oldy, int newx, int newy, std::vector<vk> const & keystate) {}
        virtual void onMouseVScroll(float delta, int x, int y, std::vector<vk> const & keystate) {}
        virtual void onMouseHScroll(float delta, int x, int y, std::vector<vk> const & keystate) {}
        virtual void onSizeChange(int oldw, int oldh, int neww, int newh) {} // fired before change takes effect; do not call setSize() in this function.
        void setPosition(int x, int y);
        int getX() const;
        int getY() const;
        void setSize(int w, int h);
        int getWidth() const;
        int getHeight() const;
    };

    class container : public component {
    private:
        std::vector<component *> components;
        layout * lo;

        virtual bool contains(component const * c) const;
    public:
        container();
        virtual ~container();

        void addComponent(component * comp);
        void removeComponent(component * comp);
        std::vector<component *> const & getComponents() const;

        void setLayout(layout * lo);

        virtual void paint(opengl & gl);
        virtual void onKeyStroke(int x, int y, std::vector<vk> const & keystroke);
        virtual void onKeyDown(int x, int y, std::vector<vk> const & keystroke);
        virtual void onKeyUp(int x, int y, std::vector<vk> const & keystroke);
        virtual void onMouseMove(int oldx, int oldy, int newx, int newy, std::vector<vk> const & keystate);
        virtual void onMouseVScroll(float delta, int x, int y, std::vector<vk> const & keystate);
        virtual void onMouseHScroll(float delta, int x, int y, std::vector<vk> const & keystate);
        virtual void onSizeChange(int oldw, int oldh, int neww, int newh);
    };

    enum class close_operation {
        exit_on_close,
        destroy_on_close,
        hide_on_close,
        do_nothing_on_close,
    };

    class frame {
    private:
        class hwnd_comp {
        public:
            bool operator ()(HWND a, HWND b) const {
                return (ptrdiff_t) a < (ptrdiff_t) b;
            }
        };
        static std::map<HWND, frame *, hwnd_comp> frames;

        static char class_name[];
        static ATOM window_class;
        static component dummy_component;

        static LRESULT CALLBACK MainWndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

        class static_init {
        public:
            static_init() {
                WNDCLASS wndclass = {
                    CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                    MainWndProc,
                    0,
                    0,
                    GetModuleHandle(NULL),
                    NULL,
                    LoadCursor(NULL, IDC_ARROW),
                    NULL,
                    NULL,
                    class_name
                };
                window_class = RegisterClass(&wndclass);
                if (window_class == 0) {
                }
            }

            ~static_init() {
                if (window_class != 0) {
                    UnregisterClass(class_name, GetModuleHandle(NULL));
                }
            }
        };
        static static_init init; // static initialization & finalization

        static void registerWindow(HWND hwnd, frame * frm);
        static void unregisterWindow(HWND hwnd);
        static vk translate_key(WPARAM wparam, LPARAM lparam);

        static thread_local HWND msg_hwnd;

        HWND hwnd;
        opengl gl;
        close_operation do_on_close;
        bool keyrepeat;
        std::vector<vk> keystroke, full_keystroke;
        component * content;

        bool oldxy;
        int oldx, oldy;

        frame(frame const &) = delete;
    public:
        static int startMessageLoop();
        static void repaint();

        frame();
        frame(char const * title);
        ~frame();

        bool isAlive() const;
        void destroy();

        void refresh();
        void setVisible(bool visible);
        void setFocus();
        void setCloseOperation(close_operation op);
        void setPosition(int x, int y);
        void getPosition(int & x, int & y) const;
        void setSize(int w, int h);
        void getSize(int & w, int & h) const;
        void ensureClientSize(int w, int h);
        void setContent(component * content);

        LRESULT fireEvent(UINT msg, WPARAM wparam, LPARAM lparam);
    };
}

#endif
