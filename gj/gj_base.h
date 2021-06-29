#if !defined(GJ_BASE_H)
#define GJ_BASE_H

#include <cstring> // memset

#define STB_SPRINTF_IMPLEMENTATION
#include <libs/stb/stb_sprintf.h>

///////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////
#include <stdint.h>

typedef int32_t b32;
typedef float f32;
typedef double f64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 one_byte;

#define internal static
#define local_persist static
#define global_variable static

///////////////////////////////////////////////////////////////////////////
// Util
///////////////////////////////////////////////////////////////////////////
#define Assert(expression)                      \
    do                                          \
    {                                           \
        if(!(expression)) *(int*)0 = 0;         \
    } while(0)
#define InvalidCodePath Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define gj_BitmaskU32 (0xFFFFFFFF)

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

inline u32
gj_safe_cast_u64_to_u32(u64 value)
{ Assert(value <= gj_BitmaskU32); return (u32)(value & gj_BitmaskU32); }

#define IsCharacter(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define IsDigit(c) (c >= '0' && c <= '9')
#define IsWhitespace(c) (c == ' ' || c == '\n' || c == '\r' || c == '\t')

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */

#define BUFFER_SIZE 512

#define gj_SwapVar(type, x, y) do {##type __tmp = x; x = y; y = __tmp;} while(false)
#define gj_SwapArray(array, type, i, j) do {##type __tmp = array[i]; array[i] = array[j]; array[j] = __tmp;} while(false)

#if defined(GJ_DEBUG)
#define brk do { int ______ = 0; ______++; } while(false)
#endif

#if defined(GJ_DEBUG) && defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

struct DebugTimer
{
    b32 active;
    char name[BUFFER_SIZE];
    u64 current;
    f32 total;
    f32 total_this_frame;
    u64 count;
};

global_variable u32        g_debug_timer_frame_count = 0;
global_variable DebugTimer g_debug_timers[20] =
{
    {true, "Frame", 0, 0.0f, 0.0f, 0} // Note: Used to measure each frame
};

internal DebugTimer*
gj_get_timer(const char* name)
{
    DebugTimer* result = NULL;
    u32 debug_timer_index = 0; 
    while (debug_timer_index < ArrayCount(g_debug_timers) &&
           strcmp(g_debug_timers[debug_timer_index].name, name) != 0) { debug_timer_index++; }
    if (debug_timer_index < ArrayCount(g_debug_timers))
    {
        result = &g_debug_timers[debug_timer_index];
    }
    return result;
}

internal void
gj_start_timer(const char* name)
{
    DebugTimer* debug_timer = gj_get_timer(name);
    if (!debug_timer)
    {
        u32 debug_timer_index = 0;
        while (debug_timer_index < ArrayCount(g_debug_timers) &&
               g_debug_timers[debug_timer_index].active) { debug_timer_index++; }
        Assert(debug_timer_index < ArrayCount(g_debug_timers));
        
        g_debug_timers[debug_timer_index] = {};
        g_debug_timers[debug_timer_index].active = true;                
        strcpy_s(g_debug_timers[debug_timer_index].name, name);
        debug_timer = &g_debug_timers[debug_timer_index];
    }
    Assert(debug_timer->active);
    
    LARGE_INTEGER perf_counter_start; QueryPerformanceCounter(&perf_counter_start);
    debug_timer->current = perf_counter_start.QuadPart;
    debug_timer->count++;
}

internal void
gj_end_timer(const char* name)
{
    DebugTimer* debug_timer = gj_get_timer(name);
    Assert(debug_timer);

    LARGE_INTEGER perf_counter_end; QueryPerformanceCounter(&perf_counter_end);
    LARGE_INTEGER perf_freq;        QueryPerformanceFrequency(&perf_freq);
    f32 current_time = (f32)(perf_counter_end.QuadPart - debug_timer->current) / (f32)perf_freq.QuadPart;
    debug_timer->total += current_time;
    debug_timer->total_this_frame += current_time;
}

internal void
gj_timers_start_frame()
{
    g_debug_timer_frame_count++;
    
    gj_start_timer("Frame");
    for (u32 debug_timer_index = 0;
         debug_timer_index < ArrayCount(g_debug_timers);
         debug_timer_index++)
    {
        DebugTimer* debug_timer = &g_debug_timers[debug_timer_index];
        debug_timer->total_this_frame = 0.0f;
    }
}

internal void
gj_timers_end_frame()
{
    gj_end_timer("Frame");
}

#else

struct DebugTimer;
internal DebugTimer* gj_get_timer(const char* name) { }
internal void gj_start_timer(const char* name) { }
internal void gj_end_timer(const char* name) { }
internal void gj_timers_start_frame() { }
internal void gj_timers_end_frame() { }

#endif

inline void gj_set_flag  (u32* flags, u32 flag) { *flags |= (1 << flag); }
inline void gj_unset_flag(u32* flags, u32 flag) { *flags &= ~(gj_BitmaskU32 & (1 << flag)); }
inline b32  gj_get_flag  (u32 flags,  u32 flag) { return flags & (1 << flag); }

s32 get_s32_length(s32 number)
{
    s32 result = 1;
    s32 tmp = number;
    while(tmp > 9)
    {
        tmp /= 10;
        result++;
    }
    return result;
}

void build_binary_string(u32 number, char* buffer, u32 max_size, u32 padding)
{
    if (number == 0)
    {
        for (u32 i = 0; i < padding; i++)
        {
            buffer[i] = '0';
        }
    }                                                                   
    else {
        for (u32 i = 0; i < padding; i++) {
            u32 bit_position = padding - 1 - i;
            b32 is_one = ((s32)number & (1 << bit_position)) == (1 << bit_position);
            buffer[i] = (is_one ? '1' : '0');
        }
    }
    buffer[padding] = '\0';
}

///////////////////////////////////////////////////////////////////////////
// Memory
///////////////////////////////////////////////////////////////////////////
struct MemoryArena
{
    size_t size;
    u8* base;
    size_t used;
};

#define BeginTemporaryMemoryBlock(arena)size_t tmp_used = (arena)->used
#define EndTemporaryMemoryBlock(arena) (arena)->used = tmp_used

void initialize_arena(MemoryArena* arena, size_t size, u8* base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

void clear_arena(MemoryArena* arena)
{
    memset(arena->base, 0, arena->size);
    arena->used = 0;
}

#define push_size(arena, size)_push(arena, size)
#define push_array(arena, type, count)(type*)push_size(arena, sizeof(type) * count)
#define push_struct(arena, type) (type*)_push(arena, sizeof(type))
void* _push(MemoryArena* arena, size_t size, size_t alignment = 4)
{
    size_t result_pointer = (size_t)arena->base + arena->used;
    
    size_t alignment_offset = 0;
    
    size_t alignment_mask = alignment - 1;
    if (result_pointer & alignment_mask)
    {
        alignment_offset = alignment - result_pointer & alignment_mask;
    }
    size += alignment_offset;
    
    Assert(arena->used + size <= arena->size);
    arena->used += size;

    void* result = (void*)(result_pointer + alignment_offset);

    memset(result, 0, size);
    
    return result;
}

///////////////////////////////////////////////////////////////////////////
// OS API
///////////////////////////////////////////////////////////////////////////
enum PlatformOpenFileModeFlags
{
    PlatformOpenFileModeFlags_Read  = 0x1,
    PlatformOpenFileModeFlags_Write = 0x2,
};

struct PlatformFileHandle
{
    void* handle;
    u32 file_size;
    // TODO: Remove?
    char* full_file_name;
};

struct PlatformFileListing
{
    char* file_name;
    PlatformFileListing* next;
};

struct PlatformThreadContext
{
    void (*thread_func)(void*);
    void* param;
    void* platform;
};

enum ThreadStatus
{
    ThreadStatus_Done    = 0,
    ThreadStatus_Running
};

struct TicketMutex
{
    u64 volatile ticket;
    u64 volatile serving;
};

typedef PlatformFileHandle   GetFileHandle(const char* file_name, u8 mode_flags);
typedef void                 ReadDataFromFileHandle(PlatformFileHandle file_handle, size_t offset, size_t size, void* dst);
typedef void                 WriteDataToFileHandle(PlatformFileHandle file_handle, size_t offset, size_t size, void* src);
typedef void                 CloseFileHandle(PlatformFileHandle file_handle);
typedef PlatformFileListing* ListFiles(MemoryArena* memory_arena, const char* file_name_pattern);
typedef void*                AllocateMemory(size_t size);
typedef void                 DeallocateMemory(void* memory);
typedef void                 NewThread(PlatformThreadContext* thread_context);
typedef b32                  WaitForThreads(PlatformThreadContext* threads, u32 thread_count);
typedef ThreadStatus         CheckThreadStatus(PlatformThreadContext thread_context);
typedef void                 BeginTicketMutex(TicketMutex* ticket_mutex);
typedef void                 EndTicketMutex(TicketMutex* ticket_mutex);
#if GJ_DEBUG
typedef void                 DebugPrint(const char* format, ...);
#endif

///////////////////////////////////////////////////////////////////////////
// Audio API
///////////////////////////////////////////////////////////////////////////
struct SoundBuffer
{
    int16_t* buffer;
    size_t buffer_size;
    uint32_t count;
    void* platform;
};

typedef u32         GetMaxQueuedSoundBuffers();
typedef u32         QueuedSoundBuffers();
typedef SoundBuffer CreatesoundBuffer(uint32_t sample_count);
typedef void        SubmitSoundBuffer(SoundBuffer sound_buffer);
typedef u32         GetRemainingSamples(SoundBuffer sound_buffer);

///////////////////////////////////////////////////////////////////////////
// PlatformAPI
///////////////////////////////////////////////////////////////////////////
struct PlatformAPI
{
    // OS API
    GetFileHandle*          get_file_handle;
    ReadDataFromFileHandle* read_data_from_file_handle;
    WriteDataToFileHandle*  write_data_to_file_handle;
    CloseFileHandle*        close_file_handle;
    ListFiles*              list_files;
    AllocateMemory*         allocate_memory;
    DeallocateMemory*       deallocate_memory;
    NewThread*              new_thread;
    WaitForThreads*         wait_for_threads;
    CheckThreadStatus*      check_thread_status;
    BeginTicketMutex*       begin_ticket_mutex;
    EndTicketMutex*         end_ticket_mutex;
#if GJ_DEBUG
    DebugPrint*             debug_print;
#endif

    // Audio API
    GetMaxQueuedSoundBuffers* get_max_queued_sound_buffers;
    QueuedSoundBuffers*       queued_sound_buffers;
    CreatesoundBuffer*        create_sound_buffer;
    SubmitSoundBuffer*        submit_sound_buffer;
    GetRemainingSamples*      get_remaining_samples;
        
    // Memory
    void* memory;
    size_t memory_size;
        
    b32 should_update_window_size;
    u32 window_width;
    u32 window_height;
};

global_variable PlatformAPI g_platform_api;

#endif
