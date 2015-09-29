#ifndef __QROX_VK_H__
#define __QROX_VK_H__

#include <string>

class vk {
public:
    enum key : int {
        capslock = -0x40000000,
        pageup,
        pagedown,
        end,
        home,
        left,
        up,
        right,
        down,
        printscr,
        ins,
        meta,
        //function keys
        numlock,
        scrolllock,
        shift,
        ctrl,
        alt,
        mouseleft,
        mousemiddle,
        mouseright,

        back = 8,
        tab = 9,
        enter = 13,
        esc = 27,
        space = 32,
        del = 127,
    };

    enum class layout {
        none,
        left,
        right,
        numpad,
    };

private:
    int k;
    layout l;

public:
    vk();
    vk(int k, layout l = layout::none);
    operator int() const;
    operator std::string() const;
    bool operator ==(vk o);
    bool operator ==(int ok);
    bool operator &(layout ol);
};

#endif
