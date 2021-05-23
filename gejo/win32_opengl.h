#if !defined(WIN32_OPENGL_H)
#define WIN32_OPENGL_H

#include <libs/wglext.h>
typedef BOOL        OPENGL_APIENTRY wgl_swap_interval_EXT(int interval);
typedef HGLRC       OPENGL_APIENTRY wgl_create_context_attribs_ARB(HDC hdc, HGLRC h_share_context, const int *attrib_list);
typedef BOOL        OPENGL_APIENTRY wgl_choose_pixel_format_ARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef const char* OPENGL_APIENTRY wgl_get_extensions_string_ARB(HDC hdc);

global_variable wgl_swap_interval_EXT*          wglSwapIntervalEXT;
global_variable wgl_create_context_attribs_ARB* wglCreateContextAttribsARB;
global_variable wgl_choose_pixel_format_ARB*    wglChoosePixelFormatARB;
global_variable wgl_get_extensions_string_ARB*  wglGetExtensionsStringARB;

typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar *message,
                                     const void *userParam);

internal void OPENGL_APIENTRY
opengl_error_callback(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam)
{
#define GL_DEBUG_TYPE_ERROR               0x824C
    g_platform_api.debug_print("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                               ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                               type, severity, message);
    Assert(type != GL_DEBUG_TYPE_ERROR);
}

struct Win32OpenGL
{
    HGLRC context;
    OpenGL api;
};

internal Win32OpenGL
win32_init_opengl(HDC device_ctx)
{
    Win32OpenGL result = {};

    {
        WNDCLASSA window_class = {};
        window_class.lpfnWndProc = DefWindowProcA;
        window_class.hInstance = GetModuleHandle(0);
        window_class.lpszClassName = "WGLLoader";
        RegisterClassA(&window_class);
        HWND window = CreateWindowExA(
            0,
            window_class.lpszClassName,
            "WGLLoader",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            window_class.hInstance,
            0);
        HDC window_dc = GetDC(window);
        
        PIXELFORMATDESCRIPTOR descriptor;
        descriptor.nSize = sizeof(descriptor);
        descriptor.nVersion = 1;
        descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        descriptor.iPixelType = PFD_TYPE_RGBA;
        descriptor.cColorBits = 32;
        descriptor.cRedBits = 8;
        descriptor.cGreenBits = 8;
        descriptor.cBlueBits = 8;
        descriptor.cAlphaBits = 8;
        descriptor.cDepthBits = 24;
        descriptor.cStencilBits = 8;

        s32 pixel_format = ChoosePixelFormat(window_dc, &descriptor);
        Assert(SetPixelFormat(window_dc, pixel_format, &descriptor));

        HGLRC tmp_opengl_context = wglCreateContext(window_dc);
        Assert(wglMakeCurrent(window_dc, tmp_opengl_context));

        wglChoosePixelFormatARB = (wgl_choose_pixel_format_ARB*)wglGetProcAddress("wglChoosePixelFormatARB");
        Assert(wglChoosePixelFormatARB);
        wglCreateContextAttribsARB = (wgl_create_context_attribs_ARB*)wglGetProcAddress("wglCreateContextAttribsARB");
        Assert(wglCreateContextAttribsARB);
        wglSwapIntervalEXT = (wgl_swap_interval_EXT*)wglGetProcAddress("wglSwapIntervalEXT");
        Assert(wglSwapIntervalEXT);
        wglGetExtensionsStringARB = (wgl_get_extensions_string_ARB*)wglGetProcAddress("wglGetExtensionsStringARB");
        Assert(wglGetExtensionsStringARB);

        wglMakeCurrent(0, 0);
        wglDeleteContext(tmp_opengl_context);
        ReleaseDC(window, window_dc);
        DestroyWindow(window);
    }

    int pixel_format_attribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0, // End
        };

    s32 pixel_format;
    UINT format_count;
    wglChoosePixelFormatARB(device_ctx, pixel_format_attribs, NULL, 1, &pixel_format, &format_count);
    Assert(format_count);

    PIXELFORMATDESCRIPTOR descriptor;
    DescribePixelFormat(device_ctx, pixel_format, sizeof(pixel_format), &descriptor);
    Assert(SetPixelFormat(device_ctx, pixel_format, &descriptor));

    GLint major_version = 4;
    GLint minor_version = 3;
    int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_version,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_version,
        WGL_CONTEXT_FLAGS_ARB, 0
#if SIDESCROLLER_DEBUG
        |WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        ,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    result.context = wglCreateContextAttribsARB(device_ctx, 0, context_attribs);
    Assert(result.context);

    Assert(wglMakeCurrent(device_ctx, result.context));

    return result;
}

#endif
