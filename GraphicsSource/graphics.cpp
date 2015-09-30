#include "gl/glew.h"
#include <gl/glu.h>
#include "graphics.h"

#include <iostream>
#include <algorithm>

#include "advancedmath.h"
#include "profiler.h"

using namespace std;

namespace ui {
    PIXELFORMATDESCRIPTOR const opengl::default_pfd {
        sizeof(PIXELFORMATDESCRIPTOR),                                  // nSize
        1,                                                              // nVersion
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,     // dwFlags
        PFD_TYPE_RGBA,                                                  // iPixelType
        32,                                                             // cColorBits
        0, 0, 0, 0, 0, 0, 0, 0,                                         // c Red/Green/Blue/Alpha Bits/Shift
        0,                                                              // cAccumBits
        0, 0, 0, 0,                                                     // cAccumRed/Green/Blue/AlphaBits
        24,                                                             // cDepthBits
        8,                                                              // cStencilBits
        0,                                                              // cAuxBuffers
        PFD_MAIN_PLANE,                                                 // iLayerType
        0,                                                              // bReserved
        0, 0, 0                                                         // dwLayerMask, dwVisibleMask, dwDamageMask
    };

    MAT2 const opengl::identity_mat2 {
        {0, 1}, {0, 0},
        {0, 0}, {0, 1},
    };

    void opengl::reload_shader() {
        do {
            font_outline_cache.clear();
            font_vs.destroy();
            font_fs.destroy();
            font_sp.destroy();

            font_vs = shader(GL_VERTEX_SHADER, "./shader/font.vs");
            if (!font_vs.compiled()) break;
            font_fs = shader(GL_FRAGMENT_SHADER, "./shader/font.fs");
            if (!font_fs.compiled()) break;
            font_sp.create();
            font_sp.attach(font_vs);
            font_sp.attach(font_fs);
            if (!font_sp.link()) {
                font_sp.destroy();
                font_vs.destroy();
                font_fs.destroy();
                break;
            }
            font_outline = font_sp.getUniformBlockIndex("outline");
            font_coord = font_sp.getAttribLocation("coord");
            font_i32cnt = font_sp.getUniformLocation("i32cnt");
            font_emsize = font_sp.getUniformLocation("emsize");
            font_mscreen = font_sp.getUniformLocation("mscreen");
            font_mtextomdl = font_sp.getUniformLocation("mtextomdl");
        } while (false);
    }

    opengl::opengl(HWND hwnd) : hwnd(hwnd), font_outline(-1), font_coord(-1),
            font_outline_cache(256, objectcache<int, font_outline_info>::replacement_rule::lru,
                    [this] (int key) -> font_outline_info {
                        font_outline_info info;
                        if (emfont.f) {
                            SelectObject(hdc, emfont.f.get());
                            DWORD size = GetGlyphOutline(hdc, key, GGO_NATIVE, &info.gm, 0, nullptr, &identity_mat2);
                            if (size == GDI_ERROR) {
                                std::fill((char *) &info, ((char *) &info) + sizeof(info), 0);
                                return info;
                            }

                            if (size <= 16 || size > font_outline_buf_size) {  // space or overflow
                                info.disp_list = 0;
                                info.data_size = 0;
                                return info;
                            } else {
                                glGenBuffers(1, &info.disp_list);
                                glBindBufferBase(GL_UNIFORM_BUFFER, font_outline, info.disp_list);  // just for glBufferData & glMapBufferRange to work...
                                glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
                                TTPOLYGONHEADER * buf = (TTPOLYGONHEADER *) glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                                if (!buf) {
                                    glDeleteBuffers(1, &info.disp_list);
                                    info.disp_list = 0;
                                    info.data_size = 0;
                                    return info;
                                }
                                if (GetGlyphOutline(hdc, key, GGO_NATIVE, &info.gm, size, buf, &identity_mat2) == GDI_ERROR) {
                //                                glUnmapBuffer(GL_UNIFORM_BUFFER);
                                    glDeleteBuffers(1, &info.disp_list);
                                    info.disp_list = 0;
                                    info.data_size = 0;
                                    return info;
                                } else {
                                    glUnmapBuffer(GL_UNIFORM_BUFFER);
                                    info.data_size = size;
                                    return info;
                                }
                            }
                        } else {
                            std::fill((char *) &info, ((char *) &info) + sizeof(info), 0);
                            return info;
                        }
                    },
                    [] (int key, font_outline_info val) {
                        glDeleteBuffers(1, &val.disp_list);
                    }) {
        if (hwnd != NULL) {
            hdc = GetDC(hwnd);
            GLuint pixel_format = ChoosePixelFormat(hdc, &default_pfd);
            SetPixelFormat(hdc, pixel_format, &default_pfd);
            hrc = wglCreateContext(hdc);
            RECT rect;
            if (GetClientRect(hwnd, &rect)) {
                resize(rect.right - rect.left, rect.bottom - rect.top);
            }

            shaderprogram::static_init init;

            glShadeModel(GL_SMOOTH);
            glDepthFunc(GL_LEQUAL);
            glClearDepth(1.0f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            glEnable(GL_RESCALE_NORMAL);

            setFont(font("", 0));

            reload_shader();
        }
    }

    opengl::~opengl() {
        font_outline_cache.clear();
        font_vs.destroy();
        font_fs.destroy();
        font_sp.destroy();

        wglDeleteContext(hrc);
        ReleaseDC(hwnd, hdc);
    }

    void opengl::resize(int width, int height) {
        makeCurrent();
        glViewport(0, 0, width, height);
        glGetIntegerv(GL_VIEWPORT, viewport);

        // 2D view
        GLdouble mat[16] = {
            2. / width,            0, 0, 0,
                     0, -2. / height, 0, 0,
                     0,            0, 1, 0,
                    -1,            1, 0, 1,
        };
        copy(mat, mat + 16, matrix_proj_2d);

        // 3D frustum view
        float constexpr viewport_size = .293;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float scrPorpotion = (width == 0 || height == 0) ? 1 : (float) width / (float) height;
        float w, h, d;
        if (width <= height) {
            w = viewport_size;
            h = w / scrPorpotion;
            d = w * 1.5;
            float _d = d / (5. / 128);
            w /= _d;
            h /= _d;
            d /= _d;
            glFrustum(-w, w, -h, h, d, 5);
            glTranslatef(0, 0, -d);
        } else {
            h = viewport_size;
            w = h * scrPorpotion;
            d = h * 1.5;
            float _d = d / (5. / 128);
            w /= _d;
            h /= _d;
            d /= _d;
            glFrustum(-w, w, -h, h, d, 5);
            glTranslatef(0, 0, -d);
        }
        glGetDoublev(GL_PROJECTION_MATRIX, matrix_proj_3d_frustum);

        // 3D ortho view
        glLoadIdentity();
        h = 1;
        w = h * scrPorpotion;
        glOrtho(0, w, h, 0, 0, 1);
        glGetDoublev(GL_PROJECTION_MATRIX, matrix_proj_3d_ortho);

        glMatrixMode(GL_MODELVIEW);
    }

    void opengl::makeCurrent() {
        wglMakeCurrent(hdc, hrc);
    }

    void opengl::mode_2d() {
        mode = _2d;
        makeCurrent();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(matrix_proj_2d);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilMask(0);
    }

    void opengl::mode_frustum(float eyex, float eyey, float eyez, float sightx, float sighty, float sightz, float stancex, float stancey, float stancez) {
        mode = frustum;
        makeCurrent();
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(matrix_proj_3d_frustum);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eyex, eyey, eyez, sightx, sighty, sightz, stancex, stancey, stancez);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void opengl::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void opengl::flush() {
        glFlush();
    }

    void opengl::pushMatrix() {
        glPushMatrix();
    }

    void opengl::popMatrix() {
        glPopMatrix();
    }

    void opengl::coordinate(vec3 o, vec3 x, vec3 y, vec3 z) {
        GLfloat mat[16];
        float const *po = o, *px = x, *py = y, *pz = z;
        copy(px, px + 3, mat); mat[3] = 0;
        copy(py, py + 3, mat + 4); mat[7] = 0;
        copy(pz, pz + 3, mat + 8); mat[11] = 0;
        copy(po, po + 3, mat + 12); mat[15] = 1;
        glMultMatrixf(mat);
    }

    void opengl::translate(float dx, float dy, float dz) {
        glTranslatef(dx, dy, dz);
    }

    void opengl::rotate(float angle, float x, float y, float z) {
        glRotatef(angle / pi * 180, x, y, z);
    }

    void opengl::setColor(int r, int g, int b) {
        glColor3ub(r, g, b);
    }

    void opengl::fillRect(int x, int y, int w, int h) {
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x, y + h);
        glVertex2i(x + w, y + h);
        glVertex2i(x + w, y);
        glEnd();
    }

    void opengl::drawSphere(float x, float y, float z, float r, int order) {
        if (order < 1) return;
        int order2 = order << 1, order3 = order + order2, order4 = order2 << 1;
        float sins[order + 1];
        sins[0] = 0; sins[order] = 1;
        float dang = pi / order2;
        float theta = dang;
        for (int i = 1; i < order; ++i, theta += dang) {
            sins[i] = sin(theta);
        }
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(r, r, r);
        float sinphi0 = 0, cosphi0 = 1;
        for (int i = 1; i <= order2; ++i) {
            float sinphi1 = i <= order ? sins[i] : sins[order2 - i];
            float cosphi1 = i <= order ? sins[order - i] : -sins[i - order];
            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= order4; ++j) {
                float sintheta, costheta;
                if (j <= order) sintheta = sins[j];
                else if (j <= order2) sintheta = sins[order2 - j];
                else if (j <= order3) sintheta = -sins[j - order2];
                else sintheta = -sins[order4 - j];
                if (j <= order) costheta = sins[order - j];
                else if (j <= order2) costheta = -sins[j - order];
                else if (j <= order3) costheta = -sins[order3 - j];
                else costheta = sins[j - order3];
                float ex0 = sinphi0 * costheta, ey0 = sinphi0 * sintheta, ez0 = cosphi0;
                float ex1 = sinphi1 * costheta, ey1 = sinphi1 * sintheta, ez1 = cosphi1;
                glNormal3f(ex0, ey0, ez0);
                glVertex3f(ex0, ey0, ez0);
                glNormal3f(ex1, ey1, ez1);
                glVertex3f(ex1, ey1, ez1);
            }
            glEnd();
            sinphi0 = sinphi1;
            cosphi0 = cosphi1;
        }
        glPopMatrix();
    }

    void opengl::setFont(font const & f) {
        if (f.f && f.f != infont.f) {
            infont = f;
            SelectObject(hdc, f.f.get());
            if (!GetOutlineTextMetrics(hdc, sizeof(inotm), &inotm)) {
                cout << "failed to get font em size, 3d rendering disabled." << endl;
                emfont = font();
            } else {
                LOGFONT em = f.lf;
                em.lfHeight = inotm.otmEMSquare;
                emfont = font(em);
                if (emfont.f) {
                    SelectObject(hdc, emfont.f.get());
                    if (!GetOutlineTextMetrics(hdc, sizeof(emotm), &emotm)) {
                        cout << "failed to get em font metrics, 3d rendering disabled." << endl;
                        emfont = font();
                    }
                } else cout << "failed to create em font, 3d rendering disabled." << endl;
            }
            font_outline_cache.clear();
        }
    }

    void opengl::drawChar(int ch, float fsize, bool translate, h_align halign, v_align valign) {
        if (emfont.f) {
            font_outline_info info = font_outline_cache.get(ch);
            if (info.disp_list) {
                font_sp.apply();
                glBindBufferBase(GL_UNIFORM_BUFFER, font_outline, info.disp_list);
                glUniform1ui(font_i32cnt, info.data_size >> 2);
                float emsize = emotm.otmEMSquare * 65536;
                glUniform1f(font_emsize, emsize);
                float height = emotm.otmTextMetrics.tmHeight;
                float accent = emotm.otmMacAscent;
                float scale = fsize / height;
                float textomdlscale = scale / 65536.f;
                float chrl = info.gm.gmptGlyphOrigin.x,
                      chrt = info.gm.gmptGlyphOrigin.y,
                      chrr = chrl + info.gm.gmBlackBoxX,
                      chrb = chrt - info.gm.gmBlackBoxY;
                float mscreen[16] = {   // column major
                    float(viewport[2]), 0, 0, 0,
                    0, float(viewport[3]), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1,
                }, mtextomdl[16] = {
                    textomdlscale, 0, 0, 0,
                    0, -textomdlscale, 0, 0,
                    0, 0, textomdlscale, 0,
                    0, 0, 0, 1,
                };
                switch (halign) {
                case h_align::left:
                    break;
                case h_align::center: {
                    float off = info.gm.gmCellIncX * .5;
                    chrl -= off;
                    chrr -= off;
                    mtextomdl[12] = -off;
                }   break;
                case h_align::right: {
                    float off = info.gm.gmCellIncX;
                    chrl -= off;
                    chrr -= off;
                    mtextomdl[12] = -off;
                }   break;
                }
                switch (valign) {
                case v_align::top: {
                    float off = accent + .5;
                    chrt += off;
                    chrb += off;
                    mtextomdl[13] = off;
                }   break;
                case v_align::center: {
                    float off = accent + .5 - height * .5;
                    chrt += off;
                    chrb += off;
                    mtextomdl[13] = off;
                }   break;
                case v_align::baseline:
                    break;
                case v_align::bottom: {
                    float off = accent + .5 - height;
                    chrt += off;
                    chrb += off;
                    mtextomdl[13] = off;
                }   break;
                }
                mtextomdl[12] *= scale;
                mtextomdl[13] *= scale;
                float mdll = chrl * scale,
                      mdlt = -chrt * scale,
                      mdlr = chrr * scale,
                      mdlb = -chrb * scale;

                GLdouble vscr[4][3];
                GLdouble matrix_modelview[16], matrix_projection[16];
                glGetDoublev(GL_MODELVIEW_MATRIX, matrix_modelview);
                glGetDoublev(GL_PROJECTION_MATRIX, matrix_projection);
                gluProject(mdll, mdlt, 0., matrix_modelview, matrix_projection, viewport, &vscr[0][0], &vscr[0][1], &vscr[0][2]);
                gluProject(mdll, mdlb, 0., matrix_modelview, matrix_projection, viewport, &vscr[1][0], &vscr[1][1], &vscr[1][2]);
                gluProject(mdlr, mdlb, 0., matrix_modelview, matrix_projection, viewport, &vscr[2][0], &vscr[2][1], &vscr[2][2]);
                gluProject(mdlr, mdlt, 0., matrix_modelview, matrix_projection, viewport, &vscr[3][0], &vscr[3][1], &vscr[3][2]);
                bool docalc = true;
                for (int i = 0; i < 4; ++i) {
                    if (vscr[i][2] <= 0) {
                        docalc = false;
                        break;
                    } else {
                        vscr[i][0] /= vscr[i][2];
                        vscr[i][1] /= vscr[i][2];
                    }
                }
                if (docalc) {
                    float sclt = fabsf(mdlr - mdll) / distance2d(vscr[0][0], vscr[0][1], vscr[3][0], vscr[3][1]);
                    float sclb = fabsf(mdlr - mdll) / distance2d(vscr[1][0], vscr[1][1], vscr[2][0], vscr[2][1]);
                    float scll = fabsf(mdlb - mdlt) / distance2d(vscr[0][0], vscr[0][1], vscr[1][0], vscr[1][1]);
                    float sclr = fabsf(mdlb - mdlt) / distance2d(vscr[3][0], vscr[3][1], vscr[2][0], vscr[2][1]);
                    mdll -= (sclt + sclb) * .5f;
                    mdlt -= (scll + sclr) * .5f;
                    mdlr += (sclt + sclb) * .5f;
                    mdlb += (scll + sclr) * .5f;    // todo: extends each point, instead of each edge
                    chrl = mdll / scale;
                    chrt = mdlt / -scale;
                    chrr = mdlr / scale;
                    chrb = mdlb / -scale;
                }
                float texl = chrl * 65536.f,
                      text = chrt * 65536.f,
                      texr = chrr * 65536.f,
                      texb = chrb * 65536.f;
                glUniformMatrix4fv(font_mscreen, 1, false, mscreen);
                glUniformMatrix4fv(font_mtextomdl, 1, false, mtextomdl);
                glPushAttrib(GL_ENABLE_BIT);
                glDisable(GL_CULL_FACE);
                glBegin(GL_QUADS);
                glVertexAttrib2f(font_coord, texl, text); glVertex2f(mdll, mdlt);
                glVertexAttrib2f(font_coord, texl, texb); glVertex2f(mdll, mdlb);
                glVertexAttrib2f(font_coord, texr, texb); glVertex2f(mdlr, mdlb);
                glVertexAttrib2f(font_coord, texr, text); glVertex2f(mdlr, mdlt);
                glEnd();
                glPopAttrib();
                if (translate) glTranslatef(info.gm.gmCellIncX * scale, info.gm.gmCellIncY * scale, 0);
                shaderprogram::no_program.apply();
            } else if (translate) {
                float height = emotm.otmTextMetrics.tmHeight;
                float scale = fsize / height;
                glTranslatef(info.gm.gmCellIncX * scale, info.gm.gmCellIncY * scale, 0);
            }
        }
    }

    bool opengl::getTextExtent(string text, float fsize, float * extent_width, float * extent_height, float * extent_accent) {
        float scale = fsize / emotm.otmTextMetrics.tmHeight;
        if (extent_width) {
            char const * str = text.c_str();
            int len = text.length();
            float width = 0;
            GLYPHMETRICS gm;
            for (int i = 0; i < len;) {
                int ch = str[i] & 0xFF;
                if (ch >= 0x80 && i + 1 < len) {
                    ch = (ch << 8) | (str[i + 1] & 0xFF);
                    i += 2;
                } else {
                    ++i;
                }
                if (GetGlyphOutline(hdc, ch, GGO_METRICS, &gm, 0, nullptr, &identity_mat2) == GDI_ERROR) return false;
                width += gm.gmCellIncX;
            }
            *extent_width = width * scale;
        }
        if (extent_height) *extent_height = fsize;
        if (extent_accent) *extent_accent = (emotm.otmMacAscent + .5) * scale;
        return true;
    }

    void opengl::drawText(string text, float size, bool translate, enum h_align halign, enum v_align valign) {
        char const * str = text.data();
        int len = text.length();
        if (!translate) glPushMatrix();
        if (halign != h_align::left || valign != v_align::baseline) {
            float w, h, a;
            getTextExtent(text, size, &w, &h, &a);
            float tx = 0, ty = 0;
            switch (halign) {
            case h_align::left:
                break;
            case h_align::center:
                tx -= w * .5;
                break;
            case h_align::right:
                tx -= w;
                break;
            }
            switch (valign) {
            case v_align::top:
                ty += a;
                break;
            case v_align::center:
                ty += a - h * .5;
                break;
            case v_align::baseline:
                break;
            case v_align::bottom:
                ty += a - h;
                break;
            }
            glTranslatef(tx, ty, 0);
        }
        for (int i = 0; i < len;) {
            int ch = str[i] & 0xFF;
            if (ch >= 0x80 && i + 1 < len) {
                ch = (ch << 8) | (str[i + 1] & 0xFF);
                i += 2;
            } else {
                ++i;
            }
            drawChar(ch, size, true);
        }
        if (!translate) glPopMatrix();
    }

    clip & opengl::clips() {
        return clp;
    }

//    // legacy code. preserved for possible reuse
//    void opengl::drawText2D(char const * s, int len, h_align halign, v_align valign) {
//        if (infont.f) {
//            double winx, winy, winz;
//            double model[16];
//            glGetDoublev(GL_MODELVIEW_MATRIX, model);
//            SelectObject(hdc, infont.f.get());
//            if (mode == _2d) {
//                gluProject(0, 0, 0, model, matrix_proj_2d, viewport, &winx, &winy, &winz);
//                drawText2D(s, len, winx, winy, halign, valign);
//            } else {
//                //todo 3d
//            }
//        }
//    }
//
//    void opengl::drawText2D(char const * s, int len, float x, float y, h_align halign, v_align valign) {
//        if (infont.f) {
//            SIZE text_size;
//            float l, b;
//            SelectObject(hdc, infont.f.get());
//            if (GetTextExtentPoint32(hdc, s, len, &text_size)) {
//                switch (halign) {
//                case h_align::left:
//                    l = 0;
//                    break;
//                case h_align::center:
//                    l = -text_size.cx / 2.;
//                    break;
//                case h_align::right:
//                    l = -text_size.cx;
//                    break;
//                }
//                switch (valign) {
//                case v_align::top:
//                    b = text_size.cy;
//                    break;
//                case v_align::center:
//                    b = text_size.cy / 2.;
//                    break;
//                case v_align::bottom:
//                    b = 0;
//                    break;
//                }
//            } else {
//                l = b = 0;
//            }
//            l += x; b += y;
//            TextOut(hdc, l, b, s, len); //todo
//        }
//    }
//
//    void opengl::drawText(char const * s, int len, h_align halign, v_align valign) {
//        if (infont.f) {
//            SIZE text_size;
//            float l, b;
//            SelectObject(hdc, infont.f.get());
//            if (GetTextExtentPoint32(hdc, s, len, &text_size)) {
//                switch (halign) {
//                case h_align::left:
//                    l = 0;
//                    break;
//                case h_align::center:
//                    l = -text_size.cx / 2.;
//                    break;
//                case h_align::right:
//                    l = -text_size.cx;
//                    break;
//                }
//                switch (valign) {
//                case v_align::top:
//                    b = text_size.cy;
//                    break;
//                case v_align::center:
//                    b = text_size.cy / 2.;
//                    break;
//                case v_align::bottom:
//                    b = 0;
//                    break;
//                }
//            } else {
//                l = b = 0;
//            }
//            glRasterPos2f(l, b);
//            while (len) {
//                int ch = *s & 0xFF;
//                --len; ++s;
//                if (len) {
//                    if (ch >= 128) {
//                        ch = (ch << 8) | (*s & 0xFF);
//                        --len; ++s;
//                    } else if ((ch == '\n' && *s == '\r') || (ch == '\r' && *s == '\n')) {
//                        --len; ++s;
//                    }
//                }
//                if (ch == '\n' || ch == '\r') {
//                    b += text_size.cy;
//                    glRasterPos2f(l, b);
//                } else if (wglUseFontBitmaps(hdc, ch, ch, font_display_list) || wglUseFontBitmaps(hdc, '?', '?', font_display_list)) {
//                    glCallLists(1, GL_UNSIGNED_INT, &font_display_list);
//                }
//            }
//            glRasterPos2f(0, 0);
//        }
//    }
}
