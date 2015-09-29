#ifndef __QROX_UI_LIST_H__
#define __QROX_UI_LIST_H__

#include <string>
#include <vector>
#include <initializer_list>
#include <functional>

#include "ui.h"

namespace ui {
    class static_string_list : public component {
    public:
        struct item {
            std::string text;
            bool enabled;
            bool selected;
        };

        enum class action {
            none,
            cancel,
            apply,
        };
    private:
        typedef std::function<void (static_string_list &)> tcallback;

        std::vector<item> items;
        bool multiple_selection;
        action return_action;
        int cur, tx, ty;
        int item_height;
        font fnt;
        tcallback callback;
        void init();
    public:
        static_string_list(int item_height, font const & fnt, tcallback callback, std::initializer_list<item> lst);
        static_string_list(int item_height, font const & fnt, tcallback callback, std::vector<item> & vec);
        virtual ~static_string_list() {}

        void enableMultipleSelection(bool mul);
        bool isSelected() const;
        int getFirstSelection() const;
        void getSelection(std::vector<int> & vec) const;
        action getAction() const;

        void clearSelection();
        void setSelection(int i, bool v);

        void paint(opengl & gl);
        void onKeyDown(int x, int y, std::vector<vk> const & keystroke);
        void onMouseMove(int oldx, int oldy, int newx, int newy, std::vector<vk> const & keystate);
        void onMouseVScroll(float delta, int x, int y, std::vector<vk> const & keystate);
        void onSizeChange(int oldw, int oldh, int neww, int newh);
    };
}

#endif
