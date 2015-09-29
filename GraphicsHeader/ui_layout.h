#ifndef __QROX_UI_LAYOUT_H__
#define __QROX_UI_LAYOUT_H__

namespace ui {
    class layout {
        friend class container;
    protected:
        class container * con;
    public:
        layout();
        virtual ~layout() {}

        virtual void onSizeChange(int oldw, int oldh, int neww, int newh) {};
    };

    class grid_layout : public layout {
    private:
        int x_cnt, y_cnt, x_pad, y_pad;
    public:
        virtual ~grid_layout() {}

        grid_layout(int x_cnt, int y_cnt, int x_pad = 1, int y_pad = 1);

        virtual void onSizeChange(int oldw, int oldh, int neww, int newh);
    };
}

#endif
