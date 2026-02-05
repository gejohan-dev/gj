#if !defined(WIN32_PLATFORM_H)
#define WIN32_PLATFORM_H

#include <gj/gj_base.h> // PlatformAPI

#if defined(GJ_DEBUG)
static void
win32_assert(bool exp)
{
    if (!exp)
    {
        char* buffer = 0;
        DWORD error = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                       NULL, 
                       error,
                       0,
                       (LPTSTR)&buffer, 
                       0, 
                       NULL);
        MessageBoxA(NULL, buffer, "Error", MB_OK);
        __debugbreak();
        ExitProcess(1);
    }
}
#else
#define win32_assert(___)
#endif

///////////////////////////////////////////////////////////////////////////
// OS API
///////////////////////////////////////////////////////////////////////////
void* win32_allocate_memory(size_t size);
void  win32_deallocate_memory(void* memory);
void  win32_log_error(char* file, char* function, s32 line, char* format, ...);

PlatformFileHandle win32_get_file_handle(const char* file_name, u8 mode_flags)
{
    PlatformFileHandle result;
    gj__ZeroStruct(result);

    DWORD handle_permissions = 0;
    DWORD handle_creation = 0;
    
    gj_AssertDebug((mode_flags & PlatformOpenFileModeFlags_Read) || (mode_flags & PlatformOpenFileModeFlags_Write));
    
    if (mode_flags & PlatformOpenFileModeFlags_Read)
    {
        handle_permissions |= GENERIC_READ;
        handle_creation    = OPEN_EXISTING;
    }

    if (mode_flags & PlatformOpenFileModeFlags_Write)
    {
        handle_permissions |= GENERIC_WRITE;
        if (mode_flags & PlatformOpenFileModeFlags_Overwrite)
        {
            handle_creation = CREATE_ALWAYS;
        }
        else
        {
            handle_creation = OPEN_ALWAYS;
        }
    }

    char buffer[BUFFER_SIZE];
    DWORD file_name_size = GetFullPathNameA(file_name, BUFFER_SIZE, buffer, NULL);
    result.full_file_name = (char*)win32_allocate_memory(file_name_size+1);
    memcpy(result.full_file_name, buffer, file_name_size+1);

    // TODO: Add FILE_FLAG_SEQUENTIAL_SCAN to dwFlagsAndAttributes since most of the time
    //       the app will just read the file top-to-bottom?
    *(HANDLE*)&result.handle = CreateFileA(result.full_file_name, handle_permissions,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE, 0, handle_creation, FILE_ATTRIBUTE_NORMAL, 0);

    if (result.handle == INVALID_HANDLE_VALUE)
    {
        result.handle = PLATFORM_INVALID_FILE_HANDLE;
    }
    else
    {
        DWORD file_size_high;
        result.file_size = GetFileSize(result.handle, &file_size_high);
    }
    
    return result;
}

void win32_read_data_from_file_handle(PlatformFileHandle file_handle, u64 offset, u64 size, void* dst)
{
    HANDLE handle = *(HANDLE*)&file_handle.handle;
    
    OVERLAPPED overlapped;
    gj__ZeroStruct(overlapped);
    overlapped.Offset = offset & 0xFFFFFFFF;
    overlapped.OffsetHigh = (u32)((offset >> 32) & 0xFFFFFFFF);

    DWORD bytes_read;
    gj_OnlyDebug(BOOL ok = )ReadFile(handle, dst, gj_safe_cast_u64_to_u32(size), &bytes_read, &overlapped);
#if defined(GJ_DEBUG)
    int x = GetLastError();
#endif
    gj_AssertDebug(ok);
    gj_AssertDebug(bytes_read == size);
}

void win32_write_data_to_file_handle(PlatformFileHandle file_handle, u64 offset, size_t size, void* src)
{
    HANDLE handle = (HANDLE)file_handle.handle;

    OVERLAPPED overlapped;
    gj__ZeroStruct(overlapped);
    overlapped.Offset = gj_safe_cast_u64_to_u32(offset);
    overlapped.OffsetHigh = (u32)((offset >> 32) & 0xFFFFFFFF);

    DWORD bytes_written;
    gj_OnlyDebug(BOOL ok = )WriteFile(handle, src, gj_safe_cast_u64_to_u32(size), &bytes_written, &overlapped);
#if defined(GJ_DEBUG)
    int x = GetLastError();
#endif
    gj_AssertDebug(ok);
    gj_AssertDebug(bytes_written == size);
}

void win32_close_file_handle(PlatformFileHandle file_handle)
{
    gj_OnlyDebug(BOOL ok = )CloseHandle(file_handle.handle);
    gj_AssertDebug(ok);
    win32_deallocate_memory(file_handle.full_file_name);
}

u32 win32_read_whole_file(const char* file_name, void* dst)
{
    u32 result = 0;
    PlatformFileHandle file_handle = win32_get_file_handle(file_name, PlatformOpenFileModeFlags_Read);
    if (file_handle.handle != NULL)
    {
        result = file_handle.file_size;
        win32_read_data_from_file_handle(file_handle, 0, file_handle.file_size, dst);
        win32_close_file_handle(file_handle);
    }
    else
    {
        win32_log_error(__FILE__, __FUNCTION__, __LINE__, "win32_read_whole_file failed to read %s", file_name);
    }
    return result;
}

PlatformFileListing* win32_list_files(void* memory, size_t memory_max_size, const char* file_name_pattern)
{
    PlatformFileListing* result = 0;

    MemoryArena memory_arena;
    initialize_arena(&memory_arena, memory_max_size, (u8*)memory);
    
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA(file_name_pattern, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        result = push_struct(&memory_arena, PlatformFileListing);
    }
    
    PlatformFileListing* current = result;
    while (find_handle != INVALID_HANDLE_VALUE)
    {
        current->file_name = (char*)push_array(&memory_arena, char, MAX_PATH);
        memcpy(current->file_name, find_data.cFileName, MAX_PATH);

        if (!FindNextFileA(find_handle, &find_data))
        {
            break;
        }
        else
        {
            current->next = push_struct(&memory_arena, PlatformFileListing);
            current = current->next;
        }
    }
    FindClose(find_handle);
    
    return result;
}

FileTime win32_get_file_last_write_time(const char* file_name)
{
    FileTime result;

    FILETIME filetime = {};
    HANDLE file_handle = CreateFileA(file_name, 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!GetFileTime(file_handle, NULL, NULL, &filetime))
    {
        DWORD error = GetLastError();
        InvalidCodePath;
    }
    CloseHandle(file_handle);
    
    SYSTEMTIME system_time;
    FileTimeToSystemTime(&filetime, &system_time);
    result.second = system_time.wSecond;
    result.minute = system_time.wMinute;
    result.hour = system_time.wHour;
    result.day = system_time.wDay;
    result.month = system_time.wMonth;
    result.year = system_time.wYear;

    result.compare_value = ((u64)filetime.dwHighDateTime << 32) | filetime.dwLowDateTime;
    
    return result;
}

b32 win32_check_file_exists(const char* file_name)
{
    DWORD attributes = GetFileAttributes(file_name);
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

void* win32_allocate_memory(size_t size)
{
#if 0
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
    
#if 1
    void* result = HeapAlloc(GetProcessHeap(),
                             HEAP_ZERO_MEMORY
#if GJ_DEBUG
                             | HEAP_GENERATE_EXCEPTIONS
#endif
                             ,
                             size);
#endif

#if 0
    void* result = malloc(size);
    memset(result, 0, size);
#endif
    
    gj_Assert(result);
    return result;
}

void win32_deallocate_memory(void* memory)
{
    if (memory)
    {
#if 0
        VirtualFree(memory, 0, MEM_RELEASE);
#endif
        
#if 1
        // NOTE: https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree
        // If the dwFreeType parameter (third) is MEM_RELEASE, this parameter (second) must be 0 (zero).
        /* Assert(VirtualFree(memory, 0, MEM_RELEASE)); */
        gj_OnlyDebug(BOOL ok = )HeapFree(GetProcessHeap(), 0, memory);
        gj_AssertDebug(ok);
#endif

#if 0
        free(memory);
#endif
    }
}

DWORD WINAPI ThreadProc(LPVOID param)
{
    PlatformThreadContext* thread_context = (PlatformThreadContext*)param;
    thread_context->thread_func(thread_context->param);
    return 0;
}

typedef struct Win32Thread
{
    DWORD id;
    HANDLE handle;
} Win32Thread;

void win32_new_thread(PlatformAPI* platform_api, PlatformThreadContext* thread_context)
{
    DWORD id;
    HANDLE thread_handle = CreateThread(NULL, 0, ThreadProc, thread_context, 0, &id);
    thread_context->platform = platform_api->allocate_memory(sizeof(Win32Thread));
    ((Win32Thread*)thread_context->platform)->id     = id;
    ((Win32Thread*)thread_context->platform)->handle = thread_handle;
}

b32 win32_wait_for_threads(PlatformAPI* platform_api, PlatformThreadContext* threads, u32 thread_count)
{
    for (u32 thread_index = 0;
         thread_index < thread_count;
         thread_index++)
    {
        HANDLE thread_handle = ((Win32Thread*)threads[thread_index].platform)->handle;
        WaitForSingleObjectEx(thread_handle, INFINITE, TRUE);
        platform_api->deallocate_memory(threads[thread_index].platform);
        CloseHandle(thread_handle);
    }
    
    return gj_True;
}

ThreadStatus win32_check_thread_status(PlatformThreadContext thread_context)
{
    ThreadStatus result = ThreadStatus_Running;
    if (thread_context.platform)
    {
        DWORD exit_code;
        HANDLE handle = ((Win32Thread*)thread_context.platform)->handle;
        gj_OnlyDebug(BOOL ok = )GetExitCodeThread(handle, &exit_code);
        gj_AssertDebug(ok);
        if (exit_code == 0) result = ThreadStatus_Done;
    }
    return result;
}

void win32_begin_ticket_mutex(TicketMutex* ticket_mutex)
{
    u64 ticket = InterlockedExchangeAdd64((volatile LONG64*)&ticket_mutex->ticket, 1);
    // NOTE: _mm_pause = PAUSE Intrinsic
    //       https://scc.ustc.edu.cn/zlsc/chinagrid/intel/compiler_c/main_cls/GUID-3488E3C1-33C3-4444-9D72-CB428DCA3658.htm
    while (ticket != ticket_mutex->serving) _mm_pause();
}

void win32_end_ticket_mutex(TicketMutex* ticket_mutex)
{
    InterlockedExchangeAdd64((volatile LONG64*)&ticket_mutex->serving, 1);
}

HANDLE win32_get_stdout_handle()
{
    HANDLE result = CreateFileA("CONOUT$",
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ| FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    gj_AssertDebug(result != INVALID_HANDLE_VALUE);
    return result;
}

static HANDLE g_stdout_handle = NULL;
static PlatformFileHandle g_log_file_handle = {};
void _write_to_stdout(char* buffer, u64 buffer_size)
{
    if (!g_stdout_handle) g_stdout_handle = win32_get_stdout_handle();
    if (!g_log_file_handle.handle) g_log_file_handle = win32_get_file_handle("logs", PlatformOpenFileModeFlags_Write);
    WriteFile(g_stdout_handle, buffer, gj_safe_cast_u64_to_u32(buffer_size), NULL, NULL);
    if (g_log_file_handle.file_size > Megabytes(1))
    {
        g_log_file_handle.file_size = 0;
    }
    win32_write_data_to_file_handle(g_log_file_handle, g_log_file_handle.file_size, buffer_size, buffer);
    g_log_file_handle.file_size += gj_safe_cast_u64_to_u32(buffer_size);
}

void win32_log_error(char* file, char* function, s32 line, char* format, ...)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    va_list varargs;
    va_start(varargs, format);
    char buffer[BUFFER_SIZE * 2];
    stbsp_vsnprintf(buffer, sizeof(buffer), format, varargs);
    va_end(varargs);

    char buffer2[BUFFER_SIZE * 4];
    u64 buffer2_size = stbsp_snprintf(buffer2, sizeof(buffer2), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] Error in %s:%s %d %s\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, file, function, line, buffer);
    _write_to_stdout(buffer2, buffer2_size);
}

void win32_log_info(char* file, char* function, s32 line, char* format, ...)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    va_list varargs;
    va_start(varargs, format);
    char buffer[BUFFER_SIZE * 2];
    stbsp_vsnprintf(buffer, sizeof(buffer), format, varargs);
    va_end(varargs);

    char buffer2[BUFFER_SIZE * 4];
    u64 buffer2_size = stbsp_snprintf(buffer2, sizeof(buffer2), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] Info %s:%s %d %s\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, file, function, line, buffer);
    _write_to_stdout(buffer2, buffer2_size);
}

void win32_debug_print(const char* format, ...)
{
    va_list varargs;
    va_start(varargs, format);
    char buffer[BUFFER_SIZE];
    u64 buffer_size = stbsp_vsprintf(buffer, format, varargs);
    _write_to_stdout(buffer, buffer_size);
    va_end(varargs);
}

///////////////////////////////////////////////////////////////////////////
// Audio API
///////////////////////////////////////////////////////////////////////////
#include <xaudio2.h>
typedef struct Win32XAudio2
{
    IXAudio2* context;
    IXAudio2MasteringVoice* mastering_voice;
    WAVEFORMATEX wave_format_ext;
} Win32XAudio2;

typedef struct Win32XAudio2Buffer
{
    XAUDIO2_BUFFER xaudio2_buffer;
    IXAudio2SourceVoice* source_voice;
} Win32XAudio2SourceVoice;

global_variable Win32XAudio2 g_win32_xaudio2;

///////////////////////////////////////////////////////////////////////////
// XAudio2 Implementation
///////////////////////////////////////////////////////////////////////////
void xaudio2_init()
{
    HRESULT hr;

    // COM
    {
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        gj_AssertDebug(SUCCEEDED(hr));
    }
        
    {
        uint32_t flags = 0;
#if WALK_PUZZLE_DEBUG
        flags |= XAUDIO2_DEBUG_ENGINE;
#endif
        g_win32_xaudio2.context = NULL;
        hr = XAudio2Create(&g_win32_xaudio2.context, flags);
        gj_AssertDebug(SUCCEEDED(hr));
    }

    {
        g_win32_xaudio2.mastering_voice = NULL;
        hr = g_win32_xaudio2.context->CreateMasteringVoice(&g_win32_xaudio2.mastering_voice);
        gj_AssertDebug(SUCCEEDED(hr));
    }
    
    g_win32_xaudio2.wave_format_ext = {};
    g_win32_xaudio2.wave_format_ext.wFormatTag      = WAVE_FORMAT_PCM;
    g_win32_xaudio2.wave_format_ext.nChannels       = 2;
    g_win32_xaudio2.wave_format_ext.nSamplesPerSec  = 48000;
    g_win32_xaudio2.wave_format_ext.wBitsPerSample  = 16;
    g_win32_xaudio2.wave_format_ext.cbSize          = 0;
        
    g_win32_xaudio2.wave_format_ext.nBlockAlign     = (g_win32_xaudio2.wave_format_ext.nChannels * g_win32_xaudio2.wave_format_ext.wBitsPerSample) / 8;
    g_win32_xaudio2.wave_format_ext.nAvgBytesPerSec = g_win32_xaudio2.wave_format_ext.nSamplesPerSec * g_win32_xaudio2.wave_format_ext.nBlockAlign;
}

SoundBuffer win32_create_sound_buffer(uint32_t sample_count)
{
    SoundBuffer result;
    
    size_t buffer_size = sizeof(int16_t) * sample_count;
    result.buffer = (int16_t*)win32_allocate_memory(buffer_size);
    result.buffer_size = buffer_size;
    result.count  = sample_count;

    result.platform = win32_allocate_memory(sizeof(Win32XAudio2Buffer));
    Win32XAudio2Buffer* win32_xaudio2_buffer = (Win32XAudio2Buffer*)result.platform;
    
    gj_ZeroMem(&win32_xaudio2_buffer->xaudio2_buffer, sizeof(XAUDIO2_BUFFER));
    win32_xaudio2_buffer->xaudio2_buffer.Flags      = 0;
    // TODO: Ensure safe assignment
    win32_xaudio2_buffer->xaudio2_buffer.AudioBytes = (UINT32)buffer_size;
    win32_xaudio2_buffer->xaudio2_buffer.pAudioData = (const BYTE*)result.buffer;
    win32_xaudio2_buffer->xaudio2_buffer.PlayBegin  = 0;
    win32_xaudio2_buffer->xaudio2_buffer.PlayLength = 0;
    win32_xaudio2_buffer->xaudio2_buffer.LoopBegin  = 0;
    win32_xaudio2_buffer->xaudio2_buffer.LoopLength = 0;
    win32_xaudio2_buffer->xaudio2_buffer.LoopCount  = 0;
    win32_xaudio2_buffer->xaudio2_buffer.pContext   = 0;

    HRESULT hr = g_win32_xaudio2.context->CreateSourceVoice(&win32_xaudio2_buffer->source_voice, &g_win32_xaudio2.wave_format_ext);
    gj_AssertDebug(SUCCEEDED(hr));
        
    return result;
}

void win32_submit_sound_buffer(SoundBuffer sound_buffer)
{
    HRESULT hr;

    Win32XAudio2Buffer* win32_xaudio2_buffer = (Win32XAudio2Buffer*)sound_buffer.platform;

    hr = win32_xaudio2_buffer->source_voice->Stop();
    gj_AssertDebug(SUCCEEDED(hr));
    if (sound_buffer.play_begin_in_samples == 0)
    {
        hr = win32_xaudio2_buffer->source_voice->FlushSourceBuffers();
        gj_AssertDebug(SUCCEEDED(hr));
    }
    
    win32_xaudio2_buffer->xaudio2_buffer.PlayBegin = sound_buffer.play_begin_in_samples;
    win32_xaudio2_buffer->xaudio2_buffer.PlayLength = sound_buffer.play_length_in_samples;
    win32_xaudio2_buffer->source_voice->SetVolume(sound_buffer.volume);

    hr = win32_xaudio2_buffer->source_voice->SubmitSourceBuffer(&win32_xaudio2_buffer->xaudio2_buffer);
    gj_AssertDebug(SUCCEEDED(hr));

    hr = win32_xaudio2_buffer->source_voice->Start(0);
    gj_AssertDebug(SUCCEEDED(hr));
}

///////////////////////////////////////////////////////////////////////////
// Init
///////////////////////////////////////////////////////////////////////////
void win32_init_platform_api(PlatformAPI* platform_api, size_t memory_size)
{
    memset(platform_api->_os_api, 0, sizeof(platform_api->_os_api));
    platform_api->get_file_handle            = win32_get_file_handle;
    platform_api->read_data_from_file_handle = win32_read_data_from_file_handle;
    platform_api->write_data_to_file_handle  = win32_write_data_to_file_handle;
    platform_api->close_file_handle          = win32_close_file_handle;
    platform_api->read_whole_file            = win32_read_whole_file;
    platform_api->list_files                 = win32_list_files;
    platform_api->get_file_last_write_time   = win32_get_file_last_write_time;
    platform_api->check_file_exists          = win32_check_file_exists;
    platform_api->allocate_memory            = win32_allocate_memory;
    platform_api->deallocate_memory          = win32_deallocate_memory;
    platform_api->new_thread                 = win32_new_thread;
    platform_api->wait_for_threads           = win32_wait_for_threads;
    platform_api->check_thread_status        = win32_check_thread_status;
    platform_api->begin_ticket_mutex         = win32_begin_ticket_mutex;
    platform_api->end_ticket_mutex           = win32_end_ticket_mutex;
    platform_api->log_error                  = win32_log_error;
    platform_api->log_info                   = win32_log_info;
#if GJ_DEBUG
    platform_api->debug_print                = win32_debug_print;
#endif
    gj_VerifyPlatformAPI((*platform_api));
    
    platform_api->create_sound_buffer          = win32_create_sound_buffer;
    platform_api->submit_sound_buffer          = win32_submit_sound_buffer;

    if (memory_size > 0)
    {
        platform_api->memory      = platform_api->allocate_memory(memory_size);
        platform_api->memory_size = memory_size;
    }
}

#endif
