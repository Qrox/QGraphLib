#include "vk.h"

using namespace std;

vk::vk() : k(0), l(layout::none) {
}

vk::vk(int k, layout l) : k(k), l(l) {
}

vk::operator int() const {
    return k;
}

vk::operator string() const {
    string str;
    switch (k) {
    case capslock:
        str = "capslock";
        break;
    case pageup:
        str = "pageup";
        break;
    case pagedown:
        str = "pagedown";
        break;
    case end:
        str = "end";
        break;
    case home:
        str = "home";
        break;
    case left:
        str = "left";
        break;
    case right:
        str = "right";
        break;
    case up:
        str = "up";
        break;
    case down:
        str = "down";
        break;
    case printscr:
        str = "printscr";
        break;
    case ins:
        str = "ins";
        break;
    case meta:
#if WIN32
        str = "windows";
#else
        str = "meta";
#endif
        break;
    case numlock:
        str = "numlock";
        break;
    case scrolllock:
        str = "scrolllock";
        break;
    case shift:
        str = "shift";
        break;
    case ctrl:
        str = "ctrl";
        break;
    case alt:
        str = "alt";
        break;
    case mouseleft:
        str = "lmb";
        break;
    case mousemiddle:
        str = "mmb";
        break;
    case mouseright:
        str = "rmb";
        break;
    case back:
        str = "backspace";
        break;
    case tab:
        str = "tab";
        break;
    case enter:
        str = "enter";
        break;
    case esc:
        str = "esc";
        break;
    case space:
        str = "space";
        break;
    case del:
        str = "delete";
        break;
    default:
        if (k >= 0 && k < 256 && isprint(k) && !isspace(k)) {
            str = ((char) k);
        } else {
            str = "?";
        }
    }
    switch (l) {
    case layout::none:
        break;
    case layout::left:
        str.insert(0, "left ");
        break;
    case layout::right:
        str.insert(0, "right ");
        break;
    case layout::numpad:
        str.insert(0, "numpad ");
        break;
    }
    return str;
}

bool vk::operator ==(vk o) {
    return k == o.k && l == o.l;
}

bool vk::operator ==(int ok) {
    return k == ok;
}

bool vk::operator &(layout ol) {
    return l == ol;
}
