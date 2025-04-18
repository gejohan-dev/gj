#if !defined(WIN32_UTIL_H)
#define WIN32_UTIL_H

// Note: Requires
// #define WIN32_EXE_FILE_NAME [exe file name e.g. "win32_app.exe"]
// #include "win32_util.h"

#define gj_AssertHR(HR) do { gj_Assert(SUCCEEDED(HR)); } while(false)

#if !defined(WIN32_EXE_FILE_NAME)
#define WIN32_EXE_FILE_NAME NULL
#endif

// TODO: move into Win32App
/* global_variable WINDOWPLACEMENT g_window_pos = {sizeof(g_window_pos)}; */
/* struct Win32App */
/* { */
/*     HWND window; */
/*     HDC device_context; */
/* }; */
/* global_variable Win32App g_win32_app; */

V2u win32_get_window_dimension(HWND window)
{
    V2u result = {};
    
    RECT client_rect;
    GetClientRect(window, &client_rect);

    result.x = client_rect.right - client_rect.left;
    result.y = client_rect.bottom - client_rect.top;

    return result;
}

inline LARGE_INTEGER
win32_get_time()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline f32
win32_get_time_difference(LARGE_INTEGER start, LARGE_INTEGER end)
{
    LARGE_INTEGER perf_count_frequency; QueryPerformanceFrequency(&perf_count_frequency);
    f32 result = ((f32)(end.QuadPart - start.QuadPart) /
                  (f32)perf_count_frequency.QuadPart);
    return result;
}

void win32_toggle_fullscreen(HWND window, WINDOWPLACEMENT* window_pos)
{
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        if(GetWindowPlacement(window, window_pos) &&
           GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
        {
            SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        
        SetWindowLong(window, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, window_pos);
        SetWindowPos(window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

inline FILETIME
win32_get_last_write_time(const char* file_name)
{
    FILETIME result = {};
    WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
    HANDLE file_handle = CreateFileA(file_name, 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!GetFileTime(file_handle, NULL, NULL, &result))
    {
        DWORD error = GetLastError();
        InvalidCodePath;
    }
    CloseHandle(file_handle);
    return result;
}

static void
win32_get_cursor_pos(HWND window, f32* mouse_pos_x, f32* mouse_pos_y)
{
    POINT mouse_pos;
    GetCursorPos(&mouse_pos);
    gj_Assert(ScreenToClient(window, &mouse_pos));
    *mouse_pos_x = (f32)mouse_pos.x;
    *mouse_pos_y = (f32)mouse_pos.y;
}

static void
win32_set_cursor(b32 show)
{
    int show_count = ShowCursor(show);
    while (show ?
           (show_count < 1) :
           (show_count > 0))
    {
        show_count = ShowCursor(show);
    }
}
inline void
win32_show_cursor() { win32_set_cursor(gj_True); }
inline void
win32_hide_cursor() { win32_set_cursor(gj_False); }
///////////////////////////////////////////////////////////////////////////
// win32_build_exe_file_path
// Builds the path
// c:\Path\To\exe_file_location\file_name
///////////////////////////////////////////////////////////////////////////
void win32_build_exe_file_path(char* output_path, const char* file_name)
{
    char exe_base_path[BUFFER_SIZE];
    size_t exe_base_path_length = GetModuleFileNameA(NULL, exe_base_path, gj_ArrayCount(exe_base_path));
    gj_AssertDebug(exe_base_path_length < gj_ArrayCount(exe_base_path));
    // TODO: Implement error
    gj_Assert(WIN32_EXE_FILE_NAME);
    exe_base_path_length -= strlen(WIN32_EXE_FILE_NAME);
    // Note: Cut-off the exe file name
    exe_base_path[exe_base_path_length] = '\0';

    memcpy(output_path, exe_base_path, exe_base_path_length);
    size_t file_name_length = strlen(file_name);
    memcpy(output_path + exe_base_path_length, file_name, file_name_length);
    output_path[exe_base_path_length + file_name_length] = '\0';
}

struct Win32HotLoadedDLL
{
    char*    dll_file_path;
    char*    lock_file_path;
    u32      tmp_dll_number;
    FILETIME last_write_time;
    HMODULE  hmodule;
};

b32 win32_check_new_dll(Win32HotLoadedDLL dll)
{
    b32 result;
    FILETIME latest_write_time = win32_get_last_write_time(dll.dll_file_path);
    // Note: 1 = First file time (latest_write_time) is later than second file time (dll.last_write_time).
    result = CompareFileTime(&latest_write_time, &dll.last_write_time) == 1;
    return result;
}

void win32_load_dll(Win32HotLoadedDLL* dll)
{
    if (GetFileAttributesA(dll->lock_file_path) == INVALID_FILE_ATTRIBUTES)
    {
        dll->last_write_time = win32_get_last_write_time(dll->dll_file_path);

        // Note: This is done to not lock the dll for writing and
        //       also keep old dlls since the program could be
        //       pointing to things in the old dll
        char tmp_dll_path[BUFFER_SIZE];
        char unique_tmp_file_name[BUFFER_SIZE];
        stbsp_sprintf(unique_tmp_file_name, "tmp%d.dll", dll->tmp_dll_number);
        dll->tmp_dll_number++;
        win32_build_exe_file_path(tmp_dll_path, unique_tmp_file_name);
        BOOL ok = CopyFileA(dll->dll_file_path, tmp_dll_path, FALSE);
        if (!ok)
        {
            // TODO: Logging
            int error = GetLastError();
            brk;
        }

        dll->hmodule = LoadLibraryA(tmp_dll_path);
        {
            // TODO: Logging
            int error = GetLastError();
            brk;
        }
        gj_Assert(dll->hmodule);
    }
}

void win32_load_dll(Win32HotLoadedDLL* dll, char* dll_file_path, char* lock_file_path)
{
    dll->dll_file_path  = dll_file_path;
    dll->lock_file_path = lock_file_path;
    win32_load_dll(dll);
}

void* win32_load_function(Win32HotLoadedDLL dll, const char* function_name)
{
    void* result = GetProcAddress(dll.hmodule, function_name);
    gj_Assert(result);
    return result;
}

void win32_init_window(HINSTANCE instance,
                       WNDPROC window_proc,
                       const char* window_title,
                       HWND* window)
{
    WNDCLASSA window_class = {};
    window_class.style         = CS_VREDRAW | CS_HREDRAW;
    window_class.lpfnWndProc   = window_proc;
    window_class.hInstance     = instance;
    window_class.hCursor       = LoadCursor(0, IDC_ARROW);
    window_class.lpszClassName = window_title;

    gj_OnlyDebug(BOOL ok = )RegisterClassA(&window_class);
    gj_AssertDebug(ok);

    *window = CreateWindowExA(
        WS_EX_APPWINDOW,
        window_class.lpszClassName,
        window_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0);
    gj_Assert(*window);
    ShowWindow(*window, SW_SHOW);
}

#endif
