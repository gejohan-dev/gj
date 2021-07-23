#if !defined(GJ_BASE_H)
#define GJ_BASE_H
//
// This file contains some common utilities/basic things I use in different
// projects.
//

/* #include <cstring> // memset */

#define STB_SPRINTF_IMPLEMENTATION
#include <libs/stb/stb_sprintf.h> // stbsp_sprintf

///////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////
#include <stdint.h>

typedef int32_t b32;

typedef float  f32;
typedef double f64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 one_byte;

#define internal        static
#define global_variable static

#define gj_True  1
#define gj_False 0

///////////////////////////////////////////////////////////////////////////
// Util
///////////////////////////////////////////////////////////////////////////
#if !defined(Assert)

#if defined(GJ_DEBUG)

#if defined(_MSC_VER)
#define gj_AssertFail() do { __debugbreak(); } while (0)
#else
#define gj_AssertFail() do { *(int*)0 = 0; } while (0)
#endif

#define gj_Assert(Exp)                          \
    do                                          \
    {                                           \
        if(!(Exp))                              \
        {                                       \
            gj_AssertFail();                    \
        }                                       \
    } while(0)

#else // !defined(GJ_DEBUG)
#define gj_Assert(Exp)
#endif

#else // !defined(Assert)
#define gj_Assert(Exp) Assert(Exp)
#endif

#define BUFFER_SIZE 512

#define InvalidCodePath gj_Assert(!"InvalidCodePath")
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define gj_BitmaskU32 (0xFFFFFFFF)

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)

#define gj_BytesToMegabytes(Bytes) ((double)(Bytes) / (double)(1024*1024))

#define gj_ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

inline u32
gj_safe_cast_u64_to_u32(u64 value)
{ gj_Assert(value <= gj_BitmaskU32); return (u32)(value & gj_BitmaskU32); }

#define gj_IsCharacter(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define gj_IsDigit(c) (c >= '0' && c <= '9')
#define gj_IsWhitespace(c) (c == ' ' || c == '\n' || c == '\r' || c == '\t')

#define gj_DigitToChar(d) ((d) + '0')
#define gj_CharToDigit(c) ((c) - '0')

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */

#define gj_SwapVar(type, x, y) do {##type __tmp = (x); (x) = (y); (y) = __tmp;} while(gj_False)
#define gj_SwapArray(array, type, i, j) do {##type __tmp = array[i]; array[i] = array[j]; array[j] = __tmp;} while(gj_False)

#define gj_ZeroMemory(Mem) do { memset(Mem, 0, sizeof(*(Mem))); } while(gj_False)

inline void gj_set_flag  (u32* flags, u32 flag) { *flags |= (1 << flag); }
inline void gj_unset_flag(u32* flags, u32 flag) { *flags &= ~(gj_BitmaskU32 & (1 << flag)); }
inline b32  gj_get_flag  (u32 flags,  u32 flag) { return flags & (1 << flag); }

inline u32 gj_string_length(char* s)
{
    u32 result = 0;
    while (*s++) result++;
    return result;
}

inline b32 gj_strings_equal_null_term(const char* s1, const char* s2)
{
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return !(*s1) && !(*s2);
}

inline b32 gj_strings_equal(const char* s1, const char* s2, s32 length)
{
    while (*s1 && *s1 == *s2) { s1++; s2++; length--; }
    return length == 0;
}

inline void gj_string_copy(char* dst, const char* src) { while (*src) { *dst++ = *src++; } }
                           
static s32 gj_get_s32_length(s32 number)
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

static void build_binary_string(u32 number, char* buffer, u32 max_size, u32 padding)
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

static s32 gj_parse_digit(char c)
{
    gj_Assert(gj_IsDigit(c));
    return gj_CharToDigit(c);
}

typedef struct GJParseNumber
{
    b32 ok;
    s32 number;
    s32 length;
} GJParseNumber;

static GJParseNumber gj_parse_number(char* s)
{
    GJParseNumber result;
    result.ok     = gj_True;
    result.number = 0;
    result.length = 0;

    if (!gj_IsDigit(*s))
    {
        result.ok = gj_False;
    }
    else
    {
        while (gj_IsDigit(*s))
        {
            result.number *= 10;
            result.number += gj_parse_digit(*s);
            s++;
            result.length++;;
        }
    }
    
    return result;
}

static s32 gj_parse_word(const char* s, char* dst, int dst_size)
{
    s32 result = 0;
    while (result < dst_size && *s != ' ' && *s != '\0')
    {
        *dst++ = *s++;
        result++;
    }
    // NOTE: Add null-termination
    // TODO: Make optional?
    if (result < dst_size) *dst = '\0';
    return result;
}

// Note: Add anywhere so that you can add breakpoint in Visual Studio before going out of scope e.g.
// if (something)
// {
//     int x = some_function();
//     brk; // <----- if this isn't here you have to step through assembly to view result of some_function in x
// }
// TODO: Try using __debugbreak
#if defined(GJ_DEBUG)
#define brk do { int ______ = 0; ______++; } while(gj_False)
#endif

///////////////////////////////////////////////////////////////////////////
// Timers
///////////////////////////////////////////////////////////////////////////
#if defined(GJ_DEBUG) && defined(_MSC_VER) && defined(GJ_TIMERS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _NO_CRT_STDIO_INLINE
#include <windows.h>

typedef struct DebugTimer
{
    b32 active;
    char name[BUFFER_SIZE];
    u64 current;
    f32 total;
    f32 total_this_frame;
    u64 count;
} DebugTimer;

static u64        g_debug_timer_perf_freq;
static u32        g_debug_timer_frame_count = 0;
static DebugTimer g_debug_timers[20] =
{
    {gj_True, "Frame",  0, 0.0f, 0.0f, 0} // Note: Used to measure each frame
};

// Note: Used to measure the timers themselves
static u64        g_debug_timers_overhead_start;
static u64        g_debug_timers_overhead = 0;
#define StartOverheadTimer()                                            \
    do {                                                                \
        LARGE_INTEGER perf_counter_start; QueryPerformanceCounter(&perf_counter_start); \
        g_debug_timers_overhead_start = perf_counter_start.QuadPart;    \
    } while (gj_False)
#define EndOverheadTimer()                                              \
    do {                                                                \
        LARGE_INTEGER perf_counter_end; QueryPerformanceCounter(&perf_counter_end); \
        g_debug_timers_overhead += (perf_counter_end.QuadPart - g_debug_timers_overhead_start) / g_debug_timer_perf_freq; \
    } while (gj_False)

static DebugTimer*
gj_get_timer(const char* name)
{
    StartOverheadTimer();
    
    DebugTimer* result = NULL;
    u32 debug_timer_index = 0; 
    while (debug_timer_index < gj_ArrayCount(g_debug_timers) &&
           !gj_strings_equal_null_term(g_debug_timers[debug_timer_index].name, name)) { debug_timer_index++; }
    if (debug_timer_index < gj_ArrayCount(g_debug_timers))
    {
        result = &g_debug_timers[debug_timer_index];
    }

    EndOverheadTimer();
    
    return result;
}

static void
gj_start_timer(const char* name)
{
    StartOverheadTimer();
    
    DebugTimer* debug_timer = gj_get_timer(name);
    if (!debug_timer)
    {
        u32 debug_timer_index = 0;
        while (debug_timer_index < gj_ArrayCount(g_debug_timers) &&
               g_debug_timers[debug_timer_index].active) { debug_timer_index++; }
        gj_Assert(debug_timer_index < gj_ArrayCount(g_debug_timers));
        
        gj_ZeroMemory(&g_debug_timers[debug_timer_index]);
        g_debug_timers[debug_timer_index].active = gj_True;
        gj_string_copy(g_debug_timers[debug_timer_index].name, name);
        debug_timer = &g_debug_timers[debug_timer_index];
    }
    gj_Assert(debug_timer->active);
    
    LARGE_INTEGER perf_counter_start; QueryPerformanceCounter(&perf_counter_start);
    debug_timer->current = perf_counter_start.QuadPart;
    debug_timer->count++;

    EndOverheadTimer();
}

static void
gj_end_timer(const char* name)
{
    StartOverheadTimer();
    
    DebugTimer* debug_timer = gj_get_timer(name);
    gj_Assert(debug_timer);

    {
        LARGE_INTEGER perf_counter_end; QueryPerformanceCounter(&perf_counter_end);
        f32 current_time = (f32)(perf_counter_end.QuadPart - debug_timer->current) / (f32)g_debug_timer_perf_freq;
        debug_timer->total += current_time;
        debug_timer->total_this_frame += current_time;
    }
    
    EndOverheadTimer();
}

static void
gj_timers_start_frame()
{
    LARGE_INTEGER perf_freq; QueryPerformanceFrequency(&perf_freq);
    g_debug_timer_perf_freq = perf_freq.QuadPart;

    StartOverheadTimer();
    
    g_debug_timer_frame_count++;
    
    gj_start_timer("Frame");
    for (u32 debug_timer_index = 0;
         debug_timer_index < gj_ArrayCount(g_debug_timers);
         debug_timer_index++)
    {
        DebugTimer* debug_timer = &g_debug_timers[debug_timer_index];
        debug_timer->total_this_frame = 0.0f;
    }

    EndOverheadTimer();
}

static void
gj_timers_end_frame()
{
    gj_end_timer("Frame");
}

#else

typedef struct DebugTimer DebugTimer;
static DebugTimer* gj_get_timer(const char* name) { }
static void gj_start_timer(const char* name) { }
static void gj_end_timer(const char* name) { }
static void gj_timers_start_frame() { }
static void gj_timers_end_frame() { }

#endif

///////////////////////////////////////////////////////////////////////////
// Memory
///////////////////////////////////////////////////////////////////////////
typedef struct MemoryArena
{
    size_t size;
    u8* base;
    size_t used;
} MemoryArena;

#define BeginTemporaryMemoryBlock(arena)size_t tmp_used = (arena)->used
#define EndTemporaryMemoryBlock(arena) (arena)->used = tmp_used

void initialize_arena(MemoryArena* arena, size_t size, u8* base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

void gj_memory_clear_arena(MemoryArena* arena)
{
    memset(arena->base, 0, arena->size);
    arena->used = 0;
}

#define push_size(arena, size)_push(arena, size, 4)
#define push_array(arena, type, count)(type*)push_size(arena, sizeof(type) * count)
#define push_struct(arena, type) (type*)_push(arena, sizeof(type), 4)
void* _push(MemoryArena* arena, size_t size, size_t alignment)
{
    size_t result_pointer = (size_t)arena->base + arena->used;
    
    size_t alignment_offset = 0;
    
    size_t alignment_mask = alignment - 1;
    if (result_pointer & alignment_mask)
    {
        alignment_offset = alignment - result_pointer & alignment_mask;
    }
    size += alignment_offset;
    
    gj_Assert(arena->used + size <= arena->size);
    arena->used += size;

    void* result = (void*)(result_pointer + alignment_offset);

    memset(result, 0, size);
    
    return result;
}

///////////////////////////////////////////////////////////////////////////
// OS API
///////////////////////////////////////////////////////////////////////////
typedef enum PlatformOpenFileModeFlags
{
    PlatformOpenFileModeFlags_Read  = 0x1,
    PlatformOpenFileModeFlags_Write = 0x2,
} PlatformOpenFileModeFlags;

typedef struct PlatformFileHandle
{
    void* handle;
    u32 file_size;
    // TODO: Remove?
    char* full_file_name;
} PlatformFileHandle;

typedef struct PlatformFileListing
{
    char* file_name;
    struct PlatformFileListing* next;
} PlatformFileListing;

typedef struct PlatformThreadContext
{
    void (*thread_func)(void*);
    void* param;
    void* platform;
} PlatformThreadContext;

typedef enum ThreadStatus
{
    ThreadStatus_Done    = 0,
    ThreadStatus_Running
} ThreadStatus;

typedef struct TicketMutex
{
    u64 volatile ticket;
    u64 volatile serving;
} TicketMutex;

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
typedef struct SoundBuffer
{
    int16_t* buffer;
    size_t buffer_size;
    uint32_t count;
    void* platform;
} SoundBuffer;

typedef u32         GetMaxQueuedSoundBuffers();
typedef u32         QueuedSoundBuffers();
typedef SoundBuffer CreatesoundBuffer(uint32_t sample_count);
typedef void        SubmitSoundBuffer(SoundBuffer sound_buffer);
typedef u32         GetRemainingSamples(SoundBuffer sound_buffer);

///////////////////////////////////////////////////////////////////////////
// PlatformAPI
///////////////////////////////////////////////////////////////////////////
typedef struct PlatformAPI
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
} PlatformAPI;

static PlatformAPI g_platform_api;

#endif
