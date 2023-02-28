#if !defined(GJ_BASE_H)
#define GJ_BASE_H
//
// This file contains some common utilities/basic things I use in different
// projects.
//

#include <cstring> // memset

#define STB_SPRINTF_IMPLEMENTATION
#include <libs/stb/stb_sprintf.h> // string format
int gj_sprintf(char* buffer, char* format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = stbsp_vsprintf(buffer, format, args);
    va_end(args);
    return result;
}

///////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <limits.h>

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

typedef u8 byte;
typedef u8 one_byte;

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
#define gj_Assert(Exp) Exp
#endif

#else // !defined(Assert)
#define gj_Assert(Exp) Assert(Exp)
#endif

#if defined(GJ_DEBUG)
#define gj_OnlyDebug(Exp) Exp
#else
#define gj_OnlyDebug(Exp)
#endif

#define gj_AssertDebug(Exp) gj_OnlyDebug(gj_Assert(Exp))

#define BUFFER_SIZE 512

#define InvalidCodePath gj_OnlyDebug(gj_Assert(!"InvalidCodePath"))
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define gj_BitmaskU32 (0xFFFFFFFF)

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)

#define gj_BytesToMegabytes(Bytes) ((double)(Bytes) / (double)(1024*1024))

typedef void _dummy_f();
#define gj_FunctionPointerSize (sizeof(_dummy_f*))

#define gj_ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

inline u32
gj_safe_cast_u64_to_u32(u64 value)
{ gj_AssertDebug(value <= gj_BitmaskU32); return (u32)(value & gj_BitmaskU32); }

inline u32
gj_safe_cast_s32_to_u32(s32 value)
{ gj_AssertDebug(value >= 0); return (u32)value; }

inline u32*
gj_safe_cast_s32_to_u32(s32* value)
{ gj_AssertDebug(value >= 0); return (u32*)value; }

#define gj_IsCharacter(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define gj_IsDigit(c) (c >= '0' && c <= '9')
#define gj_IsWhitespace(c) (c == ' ' || c == '\n' || c == '\r' || c == '\t')

#define gj_DigitToChar(d) ((d) + '0')
#define gj_CharToDigit(c) ((c) - '0')

#define FLT_MAX 3.402823466e+38F /* max value */
#define FLT_MIN 1.175494351e-38F /* min positive value */

#define gj_SwapVar(type, x, y) do {##type __tmp = (x); (x) = (y); (y) = __tmp;} while(gj_False)
#define gj_SwapArray(array, type, i, j) do {##type __tmp = array[i]; array[i] = array[j]; array[j] = __tmp;} while(gj_False)

#define gj_ZeroMem(Mem, Size)    do { memset(Mem, 0, Size); } while(gj_False)
#define gj__ZeroStruct(Struct)       do { memset(&Struct, 0, sizeof(Struct)); } while(gj_False)
#define gj_ZeroArray(Array, Length) do { memset(Array,  0, sizeof(*(Array)) * Length); } while(gj_False)

#if defined(GJ_DEBUG)
#define gj_UnusedParam(Type, Param) Type
#else
#define gj_UnusedParam(Type, Param) Type Param
#endif

#define gj_MemberSize(Type, Member) (sizeof(((Type*)0)->Member))

inline void gj_set_flag   (u32* flags, u32 flag) { *flags |= (1 << flag); }
inline void gj_unset_flag (u32* flags, u32 flag) { *flags &= ~(gj_BitmaskU32 & (1 << flag)); }
inline b32  gj_get_flag   (u32 flags,  u32 flag) { return flags & (1 << flag); }
inline void gj_toggle_flag(u32* flags, u32 flag) { *flags ^= (1 << flag); }

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

#pragma warning(suppress: 4505)
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

#pragma warning(suppress: 4505)
static void build_binary_string(u32 number, char* buffer, u32 padding)
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

#pragma warning(suppress: 4505)
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

#pragma warning(suppress: 4505)
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
//     brk; // <----- if this line wasn't here, you would have to
//                    step through assembly to view result of some_function in x
// }
// TODO: Try using __debugbreak
#if defined(GJ_DEBUG)
#define brk do { int ______ = 0; ______++; } while(gj_False)
#endif // #if defined(GJ_DEBUG)

// Note: Put this function call on a line that you wish to keep, regardless of optimization level
// ...some code that will be optimized...
// gj_UnoptimizedLine(); // <--- This line will compile to something like: call gj_UnoptimizedLine
// ...some more code that will be optimized
// TODO: Add pragmas for Clang/GCC/Others...
#if defined(GJ_DEBUG) && defined(_MSC_VER)

#if defined(_MSC_VER)
#pragma optimize("", off)
#endif // #if defined(_MSC_VER)

#pragma warning(suppress: 4505)
static void gj_UnoptimizedLine() { }

#if defined(_MSC_VER)
#pragma optimize("", on)
#endif // #if defined(_MSC_VER)

#endif // #if defined(GJ_DEBUG) && defined(_MSC_VER)

///////////////////////////////////////////////////////////////////////////
// Array
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
#define __InArray(Type)                         \
    b32 result = 0;                             \
    for (Type i = 0; i < array_size; i++)       \
    {                                           \
        if (array[i] == element)                \
        {                                       \
            result = 1;                         \
            break;                              \
        }                                       \
    }                                           \
    return result;

inline b32
gj_InArray(u32 element, u32* array, u32 array_size) { __InArray(u32); }
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

MemoryArena create_memory_arena(size_t size, u8* base)
{
    MemoryArena result;
    initialize_arena(&result, size, base);
    return result;
}

void clear_arena(MemoryArena* arena)
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
    
    gj_AssertDebug(arena->used + size <= arena->size);
    arena->used += size;

    void* result = (void*)(result_pointer + alignment_offset);

    memset(result, 0, size);
    
    return result;
}

///////////////////////////////////////////////////////////////////////////
// Array
///////////////////////////////////////////////////////////////////////////
#define gj_DefineArray(Type, Name)                                      \
    struct Name                                                         \
    {                                                                   \
        Type##* data;                                                   \
        u32   count;                                                    \
        u32   max_count;                                                \
        Type##& operator[](int i) { gj_AssertDebug((u32)i < count); return data[i]; } \
                                                                        \
        Type remove(u32 i)                                              \
        {                                                               \
            Type result = (*this)[i];                                   \
            gj_Assert(i < max_count && count > 0);                      \
            for (u32 j = i; j < count; j++)                             \
            {                                                           \
                gj_SwapArray(data, Type, j, j + 1);                     \
            }                                                           \
            count--;                                                    \
            return result;                                              \
        }                                                               \
                                                                        \
        Type##* add_new()                                               \
        {                                                               \
            gj_AssertDebug(count < max_count);                          \
            return &data[count++];                                      \
        }                                                               \
                                                                        \
        void add(Type element)                                          \
        {                                                               \
            gj_AssertDebug(count < max_count);                          \
            data[count++] = element;                                    \
        }                                                               \
                                                                        \
        Type pop()                                                      \
        {                                                               \
            return remove(count - 1);                                   \
        }                                                               \
                                                                        \
        void clear()                                                    \
        {                                                               \
            count = 0;                                                  \
        }                                                               \
                                                                        \
        void fill(Type value)                                           \
        {                                                               \
            for (u32 i = 0; i < max_count; i++) { data[i] = value; }    \
            count = max_count;                                          \
        }                                                               \
    };                                                                  \
                                                                        \
    void Name##_init(Name##* array, MemoryArena* memory_arena, u32 max_count) \
    {                                                                   \
        array->data = (Type##*)push_size(memory_arena, max_count * sizeof(Type)); \
        array->count = 0;                                               \
        array->max_count = max_count;                                   \
    }                                                           
    
gj_DefineArray(u32, U32Array);
gj_DefineArray(s32, S32Array);
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
#if defined(GJ_DEBUG)
#define gj_VerifyPlatformAPI(PlatformAPI)                               \
    do {                                                                \
        char test[gj_FunctionPointerSize] = {};                         \
        for (int i = 0; i < sizeof(PlatformAPI._os_api); i += gj_FunctionPointerSize) \
        {                                                               \
            _dummy_f* f = (_dummy_f*)((void*)(PlatformAPI._os_api + i)); \
            gj_Assert(memcmp(f, test, gj_FunctionPointerSize)); \
        }                                                               \
    } while(0)
#else
#define gj_VerifyPlatformAPI(PlatformAPI)
#endif

typedef struct PlatformAPI
{
    //
    // OS API
    //
    union
    {
        struct
        {
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
        };

#if GJ_DEBUG
        u8 _os_api[13 * sizeof(GetFileHandle*)];
#else
        u8 _os_api[12 * sizeof(GetFileHandle*)];
#endif
    };

    //
    // Audio API
    //
    GetMaxQueuedSoundBuffers* get_max_queued_sound_buffers;
    QueuedSoundBuffers*       queued_sound_buffers;
    CreatesoundBuffer*        create_sound_buffer;
    SubmitSoundBuffer*        submit_sound_buffer;
    GetRemainingSamples*      get_remaining_samples;

    //
    // Memory
    //
    void*  memory;
    size_t memory_size;

    //
    // Window dimensions
    //
    b32 should_update_window_size; // TODO: Store prev sizes instead?
    u32 window_width;
    u32 window_height;

#if GJ_DEBUG
    b32 debug_mode_on;
#endif
} PlatformAPI;

#if GJ_USE_GLOBAL_PLATFORM_API
static PlatformAPI g_platform_api;
#endif

#endif
