#include <windowsx.h>

#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <ctime>
#include "profiler.h"
#endif

#include "ui.h"

using namespace std;

namespace ui {
    bool component::contains(component const *) const {
        return false;
    }

    component::component() : x(0), y(0), w(0), h(0) {
    }

    void component::paint(opengl & gl) {
        gl.setColor(0, 0, 0);
        gl.fillRect(0, 0, w, h);
    }

    void component::setPosition(int x, int y) {
        this->x = x;
        this->y = y;
        frame::repaint();
    }

    int component::getX() const {
        return x;
    }

    int component::getY() const {
        return y;
    }

    void component::setSize(int w, int h) {
        w = std::max(0, w);
        h = std::max(0, h);
        if (w != this->w || h != this->h) {
            onSizeChange(this->w, this->h, w, h);
            this->w = w;
            this->h = h;
            frame::repaint();
        }
    }

    int component::getWidth() const {
        return w;
    }

    int component::getHeight() const {
        return h;
    }

    container::container() : lo(nullptr) {
    }

    container::~container() {
        setLayout(nullptr);
    }

    bool container::contains(component const * c) const {
        for (auto i = components.begin(); i < components.end(); ++i) {
            if (*i == c) {
                return true;
            }
        }
        for (auto i = components.begin(); i < components.end(); ++i) {
            if ((*i)->contains(c)) {
                return true;
            }
        }
        return false;
    }

    void container::addComponent(component * comp) {
        if (comp != this && !comp->contains(this)) {
            components.push_back(comp);
        }
    }

    void container::removeComponent(component * comp) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            if (*i == comp) {
                components.erase(i);
                return;
            }
        }
    }

    vector<component *> const & container::getComponents() const {
        return components;
    }

    void container::setLayout(layout * newlo) {
        if (lo != nullptr && lo->con == this) lo->con = nullptr;
        if (newlo != nullptr) {
            if (newlo->con != nullptr && newlo->con != this) {
                newlo->con->setLayout(nullptr);
            }
            lo = newlo;
            newlo->con = this;
            int w = getWidth(), h = getHeight();
            if (w != 0 || h != 0) newlo->onSizeChange(0, 0, w, h);
        } else lo = nullptr;
    }

    void container::paint(opengl & gl) {
        component::paint(gl);
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            if (gl.clips().push(comp))
                comp.paint(gl);
            gl.clips().pop();
        }
    }

    void container::onKeyStroke(int x, int y, std::vector<vk> const & keystroke) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (x > comp_x && y > comp_y) {
                int x0 = x - comp_x, y0 = y - comp_y;
                if (x0 < comp_w && y0 < comp_h) {
                    comp.onKeyStroke(x0, y0, keystroke);
                }
            }
        }
    }

    void container::onKeyDown(int x, int y, std::vector<vk> const & keystroke) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (x > comp_x && y > comp_y) {
                int x0 = x - comp_x, y0 = y - comp_y;
                if (x0 < comp_w && y0 < comp_h) {
                    comp.onKeyDown(x0, y0, keystroke);
                }
            }
        }
    }

    void container::onKeyUp(int x, int y, std::vector<vk> const & keystroke) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (x > comp_x && y > comp_y) {
                int x0 = x - comp_x, y0 = y - comp_y;
                if (x0 < comp_w && y0 < comp_h) {
                    comp.onKeyUp(x0, y0, keystroke);
                }
            }
        }
    }

    void container::onMouseMove(int oldx, int oldy, int newx, int newy, std::vector<vk> const & keystate) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (newx > comp_x && newy > comp_y) {
                int newx0 = newx - comp_x, newy0 = newy - comp_y;
                if (newx0 < comp_w && newy0 < comp_h) {
                    comp.onMouseMove(oldx - comp_x, oldy - comp_y, newx0, newy0, keystate);
                }
            }
        }
    }

    void container::onMouseVScroll(float delta, int x, int y, std::vector<vk> const & keystate) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (x > comp_x && y > comp_y) {
                int x0 = x - comp_x, y0 = y - comp_y;
                if (x0 < comp_w && y0 < comp_h) {
                    comp.onMouseVScroll(delta, x0, y0, keystate);
                }
            }
        }
    }

    void container::onMouseHScroll(float delta, int x, int y, std::vector<vk> const & keystate) {
        for (auto i = components.begin(); i < components.end(); ++i) {
            component & comp = **i;
            int comp_x = comp.getX(), comp_y = comp.getY(),
                comp_w = comp.getWidth(), comp_h = comp.getHeight();
            if (x > comp_x && y > comp_y) {
                int x0 = x - comp_x, y0 = y - comp_y;
                if (x0 < comp_w && y0 < comp_h) {
                    comp.onMouseHScroll(delta, x0, y0, keystate);
                }
            }
        }
    }

    void container::onSizeChange(int oldw, int oldh, int neww, int newh) {
        if (lo != nullptr) lo->onSizeChange(oldw, oldh, neww, newh);
    }

    map<HWND, frame *, frame::hwnd_comp> frame::frames;
    char frame::class_name[] = "QROX_FRAME";
    ATOM frame::window_class;
    component frame::dummy_component;
    frame::static_init frame::init;
    thread_local HWND frame::msg_hwnd = NULL;

    int frame::startMessageLoop() {
       MSG msg;
       int go_on;
       do {
           go_on = GetMessage(&msg, (HWND) NULL, 0, 0);
           if (go_on >= 0) { //no error
               TranslateMessage(&msg);
               DispatchMessage(&msg);
           }
       } while (go_on); //not WM_QUIT
       return msg.wParam;
    }

    void frame::repaint() {
        if (msg_hwnd) {
            InvalidateRect(msg_hwnd, NULL, FALSE);
        }
    }

    LRESULT CALLBACK frame::MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        msg_hwnd = hwnd;
        auto it = frames.find(hwnd);
        LRESULT lresult;
        if (it != frames.end() && it->second) {
            lresult = it->second->fireEvent(msg, wparam, lparam);
        } else {
            lresult = DefWindowProc(hwnd, msg, wparam, lparam);
        }
        msg_hwnd = NULL;
        return lresult;
    }

    void frame::registerWindow(HWND hwnd, frame * frm) {
        frames.insert(pair<HWND, frame *>(hwnd, frm));
    }

    void frame::unregisterWindow(HWND hwnd) {
        frames.erase(hwnd);
    }

    frame::frame() : frame("") {}

    frame::frame(char const * title) :
        hwnd(CreateWindow(
            class_name,
            title,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            (HWND) NULL,
            (HMENU) NULL,
            GetModuleHandle(NULL),
            NULL
        )), gl(hwnd), do_on_close(close_operation::exit_on_close), keyrepeat(false), content(&dummy_component), oldxy(false), oldx(0), oldy(0) {
        if (isAlive()) {
            registerWindow(hwnd, this);
        }
    }

    frame::~frame() {
        destroy();
    }

    bool frame::isAlive() const {
        return hwnd != NULL;
    }

    void frame::destroy() {
        if (isAlive()) {
            DestroyWindow(hwnd);
        }
    }

    void frame::refresh() {
        if (isAlive()) {
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    void frame::setVisible(bool visible) {
        if (isAlive()) {
            if (visible) {
                ShowWindow(hwnd, SW_SHOWDEFAULT);
                UpdateWindow(hwnd);
            } else {
                ShowWindow(hwnd, SW_HIDE);
            }
        }
    }

    void frame::setFocus() {
        if (isAlive()) {
            SetFocus(hwnd);
        }
    }

    void frame::setCloseOperation(close_operation op) {
        do_on_close = op;
    }

    void frame::setPosition(int x, int y) {
        if (isAlive()) {
            SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    void frame::getPosition(int & x, int & y) const {
        if (isAlive()) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            x = rect.left;
            y = rect.top;
        }
    }

    void frame::setSize(int w, int h) {
        if (isAlive()) {
            SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    void frame::getSize(int & w, int & h) const {
        if (isAlive()) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            w = rect.right - rect.left;
            h = rect.bottom - rect.top;
        }
    }

    void frame::ensureClientSize(int w, int h) {
        if (isAlive()) {
            RECT window, client;
            GetWindowRect(hwnd, &window);
            GetClientRect(hwnd, &client);
            setSize(w + (window.right - window.left) - (client.right - client.left),
                    h + (window.bottom - window.top) - (client.bottom - client.top));
        }
    }

    void frame::setContent(component * c) {
        if (isAlive()) {
            if (c != nullptr) {
                content = c;
                c->setPosition(0, 0);
                RECT rect;
                GetClientRect(hwnd, &rect);
                c->setSize(rect.right - rect.left, rect.bottom - rect.top);
            } else {
                content = &dummy_component;
            }
        }
    }

    vk frame::translate_key(WPARAM wparam, LPARAM lparam) {
        wparam = MapVirtualKey((lparam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX);
        if (('0' <= wparam && wparam <= '9')
                || ('A' <= wparam && wparam <= 'Z')) {
            return vk((int) wparam);
        } else if (VK_NUMPAD0 <= wparam && wparam <= VK_NUMPAD9) {
            return vk((int) wparam - VK_NUMPAD0 + '0', vk::layout::numpad);
        }
        switch (wparam) {
        case VK_BACK:
            return vk(vk::back);
        case VK_TAB:
            return vk(vk::tab);
        case VK_RETURN:
            return vk(vk::enter);
        case VK_CAPITAL:
            return vk(vk::capslock);
        case VK_ESCAPE:
            return vk(vk::esc);
        case VK_SPACE:
            return vk(' ');
        case VK_PRIOR:
            return vk(vk::pageup);
        case VK_NEXT:
            return vk(vk::pagedown);
        case VK_END:
            return vk(vk::end);
        case VK_HOME:
            return vk(vk::home);
        case VK_LEFT:
            return vk(vk::left);
        case VK_UP:
            return vk(vk::up);
        case VK_RIGHT:
            return vk(vk::right);
        case VK_DOWN:
            return vk(vk::down);
        case VK_SNAPSHOT:
            return vk(vk::printscr);
        case VK_INSERT:
            return vk(vk::ins);
        case VK_DELETE:
            return vk(vk::del);
        case VK_LWIN:
            return vk(vk::meta, vk::layout::left);
        case VK_RWIN:
            return vk(vk::meta, vk::layout::right);
    //  case VK_F*: // function keys
        case VK_NUMLOCK:
            return vk(vk::numlock);
        case VK_SCROLL:
            return vk(vk::scrolllock);
        case VK_LSHIFT:
            return vk(vk::shift, vk::layout::left);
        case VK_RSHIFT:
            return vk(vk::shift, vk::layout::right);
        case VK_LCONTROL:
            return vk(vk::ctrl, vk::layout::left);
        case VK_RCONTROL:
            return vk(vk::ctrl, vk::layout::right);
        case VK_LMENU:
            return vk(vk::alt, vk::layout::left);
        case VK_RMENU:
            return vk(vk::alt, vk::layout::right);
        case VK_OEM_1:
            return vk(';');
        case VK_OEM_PLUS:
            return vk('=');
        case VK_OEM_COMMA:
            return vk(',');
        case VK_OEM_MINUS:
            return vk('-');
        case VK_OEM_PERIOD:
            return vk('.');
        case VK_OEM_2:
            return vk('/');
        case VK_OEM_3:
            return vk('`');
        case VK_OEM_4:
            return vk('[');
        case VK_OEM_5:
            return vk('\\');
        case VK_OEM_6:
            return vk(']');
        case VK_OEM_7:
            return vk('\'');
    //  case VK_OEM_8:
        default:
            return vk(0);
        }
    }

#ifdef DEBUG
#define PROFILE false
#endif

    LRESULT frame::fireEvent(UINT msg, WPARAM wparam, LPARAM lparam) {
        if (isAlive()) {
#if PROFILE
            profiler pro;
#endif
            switch (msg) {
            case WM_CREATE:
                break;
            case WM_CLOSE:
                switch (do_on_close) {
                case close_operation::exit_on_close:
                case close_operation::destroy_on_close:
                    destroy();
                    break;
                case close_operation::hide_on_close:
                    setVisible(false);
                    break;
                case close_operation::do_nothing_on_close:
                default:
                    break;
                }
                break;
            case WM_DESTROY:
                switch (do_on_close) {
                case close_operation::exit_on_close:
                default:
                    PostQuitMessage(0);
                    break;
                case close_operation::destroy_on_close:
                    break;
                }
                unregisterWindow(hwnd);
                hwnd = 0;
                break;
            // WM_QUIT is never receive by a window
            case WM_SIZE:
                RECT rect;
                if (GetClientRect(hwnd, &rect)) {
                    int w = rect.right - rect.left;
                    int h = rect.bottom - rect. top;
                    gl.resize(w, h);
                    content->setSize(w, h);
                }
                break;
            case WM_PAINT: {
                PAINTSTRUCT ps;
                /*HDC hdc = */BeginPaint(hwnd, &ps);
                glPushAttrib(GL_ALL_ATTRIB_BITS);
                gl.clear();
                gl.mode_2d();
                if (gl.clips().push(*content))
                    content->paint(gl);
                gl.clips().pop();
                glPopAttrib();
                SwapBuffers(gl.hdc);
                EndPaint(hwnd, &ps);
            }   break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN: {
                int constexpr previously_down = 0x40000000;
                vk key;
                switch (msg) {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                    key = translate_key(wparam, lparam);
                    break;
                case WM_LBUTTONDOWN:
                    key = vk(vk::mouseleft);
                    break;
                case WM_MBUTTONDOWN:
                    key = vk(vk::mousemiddle);
                    break;
                case WM_RBUTTONDOWN:
                    key = vk(vk::mouseright);
                    break;
                default:
                    key = vk(0);
                }
                if (key) {
                    int x, y;
                    if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
                        x = oldx;
                        y = oldy;
                    } else {
                        x = GET_X_LPARAM(lparam);
                        y = GET_Y_LPARAM(lparam);
                    }
                    if ((msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) && (lparam & previously_down)) { // repeating
                        if (keystroke.back() == key) {
//                            int repeat_count = lparam & 0xFFFF;
                            full_keystroke.clear();
                            keyrepeat = true;
                            content->onKeyDown(x, y, keystroke);
                            content->onKeyStroke(x, y, keystroke);
                        }
                    } else {
                        bool contains = false;
                        for (auto i = keystroke.begin(); i < keystroke.end(); ++i) {
                            if (*i == key) {
                                contains = true;
                                break;
                            }
                        }
                        if (!contains) {
                            keystroke.push_back(key);
                            keyrepeat = false;
                            if (!full_keystroke.empty()) {
                                content->onKeyStroke(x, y, full_keystroke);
                                full_keystroke.clear();
                            }
#ifdef DEBUG
                            if (keystroke.size() == 3 &&
                                keystroke[0] == vk::ctrl &&
                                keystroke[1] == vk::alt &&
                                keystroke[2] == vk::esc) {
                                gl.reload_shader();
                            }
#endif
                            content->onKeyDown(x, y, keystroke);
                        }
                    }
                }
            }   break;
            case WM_KEYUP:
            case WM_SYSKEYUP:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP: {
                vk key;
                switch (msg) {
                case WM_KEYUP:
                case WM_SYSKEYUP:
                    key = translate_key(wparam, lparam);
                    break;
                case WM_LBUTTONUP:
                    key = vk(vk::mouseleft);
                    break;
                case WM_MBUTTONUP:
                    key = vk(vk::mousemiddle);
                    break;
                case WM_RBUTTONUP:
                    key = vk(vk::mouseright);
                    break;
                default:
                    key = vk(0);
                    break;
                }
                if (key) {
                    int x, y;
                    if (msg == WM_KEYUP || msg == WM_SYSKEYUP) {
                        x = oldx;
                        y = oldy;
                    } else {
                        x = GET_X_LPARAM(lparam);
                        y = GET_Y_LPARAM(lparam);
                    }
                    for (auto i = keystroke.begin(); i < keystroke.end(); ++i) {
                        if (*i == key) {
                            if (!keyrepeat && full_keystroke.empty()) {
                                full_keystroke = keystroke;
                            }
                            content->onKeyUp(x, y, keystroke);
                            keystroke.erase(i);
                            keyrepeat = false;
                            if (keystroke.empty()) {
                                content->onKeyStroke(x, y, full_keystroke);
                                full_keystroke.clear();
                            }
                            break;
                        }
                    }
                }
            }   break;
            case WM_MOUSEMOVE: {
                if (!oldxy) {
                    oldx = GET_X_LPARAM(lparam);
                    oldy = GET_Y_LPARAM(lparam);
                    oldxy = true;
                }
                int newx = GET_X_LPARAM(lparam), newy = GET_Y_LPARAM(lparam);
                content->onMouseMove(oldx, oldy, newx, newy, keystroke);
                oldx = newx; oldy = newy;
            }   break;
            case WM_MOUSEWHEEL: {
                content->onMouseVScroll((float) GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), keystroke);
            }   break;
            case 0x020E: { // WM_MOUSEHWHEEL
                content->onMouseHScroll((float) GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), keystroke);
            }   break;
            case WM_KILLFOCUS:
                keystroke.clear();
                full_keystroke.clear();
                keyrepeat = false;
                break;
            default:
                return DefWindowProc(hwnd, msg, wparam, lparam);
                break;
            }
#if PROFILE
            cout << "msg " << msg << " used " << pro << "s" << endl;
#endif
        }
        return 0;
    }
}
