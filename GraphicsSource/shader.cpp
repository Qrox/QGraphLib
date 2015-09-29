#include "gl/glew.h"
#include "shader.h"
#include "types.h"
typedef i64 off64_t;
#include <io.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

bool shaderprogram::glew_ok = false;

shader::shader() : id(0) {}

shader::shader(GLenum type, string file) {
    if (shaderprogram::glew_ok) {
        cout << "loading shader file \"" << file << "\"" << endl;
        int hfile = open(file.data(), _O_RDONLY | _O_BINARY);
        id = 0;
        if (hfile == -1) {
            cout << "failed to load shader file \"" << file << "\"!" << endl;
        } else {
            long length = filelength(hfile);
            if (length < 0 || length > 0x7FFFFFFF) {
                cout << "failed to load shader file \"" << file << "\"!" << endl;
            } else {
                GLint len = length;
                GLchar * data = new GLchar[len];
                if (read(hfile, data, len) != len) {
                    cout << "failed to complete loading shader file \"" << file << "\"!" << endl;
                } else {
                    id = glCreateShader(type);
                    glShaderSource(id, 1, &data, &len);
                    glCompileShader(id);
                    GLint compiled = 0;
                    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
                    if (!compiled) {
                        cout << "failed to compile shader!" << endl
                                  << "compiler logs:" << endl;
                        printlog();
                        glDeleteShader(id);
                        id = 0;
                    }
#ifdef DEBUG
                    else {
                        cout << "compiler logs:" << endl;
                        printlog();
                    }
#endif
                }
                delete [] data;
            }
            close(hfile);
        }
    } else id = 0;
}

shader::~shader() {
    //destroy();
    // if uncommented, requires user to keep the object alive.
    // if commented, requires user to explicitly destroy the object.
}

bool shader::compiled() const {
    return id != 0;
}

void shader::printlog() const {
    if (shaderprogram::glew_ok && id != 0) {
        GLint info_len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 0) {
            char * log = new char[info_len];
            glGetShaderInfoLog(id, info_len, nullptr, log);
            cout << log << endl;
            delete [] log;
        }
    }
}

void shader::destroy() {
    if (shaderprogram::glew_ok && id != 0) {
        glDeleteShader(id);
        id = 0;
    }
}

shaderprogram::static_init::static_init() {
    glew_ok = false;
    GLubyte const * version = glGetString(GL_VERSION);
    cout << "gl version: ";
    if (version == nullptr) cout << "unknown";
    else cout << version;
    cout << endl;
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        cout << "glew initialization failed, error code " << res << endl;
        return;
    }
    if (!GLEW_VERSION_2_0) {
        cout << "opengl 2.0 not supported! shaders will not work" << endl;
        return;
    }
    glew_ok = true;
}

shaderprogram::shaderprogram() : id(0) {}

shaderprogram::~shaderprogram() {
    //destroy();
    // if uncommented, requires user to keep the object alive.
    // if commented, requires user to explicitly destroy the object.
}

void shaderprogram::create() {
    if (glew_ok) id = glCreateProgram();
    else id = 0;
}

void shaderprogram::attach(shader s) {
    if (glew_ok && id != 0 && s.id != 0) {
        glAttachShader(id, s.id);
    }
}

void shaderprogram::detach(shader s) {
    if (glew_ok && id != 0 && s.id != 0) {
        glDetachShader(id, s.id);
    }
}

bool shaderprogram::link() {
    if (glew_ok && id != 0) {
        glLinkProgram(id);
        GLint linked = 0;
        glGetProgramiv(id, GL_LINK_STATUS, &linked);
        if (!linked) {
            cout << "failed to link shader program!" << endl
                      << "linker logs:" << endl;
            printlog();
            return false;
        }
#ifdef DEBUG
        else {
            cout << "linker logs:" << endl;
            printlog();
        }
#endif
        return true;
    } else return false;
}

void shaderprogram::apply() const {
    if (glew_ok) {
        glUseProgram(id);
    }
}

void shaderprogram::printlog() const {
    if (glew_ok && id != 0) {
        GLint info_len;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 0) {
            char * log = new char[info_len];
            glGetProgramInfoLog(id, info_len, nullptr, log);
            cout << log << endl;
            delete [] log;
        }
    }
}

void shaderprogram::destroy() {
    if (glew_ok && id != 0) {
        glDeleteProgram(id);
        id = 0;
    }
}

GLint shaderprogram::getAttribLocation(char const * name) const {
    if (glew_ok && id != 0) {
        GLint loc = glGetAttribLocation(id, name);
        if (loc < 0) {
            cout << "can't find attrib named " << name << endl;
        }
        return loc;
    } else return -1;
}

GLint shaderprogram::getUniformLocation(char const * name) const {
    if (glew_ok && id != 0) {
        GLint loc = glGetUniformLocation(id, name);
        if (loc < 0) {
            cout << "can't find uniform named " << name << endl;
        }
        return loc;
    } else return -1;
}

GLint shaderprogram::getUniformBlockIndex(char const * name) const {
    if (glew_ok && id != 0) {
        GLint ind = glGetUniformBlockIndex(id, name);
        if (ind < 0) {
            cout << "can't find uniform block named " << name << endl;
        }
        return ind;
    } else return -1;
}

shaderprogram const shaderprogram::no_program;
