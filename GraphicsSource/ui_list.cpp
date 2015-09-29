#include "ui_list.h"

#undef min
#undef max
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace ui {
    void static_string_list::init() {
        int j = 0;
        for (auto i = items.begin(); i < items.end(); ++i, ++j) {
            if (i->enabled) {
                cur = j;
                break;
            }
        }
    }

    static_string_list::static_string_list(int item_height, font const & fnt, tcallback callback, initializer_list<item> lst) :
            items(lst), multiple_selection(false), return_action(action::cancel), cur(-1), tx(0), ty(0), item_height(item_height), fnt(fnt), callback(callback) {
        init();
    }

    static_string_list::static_string_list(int item_height, font const & fnt, tcallback callback, vector<item> & vec) :
            items(vec), multiple_selection(false), return_action(action::cancel), cur(-1), tx(0), ty(0), item_height(item_height), fnt(fnt), callback(callback) {
        init();
    }

    void static_string_list::enableMultipleSelection(bool mul) {
        multiple_selection = mul;
    }

    bool static_string_list::isSelected() const {
        for (auto i = items.begin(); i < items.end(); ++i) {
            if (i->enabled && i->selected) {
                return true;
            }
        }
        return false;
    }

    int static_string_list::getFirstSelection() const {
        int j = 0;
        for (auto i = items.begin(); i < items.end(); ++i, ++j) {
            if (i->enabled && i->selected) {
                return j;
            }
        }
        return -1;
    }

    void static_string_list::getSelection(vector<int> & vec) const {
        int j = 0;
        for (auto i = items.begin(); i < items.end(); ++i, ++j) {
            if (i->enabled && i->selected) {
                vec.push_back(j);
                if (!multiple_selection) {
                    break;
                }
            }
        }
    }

    static_string_list::action static_string_list::getAction() const {
        return return_action;
    }

    void static_string_list::clearSelection() {
        for (auto i = items.begin(); i < items.end(); ++i) {
            i->selected = false;
        }
    }

    void static_string_list::setSelection(int i, bool v) {
        if (i >= 0 && i < int(items.size())) {
            items[i].selected = v;
        }
    }

    void static_string_list::paint(opengl & gl) {
        static unsigned char constexpr color_table[][6] {
            85, 85, 85, 0, 0, 0,    // !cur && !enabled
            170, 170, 170, 0, 0, 0, // !cur && !sel
            255, 255, 255, 0, 0, 0, // !cur && sel
            0, 0, 0, 85, 85, 85,    // cur && !enabled
            0, 0, 0, 170, 170, 170, // cur && !sel
            0, 0, 0, 255, 255, 255, // cur && sel
        };

        component::paint(gl);
        int start = ty / item_height, end = min((ty + getHeight() - 1) / item_height, (int) items.size() - 1);
        int first_selection = getFirstSelection();
        gl.pushMatrix();
        gl.translate(-tx, start * item_height - ty);
        for (int j = start; j <= end; ++j) {
            item & i = items[j];

            if (j != cur) {
                if (!i.enabled) {
                    gl.setColor(color_table[0][3], color_table[0][4], color_table[0][5]);
                } else if (!i.selected || (!multiple_selection && first_selection >= 0 && j != first_selection)) {
                    gl.setColor(color_table[1][3], color_table[1][4], color_table[1][5]);
                } else {
                    gl.setColor(color_table[2][3], color_table[2][4], color_table[2][5]);
                }
            } else {
                if (!i.enabled) {
                    gl.setColor(color_table[3][3], color_table[3][4], color_table[3][5]);
                } else if (!i.selected || (!multiple_selection && first_selection >= 0 && j != first_selection)) {
                    gl.setColor(color_table[4][3], color_table[4][4], color_table[4][5]);
                } else {
                    gl.setColor(color_table[5][3], color_table[5][4], color_table[5][5]);
                }
            }
            gl.fillRect(0, 0, getWidth(), item_height);

            if (j != cur) {
                if (!i.enabled) {
                    gl.setColor(color_table[0][0], color_table[0][1], color_table[0][2]);
                } else if (!i.selected || (!multiple_selection && first_selection >= 0 && j != first_selection)) {
                    gl.setColor(color_table[1][0], color_table[1][1], color_table[1][2]);
                } else {
                    gl.setColor(color_table[2][0], color_table[2][1], color_table[2][2]);
                }
            } else {
                if (!i.enabled) {
                    gl.setColor(color_table[3][0], color_table[3][1], color_table[3][2]);
                } else if (!i.selected || (!multiple_selection && first_selection >= 0 && j != first_selection)) {
                    gl.setColor(color_table[4][0], color_table[4][1], color_table[4][2]);
                } else {
                    gl.setColor(color_table[5][0], color_table[5][1], color_table[5][2]);
                }
            }
            gl.setFont(fnt);
            gl.drawText(i.text, item_height, false, h_align::left, v_align::top);
            gl.translate(0, item_height);
        }
        gl.popMatrix();
    }

    void static_string_list::onKeyDown(int x, int y, vector<vk> const & keystroke) {
    #if defined(DEBUG) && false
        for (auto i = keystroke.begin(); i < keystroke.end(); ++i) {
            cout << string(*i);
            if (i + 1 < keystroke.end()) {
                cout << " ";
            }
        }
        cout << endl;
    #endif
        bool repaint = false;
        if (keystroke.size() == 1) {
            int ch = keystroke.back();
            repaint = true;
            switch (ch) {
            case vk::up: {
                bool moved = false;
                if (cur >= 0) { // selection enabled
                    int min = max(0, (ty - 1) / item_height);
                    for (auto i = cur - 1; i >= min; --i) { // from cursor (excl.) to component top (incl.)
//                        if (items[i].enabled) {
                            cur = i;
                            moved = true;
                            break;
//                        }
                    }
                }
                if (moved) {
                    if (cur == (ty - 1) / item_height) {
                        ty = cur * item_height;
                    }
                } else {
                    ty = max(0, ty - item_height);
                }
            }   break;
            case vk::down: {
                int h = getHeight(), size = items.size();
                bool moved = false;
                if (cur >= 0) { // selection enabled
                    int max = min((ty + h) / item_height, size - 1);
                    for (auto i = cur + 1; i <= max; ++i) { // from cursor (excl.) to component bottom (incl.)
//                        if (items[i].enabled) {
                            cur = i;
                            moved = true;
                            break;
//                        }
                    }
                }
                if (moved) {
                    if (cur == (ty + h) / item_height) {
                        ty = (cur + 1) * item_height - h;
                    }
                } else {
                    ty = max(ty, min(ty + item_height, max(0, size * item_height - h)));
                }
            }   break;
            case vk::space:
                if (cur >= 0 && cur < (int) items.size() && items[cur].enabled) {
                    if (multiple_selection) {
                        items[cur].selected = !items[cur].selected;
                    } else {
                        for (auto i = items.begin(); i < items.end(); ++i) {
                            i->selected = false;
                        }
                        items[cur].selected = true;
                        return_action = action::apply;
                        //todo: close & return
                        if (callback) callback(*this);
                    }
                }
                break;
            case vk::mouseleft: {
                int cur = (ty + y) / item_height;
                if (cur >= 0 && cur < (int) items.size() && items[cur].enabled) {
                    if (multiple_selection) {
                        items[cur].selected = !items[cur].selected;
                    } else {
                        for (auto i = items.begin(); i < items.end(); ++i) {
                            i->selected = false;
                        }
                        items[cur].selected = true;
                        return_action = action::apply;
                        //todo: close & return
                        if (callback) callback(*this);
                    }
                }
            }   break;
            case vk::enter:
                if (cur >= 0 && cur < (int) items.size() && items[cur].enabled) {
                    if (!multiple_selection) {
                        for (auto i = items.begin(); i < items.end(); ++i) {
                            i->selected = false;
                        }
                        items[cur].selected = true;
                    }
                    return_action = action::apply;
                    //todo: close & return
                    if (callback) callback(*this);
                }
                break;
            case vk::esc:
                return_action = action::cancel;
                //todo: close & return;
                if (callback) callback(*this);
                break;
            default:
                repaint = false;
                break;
            }
        }
        if (repaint) {
            frame::repaint();
        }
    }

    void static_string_list::onMouseMove(int oldx, int oldy, int newx, int newy, vector<vk> const & keystate) {
        int cur = (ty + newy) / item_height;
        if (cur >= 0 && cur < (int) items.size()
//                && items[cur].enabled
                && this->cur != cur) {
            this->cur = cur;
            frame::repaint();
        }
    }

    void static_string_list::onMouseVScroll(float delta, int x, int y, vector<vk> const & keystate) {
        int dy = -delta * 10;
        int comp_height = getHeight();
        int full_height = items.size() * item_height;
        if (dy > 0) {
            if (ty + comp_height + dy <= full_height) {
                ty += dy;
            } else if (comp_height < full_height) {
                ty = full_height - comp_height;
            }
        } else if (dy < 0) {
            if (ty + dy >= 0) {
                ty += dy;
            } else {
                ty = 0;
            }
        } else return;
        frame::repaint();
    }

    void static_string_list::onSizeChange(int oldw, int oldh, int neww, int newh) {
        if ((cur + 1) * item_height - ty >= newh) {
            ty = (cur + 1) * item_height - newh;
        }
    }
}
