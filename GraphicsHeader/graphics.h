#ifndef __QROX_GRAPHICS_H__
#define __QROX_GRAPHICS_H__

#include <windows.h>
#include <gl/gl.h>
#include <set>

#include "vec.h"
#include "font.h"
#include "shader.h"
#include "objectcache.h"
#include "ui_clip.h"

namespace ui {
    class opengl {
        friend class frame;
    private:
        typedef vec<float, 3> vec3;

        static PIXELFORMATDESCRIPTOR const default_pfd;
        static MAT2 const identity_mat2;

        HWND hwnd;
        HDC hdc;
        HGLRC hrc;
        OUTLINETEXTMETRIC inotm, emotm;

        GLint viewport[4];
        GLdouble matrix_proj_2d[16], matrix_proj_3d_frustum[16], matrix_proj_3d_ortho[16];

        enum {
            uninitialized = 0,
            _2d,
            frustum,
            ortho,
        } mode = uninitialized;

        struct font_outline_info {
            GLuint disp_list;
            DWORD data_size;
            GLYPHMETRICS gm;
        };
        static int constexpr font_outline_buf_size = 16384;

        font infont, emfont;
        shader font_vs, font_fs;
        shaderprogram font_sp;
        GLint font_outline, font_coord, font_i32cnt, font_emsize, font_mscreen, font_mtextomdl;
        objectcache<int, font_outline_info> font_outline_cache;
        clip clp;

        opengl(opengl const &) = delete;
        opengl(HWND hwnd);
        ~opengl();

        void resize(int w, int h);
    public:
        static float constexpr pi = 3.1415926535897932384626433832795f;

        void reload_shader();

        void makeCurrent();
        void mode_2d();
        void mode_frustum(float eyex, float eyey, float eyez, float sightx, float sighty, float sightz, float stancex, float stancey, float stancez);
        void mode_ortho(float eyex, float eyey, float eyez, float sightx, float sighty, float sightz, float stancex, float stancey, float stancez);
        void clear();
        void flush();
        void pushMatrix();
        void popMatrix();
        void coordinate(vec3 o, vec3 x, vec3 y, vec3 z);
        void translate(float dx, float dy, float dz = 0);
        void rotate(float angle, float axisx, float axisy, float axisz);
        void setColor(int r, int g, int b);
        void fillRect(int x, int y, int w, int h);
        void drawSphere(float x, float y, float z, float r, int order);
        void setFont(font const & f);
        void drawChar(int ch, float size, bool translate = true, h_align halign = h_align::left, v_align valign = v_align::baseline);
        bool getTextExtent(std::string text, float size, float * width, float * height, float * accent);
        // if translate = true, subsequent calls after the first call should have default align parameters to start drawing at the previous end point
        void drawText(std::string text, float size, bool translate = false, h_align halign = h_align::left, v_align valign = v_align::baseline);
        clip & clips();
    };
}

#endif
