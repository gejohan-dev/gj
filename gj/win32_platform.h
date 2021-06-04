#if !defined(WIN32_PLATFORM_H)
#define WIN32_PLATFORM_H

#include <stdio.h> // vsprintf_s
#include <malloc.h>

#include <gj/gj_base.h> // PlatformAPI

///////////////////////////////////////////////////////////////////////////
// OS API
///////////////////////////////////////////////////////////////////////////
PlatformFileHandle win32_get_file_handle(const char* file_name, u8 mode_flags)
{
    PlatformFileHandle result = {};
    Assert(sizeof(HANDLE) <= sizeof(result.handle));

    DWORD handle_permissions = 0;
    DWORD handle_creation = 0;
    if (mode_flags & PlatformOpenFileModeFlags_Read)
    {
        handle_permissions |= GENERIC_READ;
        handle_creation    = OPEN_EXISTING;

        WIN32_FILE_ATTRIBUTE_DATA file_attribute_data;
        Assert(GetFileAttributesExA(file_name, GetFileExInfoStandard, &file_attribute_data));
        // TODO: If files ever get this big I need to implement handling when reading data by DWORD in ReadFile
        Assert(file_attribute_data.nFileSizeHigh == 0);
        result.file_size = file_attribute_data.nFileSizeLow;
    }
    else if (mode_flags & PlatformOpenFileModeFlags_Write)
    {
        handle_permissions |= GENERIC_WRITE;
        handle_creation    = OPEN_ALWAYS;
        result.file_size = 0;
    }
    else InvalidCodePath;

    char buffer[BUFFER_SIZE];
    DWORD file_name_size = GetFullPathNameA(file_name, BUFFER_SIZE, buffer, NULL);
    result.full_file_name = (char*)g_platform_api.allocate_memory(file_name_size+1);
    memcpy(result.full_file_name, buffer, file_name_size+1);

    result.handle = (void*)CreateFileA(result.full_file_name, handle_permissions,
                                       FILE_SHARE_READ, 0, handle_creation, FILE_ATTRIBUTE_NORMAL, 0);
    Assert(result.handle != INVALID_HANDLE_VALUE);
    
    return result;
}

void win32_read_data_from_file_handle(PlatformFileHandle* file_handle, u32 offset, u32 size, void* dst)
{
    HANDLE handle = (HANDLE)file_handle->handle;

    OVERLAPPED overlapped = {};
    overlapped.Offset = offset;
    // NOTE: If offset becomes 64 bit I need to do:
    // overlapped.OffsetHigh = (u32)((offset >> 32) & 0xFFFFFFFF);

    DWORD bytes_read;
    Assert(ReadFile(handle, dst, size, &bytes_read, &overlapped));
    Assert(bytes_read == size);
}

void win32_write_data_to_file_handle(PlatformFileHandle* file_handle, u32 offset, u32 size, void* src)
{
    HANDLE handle = (HANDLE)file_handle->handle;

    OVERLAPPED overlapped = {};
    overlapped.Offset = offset;
    // NOTE: If offset becomes 64 bit I need to do:
    // overlapped.OffsetHigh = (u32)((offset >> 32) & 0xFFFFFFFF);

    DWORD bytes_written;
    Assert(WriteFile(handle, src, size, &bytes_written, &overlapped));
    Assert(bytes_written == size);
}

void win32_close_file_handle(PlatformFileHandle* file_handle)
{
    Assert(CloseHandle(file_handle->handle));
    g_platform_api.deallocate_memory(file_handle->full_file_name);
}

PlatformFileListing* win32_list_files(MemoryArena* memory_arena, const char* file_name_pattern)
{
    PlatformFileListing* result = 0;

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA(file_name_pattern, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE)
    {
        result = push_struct(memory_arena, PlatformFileListing);
    }
    
    PlatformFileListing* current = result;
    while (find_handle != INVALID_HANDLE_VALUE)
    {
        current->file_name = (char*)push_array(memory_arena, char, MAX_PATH);
        memcpy(current->file_name, find_data.cFileName, MAX_PATH);

        if (!FindNextFileA(find_handle, &find_data))
        {
            break;
        }
        else
        {
            current->next = push_struct(memory_arena, PlatformFileListing);
            current = current->next;
        }
    }
    FindClose(find_handle);
    
    return result;
}

void* win32_allocate_memory(size_t size)
{
#if 0
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#endif
    
#if 0
    void* result = HeapAlloc(GetProcessHeap(),
                             HEAP_ZERO_MEMORY
                             | HEAP_NO_SERIALIZE
#if GJ_DEBUG
                             | HEAP_GENERATE_EXCEPTIONS
#endif
                             ,
                             size);
#endif
    void* result = malloc(size);
    Assert(result);
    return result;
}

void win32_deallocate_memory(void* memory)
{
    if (memory)
    {
#if 0
        // NOTE: https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree
        // If the dwFreeType parameter (third) is MEM_RELEASE, this parameter (second) must be 0 (zero).
        /* Assert(VirtualFree(memory, 0, MEM_RELEASE)); */
        Assert(HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, memory));
#endif
        free(memory);
    }
}

DWORD WINAPI ThreadProc(LPVOID param)
{
    PlatformThreadContext* thread_context = (PlatformThreadContext*)param;
    thread_context->thread_func(thread_context->param);
    return 0;
}

struct Win32Thread
{
    DWORD id;
    HANDLE handle;
};

void win32_new_thread(PlatformThreadContext* thread_context)
{
    DWORD id;
    HANDLE thread_handle = CreateThread(NULL, 0, ThreadProc, thread_context, 0, &id);
    thread_context->platform = g_platform_api.allocate_memory(sizeof(Win32Thread));
    ((Win32Thread*)thread_context->platform)->id     = id;
    ((Win32Thread*)thread_context->platform)->handle = thread_handle;
}

b32 win32_wait_for_threads(PlatformThreadContext* threads, u32 thread_count)
{
    for (u32 thread_index = 0;
         thread_index < thread_count;
         thread_index++)
    {
        HANDLE thread_handle = ((Win32Thread*)threads[thread_index].platform)->handle;
        WaitForSingleObjectEx(thread_handle, INFINITE, TRUE);
        g_platform_api.deallocate_memory(threads[thread_index].platform);
        CloseHandle(thread_handle);
    }
    
    return true;
}

ThreadStatus win32_check_thread_status(PlatformThreadContext thread_context)
{
    ThreadStatus result = ThreadStatus_Running;
    if (thread_context.platform)
    {
        DWORD exit_code;
        HANDLE handle = ((Win32Thread*)thread_context.platform)->handle;
        Assert(GetExitCodeThread(handle, &exit_code));
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

void win32_debug_print(const char* format, ...)
{
    va_list varargs;
    va_start(varargs, format);
    char buffer[BUFFER_SIZE];
    vsprintf_s(buffer, format, varargs);
    OutputDebugStringA(buffer);
    va_end(varargs);
}

///////////////////////////////////////////////////////////////////////////
// Audio API
///////////////////////////////////////////////////////////////////////////
#include <xaudio2.h>
struct Win32XAudio2
{
    IXAudio2* context;
    IXAudio2MasteringVoice* mastering_voice;
    WAVEFORMATEX wave_format_ext;
    IXAudio2SourceVoice* source_voice;
};

global_variable Win32XAudio2 g_win32_xaudio2;

u32 win32_get_max_queued_sound_buffers()
{
    return XAUDIO2_MAX_QUEUED_BUFFERS;
}

u32 win32_queued_sound_buffers()
{
    u32 result;
    XAUDIO2_VOICE_STATE xaudio2_voice_state;
    g_win32_xaudio2.source_voice->GetState(&xaudio2_voice_state, 0);
    result = xaudio2_voice_state.BuffersQueued;
    return result;
}

SoundBuffer win32_create_sound_buffer(uint32_t sample_count)
{
    SoundBuffer result;
    
    size_t buffer_size = (sizeof(int16_t) * sample_count * g_win32_xaudio2.wave_format_ext.nChannels);
    result.buffer = (int16_t*)g_platform_api.allocate_memory(buffer_size);
    result.buffer_size = buffer_size;
    result.count  = sample_count;

    result.platform = g_platform_api.allocate_memory(sizeof(XAUDIO2_BUFFER));
    XAUDIO2_BUFFER* xaudio2_buffer = (XAUDIO2_BUFFER*)result.platform;
    xaudio2_buffer->Flags      = 0;
    // TODO: Ensure safe assignment
    xaudio2_buffer->AudioBytes = (UINT32)buffer_size;
    xaudio2_buffer->pAudioData = (const BYTE*)result.buffer;
    xaudio2_buffer->PlayBegin  = 0;
    xaudio2_buffer->PlayLength = 0;
    xaudio2_buffer->LoopBegin  = 0;
    xaudio2_buffer->LoopLength = 0;
    xaudio2_buffer->LoopCount  = 0;
    xaudio2_buffer->pContext   = 0;

    return result;
}

void win32_submit_sound_buffer(SoundBuffer sound_buffer)
{
    XAUDIO2_BUFFER* xaudio2_buffer = (XAUDIO2_BUFFER*)sound_buffer.platform;
    HRESULT hr = g_win32_xaudio2.source_voice->SubmitSourceBuffer(xaudio2_buffer);
    Assert(SUCCEEDED(hr));
}

u32 win32_get_remaining_samples(SoundBuffer sound_buffer)
{
    u32 result;
    XAUDIO2_VOICE_STATE xaudio2_voice_state;
    g_win32_xaudio2.source_voice->GetState(&xaudio2_voice_state, 0);
    result = xaudio2_voice_state.SamplesPlayed % sound_buffer.count;
    return result;
}

///////////////////////////////////////////////////////////////////////////
// Init
///////////////////////////////////////////////////////////////////////////
void win32_init_platform_api(PlatformAPI* platform_api)
{
    platform_api->get_file_handle            = win32_get_file_handle;
    platform_api->read_data_from_file_handle = win32_read_data_from_file_handle;
    platform_api->write_data_to_file_handle  = win32_write_data_to_file_handle;
    platform_api->close_file_handle          = win32_close_file_handle;
    platform_api->list_files                 = win32_list_files;
    platform_api->allocate_memory            = win32_allocate_memory;
    platform_api->deallocate_memory          = win32_deallocate_memory;
    platform_api->new_thread                 = win32_new_thread;
    platform_api->wait_for_threads           = win32_wait_for_threads;
    platform_api->check_thread_status        = win32_check_thread_status;
    platform_api->begin_ticket_mutex         = win32_begin_ticket_mutex;
    platform_api->end_ticket_mutex           = win32_end_ticket_mutex;
#if GJ_DEBUG
    platform_api->debug_print                = win32_debug_print;
#endif

    platform_api->get_max_queued_sound_buffers = win32_get_max_queued_sound_buffers;
    platform_api->queued_sound_buffers         = win32_queued_sound_buffers;
    platform_api->create_sound_buffer          = win32_create_sound_buffer;
    platform_api->submit_sound_buffer          = win32_submit_sound_buffer;
    platform_api->get_remaining_samples        = win32_get_remaining_samples;
}

#endif
