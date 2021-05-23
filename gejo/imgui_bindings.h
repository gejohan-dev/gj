// Hacky way of getting bindings for OpenGL for ImGui

#if !defined(IMGUI_BINDINGS_H)
#define IMGUI_BINDINGS_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

typedef char                 GLchar;
typedef int                  GLint;
typedef unsigned int         GLenum;
typedef unsigned int         GLuint;
typedef int                  GLsizei;
typedef unsigned char        GLboolean;
typedef float                GLfloat;
typedef double               GLdouble;
typedef signed long long int GLsizeiptr;

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_FUNC_ADD                       0x8006
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_TEXTURE0                       0x84C0
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STREAM_DRAW                    0x88E0
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STREAM_DRAW                    0x88E0
#define GL_ARRAY_BUFFER                   0x8892
#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_ARRAY_BUFFER                   0x8892

typedef void   type_glActiveTexture (GLenum texture);
typedef void   type_glAttachShader (GLuint program, GLuint shader);
typedef void   type_glBindBuffer (GLenum target, GLuint buffer);
typedef void   type_glBindVertexArray (GLuint array);
typedef void   type_glBlendEquation (GLenum mode);
typedef void   type_glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
typedef void   type_glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void   type_glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void   type_glCompileShader (GLuint shader);
typedef GLuint type_glCreateProgram (void);
typedef GLuint type_glCreateShader (GLenum type);
typedef void   type_glDeleteBuffers (GLsizei n, const GLuint *buffers);
typedef void   type_glDeleteProgram (GLuint program);
typedef void   type_glDeleteShader (GLuint shader);
typedef void   type_glDeleteVertexArrays (GLsizei n, const GLuint *arrays);
typedef void   type_glDetachShader (GLuint program, GLuint shader);
typedef void   type_glEnableVertexAttribArray (GLuint index);
typedef void   type_glGenBuffers (GLsizei n, GLuint *buffers);
typedef void   type_glGenVertexArrays (GLsizei n, GLuint *arrays);
typedef GLint  type_glGetAttribLocation (GLuint program, const GLchar *name);
typedef void   type_glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void   type_glGetProgramiv (GLuint program, GLenum pname, GLint *params);
typedef void   type_glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void   type_glGetShaderiv (GLuint shader, GLenum pname, GLint *params);
typedef GLint  type_glGetUniformLocation (GLuint program, const GLchar *name);
typedef void   type_glLinkProgram (GLuint program);
typedef void   type_glShaderSource (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void   type_glUniform1i (GLint location, GLint v0);
typedef void   type_glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void   type_glUseProgram (GLuint program);
typedef void   type_glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

#if defined(IMGUI_BINDINGS_IMPL)
type_glActiveTexture*           glActiveTexture;      
type_glAttachShader*            glAttachShader;      
type_glBindBuffer*              glBindBuffer;      
type_glBindVertexArray*         glBindVertexArray;      
type_glBlendEquation*           glBlendEquation;      
type_glBlendEquationSeparate*   glBlendEquationSeparate;      
type_glBlendFuncSeparate*       glBlendFuncSeparate;      
type_glBufferData*              glBufferData;      
type_glCompileShader*           glCompileShader;      
type_glCreateProgram*           glCreateProgram;      
type_glCreateShader*            glCreateShader;      
type_glDeleteBuffers*           glDeleteBuffers;      
type_glDeleteProgram*           glDeleteProgram;      
type_glDeleteShader*            glDeleteShader;      
type_glDeleteVertexArrays*      glDeleteVertexArrays;      
type_glDetachShader*            glDetachShader;      
type_glEnableVertexAttribArray* glEnableVertexAttribArray;      
type_glGenBuffers*              glGenBuffers;      
type_glGenVertexArrays*         glGenVertexArrays;      
type_glGetAttribLocation*       glGetAttribLocation;      
type_glGetProgramInfoLog*       glGetProgramInfoLog;      
type_glGetProgramiv*            glGetProgramiv;      
type_glGetShaderInfoLog*        glGetShaderInfoLog;      
type_glGetShaderiv*             glGetShaderiv;      
type_glGetUniformLocation*      glGetUniformLocation;      
type_glLinkProgram*             glLinkProgram;      
type_glShaderSource*            glShaderSource;      
type_glUniform1i*               glUniform1i;      
type_glUniformMatrix4fv*        glUniformMatrix4fv;      
type_glUseProgram*              glUseProgram;      
type_glVertexAttribPointer*     glVertexAttribPointer;
#else
extern type_glActiveTexture*           glActiveTexture;
extern type_glAttachShader*            glAttachShader;
extern type_glBindBuffer*              glBindBuffer;
extern type_glBindVertexArray*         glBindVertexArray;
extern type_glBlendEquation*           glBlendEquation;
extern type_glBlendEquationSeparate*   glBlendEquationSeparate;
extern type_glBlendFuncSeparate*       glBlendFuncSeparate;
extern type_glBufferData*              glBufferData;
extern type_glCompileShader*           glCompileShader;
extern type_glCreateProgram*           glCreateProgram;
extern type_glCreateShader*            glCreateShader;
extern type_glDeleteBuffers*           glDeleteBuffers;
extern type_glDeleteProgram*           glDeleteProgram;
extern type_glDeleteShader*            glDeleteShader;
extern type_glDeleteVertexArrays*      glDeleteVertexArrays;
extern type_glDetachShader*            glDetachShader;
extern type_glEnableVertexAttribArray* glEnableVertexAttribArray;
extern type_glGenBuffers*              glGenBuffers;
extern type_glGenVertexArrays*         glGenVertexArrays;
extern type_glGetAttribLocation*       glGetAttribLocation;
extern type_glGetProgramInfoLog*       glGetProgramInfoLog;
extern type_glGetProgramiv*            glGetProgramiv;
extern type_glGetShaderInfoLog*        glGetShaderInfoLog;
extern type_glGetShaderiv*             glGetShaderiv;
extern type_glGetUniformLocation*      glGetUniformLocation;
extern type_glLinkProgram*             glLinkProgram;
extern type_glShaderSource*            glShaderSource;
extern type_glUniform1i*               glUniform1i;
extern type_glUniformMatrix4fv*        glUniformMatrix4fv;
extern type_glUseProgram*              glUseProgram;
extern type_glVertexAttribPointer*     glVertexAttribPointer;
#endif

struct ImGuiOpenGL
{
    type_glActiveTexture*           glActiveTexture;
    type_glAttachShader*            glAttachShader;
    type_glBindBuffer*              glBindBuffer;
    type_glBindVertexArray*         glBindVertexArray;
    type_glBlendEquation*           glBlendEquation;
    type_glBlendEquationSeparate*   glBlendEquationSeparate;
    type_glBlendFuncSeparate*       glBlendFuncSeparate;
    type_glBufferData*              glBufferData;
    type_glCompileShader*           glCompileShader;
    type_glCreateProgram*           glCreateProgram;
    type_glCreateShader*            glCreateShader;
    type_glDeleteBuffers*           glDeleteBuffers;
    type_glDeleteProgram*           glDeleteProgram;
    type_glDeleteShader*            glDeleteShader;
    type_glDeleteVertexArrays*      glDeleteVertexArrays;
    type_glDetachShader*            glDetachShader;
    type_glEnableVertexAttribArray* glEnableVertexAttribArray;
    type_glGenBuffers*              glGenBuffers;
    type_glGenVertexArrays*         glGenVertexArrays;
    type_glGetAttribLocation*       glGetAttribLocation;
    type_glGetProgramInfoLog*       glGetProgramInfoLog;
    type_glGetProgramiv*            glGetProgramiv;
    type_glGetShaderInfoLog*        glGetShaderInfoLog;
    type_glGetShaderiv*             glGetShaderiv;
    type_glGetUniformLocation*      glGetUniformLocation;
    type_glLinkProgram*             glLinkProgram;
    type_glShaderSource*            glShaderSource;
    type_glUniform1i*               glUniform1i;
    type_glUniformMatrix4fv*        glUniformMatrix4fv;
    type_glUseProgram*              glUseProgram;
    type_glVertexAttribPointer*     glVertexAttribPointer;
};

#define CreateImGuiOpenGL(OpenGL)                                       \
    ImGuiOpenGL{                                                        \
        glActiveTexture = OpenGL##.glActiveTexture,                     \
            glAttachShader = OpenGL##.glAttachShader,                   \
            glBindBuffer = OpenGL##.glBindBuffer,                       \
            glBindVertexArray = OpenGL##.glBindVertexArray,             \
            glBlendEquation = OpenGL##.glBlendEquation,                 \
            glBlendEquationSeparate = OpenGL##.glBlendEquationSeparate, \
            glBlendFuncSeparate = OpenGL##.glBlendFuncSeparate,         \
            glBufferData = OpenGL##.glBufferData,                       \
            glCompileShader = OpenGL##.glCompileShader,                 \
            glCreateProgram = OpenGL##.glCreateProgram,                 \
            glCreateShader = OpenGL##.glCreateShader,                   \
            glDeleteBuffers = OpenGL##.glDeleteBuffers,                 \
            glDeleteProgram = OpenGL##.glDeleteProgram,                 \
            glDeleteShader = OpenGL##.glDeleteShader,                   \
            glDeleteVertexArrays = OpenGL##.glDeleteVertexArrays,       \
            glDetachShader = OpenGL##.glDetachShader,                   \
            glEnableVertexAttribArray = OpenGL##.glEnableVertexAttribArray, \
            glGenBuffers = OpenGL##.glGenBuffers,                       \
            glGenVertexArrays = OpenGL##.glGenVertexArrays,             \
            glGetAttribLocation = OpenGL##.glGetAttribLocation,         \
            glGetProgramInfoLog = OpenGL##.glGetProgramInfoLog,         \
            glGetProgramiv = OpenGL##.glGetProgramiv,                   \
            glGetShaderInfoLog = OpenGL##.glGetShaderInfoLog,           \
            glGetShaderiv = OpenGL##.glGetShaderiv,                     \
            glGetUniformLocation = OpenGL##.glGetUniformLocation,       \
            glLinkProgram = OpenGL##.glLinkProgram,                     \
            glShaderSource = OpenGL##.glShaderSource,                   \
            glUniform1i = OpenGL##.glUniform1i,                         \
            glUniformMatrix4fv = OpenGL##.glUniformMatrix4fv,           \
            glUseProgram = OpenGL##.glUseProgram,                       \
            glVertexAttribPointer = OpenGL##.glVertexAttribPointer,     \
            }

static void
init_global_opengl_bindings(ImGuiOpenGL imgui_opengl)
{
    glActiveTexture           = imgui_opengl.glActiveTexture;
    glAttachShader            = imgui_opengl.glAttachShader;
    glBindBuffer              = imgui_opengl.glBindBuffer;
    glBindVertexArray         = imgui_opengl.glBindVertexArray;
    glBlendEquation           = imgui_opengl.glBlendEquation;
    glBlendEquationSeparate   = imgui_opengl.glBlendEquationSeparate;
    glBlendFuncSeparate       = imgui_opengl.glBlendFuncSeparate;
    glBufferData              = imgui_opengl.glBufferData;
    glCompileShader           = imgui_opengl.glCompileShader;
    glCreateProgram           = imgui_opengl.glCreateProgram;
    glCreateShader            = imgui_opengl.glCreateShader;
    glDeleteBuffers           = imgui_opengl.glDeleteBuffers;
    glDeleteProgram           = imgui_opengl.glDeleteProgram;
    glDeleteShader            = imgui_opengl.glDeleteShader;
    glDeleteVertexArrays      = imgui_opengl.glDeleteVertexArrays;
    glDetachShader            = imgui_opengl.glDetachShader;
    glEnableVertexAttribArray = imgui_opengl.glEnableVertexAttribArray;
    glGenBuffers              = imgui_opengl.glGenBuffers;
    glGenVertexArrays         = imgui_opengl.glGenVertexArrays;
    glGetAttribLocation       = imgui_opengl.glGetAttribLocation;
    glGetProgramInfoLog       = imgui_opengl.glGetProgramInfoLog;
    glGetProgramiv            = imgui_opengl.glGetProgramiv;
    glGetShaderInfoLog        = imgui_opengl.glGetShaderInfoLog;
    glGetShaderiv             = imgui_opengl.glGetShaderiv;
    glGetUniformLocation      = imgui_opengl.glGetUniformLocation;
    glLinkProgram             = imgui_opengl.glLinkProgram;
    glShaderSource            = imgui_opengl.glShaderSource;
    glUniform1i               = imgui_opengl.glUniform1i;
    glUniformMatrix4fv        = imgui_opengl.glUniformMatrix4fv;
    glUseProgram              = imgui_opengl.glUseProgram;
    glVertexAttribPointer     = imgui_opengl.glVertexAttribPointer;
}

#endif
