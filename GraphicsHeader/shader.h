#ifndef __QROX_SHADER_H__
#define __QROX_SHADER_H__

#include <gl/gl.h>
#include <string>

class shader {
private:
    GLuint id;
public:
    friend class shaderprogram;

    shader();
    shader(GLenum type, std::string file);
    ~shader();
    bool compiled() const;
    void printlog() const;
    void destroy();
};

class shaderprogram {
private:
    static bool glew_ok;

    GLuint id;
public:
    class static_init {
    public:
        static_init();
    };

    friend class shader;

    shaderprogram();
    ~shaderprogram();
    void create();
    void attach(shader s);
    void detach(shader s);
    bool link();
    void apply() const;
    void printlog() const;
    void destroy();
    GLint getAttribLocation(char const * name) const;
    GLint getUniformLocation(char const * name) const;
    GLint getUniformBlockIndex(char const * name) const;

    static shaderprogram const no_program;
};

#endif
