#if !defined(WIN32_UTIL_H)
#define WIN32_UTIL_H

// Note: Requires
// #define WIN32_EXE_FILE_NAME [exe file name e.g. "win32_app.exe"]
// #include "win32_util.h"

#if !defined(WIN32_EXE_FILE_NAME)
#define WIN32_EXE_FILE_NAME NULL
#endif

global_variable int64_t         g_perf_count_frequency;
global_variable WINDOWPLACEMENT g_window_pos = {sizeof(g_window_pos)};
struct Win32App
{
    HWND window;
    HDC device_context;
    V2u window_dimensions;
};
global_variable Win32App        g_win32_app;

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

inline float
win32_get_time_difference(LARGE_INTEGER start, LARGE_INTEGER end)
{
    float result = ((float)(end.QuadPart - start.QuadPart) /
                  (float)g_perf_count_frequency);
    return result;
}

void win32_toggle_fullscreen(HWND window)
{
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        if(GetWindowPlacement(window, &g_window_pos) &&
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
        SetWindowPlacement(window, &g_window_pos);
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
    if (GetFileAttributesExA(file_name, GetFileExInfoStandard, &file_attribute_data))
    {
        result = file_attribute_data.ftLastWriteTime;
    }
    else InvalidCodePath;
    return result;
}

///////////////////////////////////////////////////////////////////////////
// win32_build_exe_file_path
// Builds the path
// c:\Path\To\exe_file_location\file_name
///////////////////////////////////////////////////////////////////////////
void win32_build_exe_file_path(char* output_path, const char* file_name)
{
    char exe_base_path[BUFFER_SIZE];
    size_t exe_base_path_length = GetModuleFileNameA(NULL, exe_base_path, ArrayCount(exe_base_path));
    Assert(exe_base_path_length < ArrayCount(exe_base_path));
    // TODO: Implement error
    Assert(WIN32_EXE_FILE_NAME);
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
    char _ignored[BUFFER_SIZE];
    if (!GetFileAttributesExA(dll->lock_file_path, GetFileExInfoStandard, &_ignored))
    {
        // Note: This is done to not lock the dll for writing and
        //       also keep old dlls since the program could be
        //       pointing to things in the old dll
        char tmp_dll_path[BUFFER_SIZE];
        char unique_tmp_file_name[BUFFER_SIZE];
        sprintf_s(unique_tmp_file_name, ArrayCount(unique_tmp_file_name), "tmp%d.dll", dll->tmp_dll_number);
        dll->tmp_dll_number++;
        win32_build_exe_file_path(tmp_dll_path, unique_tmp_file_name);
        Assert(CopyFileA(dll->dll_file_path, tmp_dll_path, FALSE));
        
        dll->last_write_time = win32_get_last_write_time(dll->dll_file_path);
        dll->hmodule = LoadLibraryA(tmp_dll_path);
        Assert(dll->hmodule);
    }
}

void win32_load_dll(Win32HotLoadedDLL* dll, char* dll_file_path, char* lock_file_path)
{
    dll->dll_file_path  = dll_file_path;
    dll->lock_file_path = lock_file_path;
    win32_load_dll(dll);
}

void win32_init_window(HINSTANCE instance, WNDPROC window_proc)
{
    WNDCLASSA window_class = {};
    window_class.style = CS_VREDRAW | CS_HREDRAW;
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursor(0, IDC_ARROW);
    window_class.lpszClassName = "MusicStudio";

    Assert(RegisterClassA(&window_class));

    g_win32_app.window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "AudioGame",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        0,
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0);
    Assert(g_win32_app.window);

    g_win32_app.device_context = GetDC(g_win32_app.window);
}

#endif
