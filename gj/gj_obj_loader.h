#if !defined(GJ_OBJ_LOADER_H)
#define GJ_OBJ_LOADER_H

// GJ_ObjLoader_Mesh gj_obj_loader_load
// Allocates
//  1024 * sizeof(V3f) + 1024 * sizeof(V3f) + 1024 * 1024 * sizeof(s32)
// Make sure to call gj_obj_loader_unload when done.

#include <gj/gj_base.h>
#include <gj/gj_math.h>

#include <stdlib.h> // strtof

#define Pos             "v"
#define PosStrLength    1
#define Normal          "vn"
#define NormalStrLength 2
#define UV              "vt"
#define UVStrLength     2
#define Face            "f"
#define FaceStrLength   1

struct GJ_ObjLoader_ParseState
{
    u8* buffer;
    u32 buffer_index;
    u32 buffer_contents_size;
};

internal void
gj_obj_loader_skip_whitespace(GJ_ObjLoader_ParseState* parse_state)
{
    while (gj_IsWhitespace(parse_state->buffer[parse_state->buffer_index]))
    {
        parse_state->buffer_index++;
    }
}

internal void
gj_obj_loader_skip_char(GJ_ObjLoader_ParseState* parse_state, char c)
{
    if (parse_state->buffer[parse_state->buffer_index] == c) parse_state->buffer_index++;
}

internal bool
gj_obj_loader_check_current_word(GJ_ObjLoader_ParseState* parse_state, const char* str)
{
    bool result = true;
    gj_obj_loader_skip_whitespace(parse_state);
    for (u32 char_index = parse_state->buffer_index;
         char_index < parse_state->buffer_contents_size;
         char_index++)
    {
        char str_char    = str[char_index - parse_state->buffer_index];
        char buffer_char = parse_state->buffer[char_index];
        if (gj_IsWhitespace(buffer_char))
        {
            break;
        }
        else if (str_char != buffer_char)
        {
            result = false;
            break;
        }
    }
    return result;
}

internal f32
gj_obj_loader_parse_f32(GJ_ObjLoader_ParseState* parse_state)
{
    f32 result;
    char* end;
    result = strtof((char*)&parse_state->buffer[parse_state->buffer_index], &end);
    parse_state->buffer_index += (u32)(end - (char*)&parse_state->buffer[parse_state->buffer_index]);
    return result;
}

internal s32
gj_obj_loader_parse_s32(GJ_ObjLoader_ParseState* parse_state)
{
    s32 result;
    char* end;
    result = strtol((char*)&parse_state->buffer[parse_state->buffer_index], &end, 10);
    parse_state->buffer_index += (u32)(end - (char*)&parse_state->buffer[parse_state->buffer_index]);
    return result;
}

internal void
gj_obj_loader_load(const char* obj_filename,
                   V3f* out_positions, V3f* out_normals, V2f* out_uvs, size_t stride, const u32 vertices_max_count, u32* vertex_count,
                   s32* out_indices, const u32 indices_max_count, u32* index_count)
{
    GJ_ObjLoader_ParseState parse_state;
    gj__ZeroStruct(parse_state);

    parse_state.buffer = (u8*)g_platform_api.allocate_memory(1024 * 1024 * 1024);    
    PlatformFileHandle obj_file_handle = g_platform_api.get_file_handle(obj_filename, PlatformOpenFileModeFlags_Read);
    g_platform_api.read_data_from_file_handle(obj_file_handle, 0, obj_file_handle.file_size, parse_state.buffer);
    parse_state.buffer_contents_size = obj_file_handle.file_size;

    *vertex_count = 0;
    *index_count  = 0;
    
    u32  position_count  = 0;
    V3f* positions       = out_positions;
    u32  normal_count    = 0;
    V3f* normals         = (V3f*)g_platform_api.allocate_memory(sizeof(V3f) * vertices_max_count);
    V3f* normals_pointer = normals;
    u32  uv_count        = 0;
    V2f* uvs             = (V2f*)g_platform_api.allocate_memory(sizeof(V2f) * vertices_max_count);
    V2f* uvs_pointer     = uvs;
    s32* indices         = out_indices;
    while (parse_state.buffer_index < parse_state.buffer_contents_size)
    {
        if (gj_obj_loader_check_current_word(&parse_state, Pos))
        {
            parse_state.buffer_index += PosStrLength;
            positions->x = gj_obj_loader_parse_f32(&parse_state);
            positions->y = gj_obj_loader_parse_f32(&parse_state);
            positions->z = gj_obj_loader_parse_f32(&parse_state);
            positions = (V3f*)(((byte*)positions) + stride);
            position_count++; gj_Assert(position_count <= vertices_max_count);
            *vertex_count += 1;
        }
        else if (gj_obj_loader_check_current_word(&parse_state, Normal))
        {
            parse_state.buffer_index += NormalStrLength;
            normals_pointer->x = gj_obj_loader_parse_f32(&parse_state);
            normals_pointer->y = gj_obj_loader_parse_f32(&parse_state);
            normals_pointer->z = gj_obj_loader_parse_f32(&parse_state);
            normals_pointer++;
            normal_count++; gj_Assert(normal_count <= vertices_max_count);
        }
        else if (gj_obj_loader_check_current_word(&parse_state, UV))
        {
            parse_state.buffer_index += UVStrLength;
            uvs_pointer->x = gj_obj_loader_parse_f32(&parse_state);
            uvs_pointer->y = gj_obj_loader_parse_f32(&parse_state);
            uvs_pointer++;
            uv_count++; gj_Assert(uv_count <= vertices_max_count);
        }
        else if (gj_obj_loader_check_current_word(&parse_state, Face))
        {
            parse_state.buffer_index += FaceStrLength;
#define AssignIndicesAndUVs(P, V)                                       \
            s32 P = gj_obj_loader_parse_s32(&parse_state);              \
            gj_obj_loader_skip_char(&parse_state, '/');                 \
            s32 uv##V = gj_obj_loader_parse_s32(&parse_state);          \
            gj_obj_loader_skip_char(&parse_state, '/');                 \
            s32 n##V = gj_obj_loader_parse_s32(&parse_state);           \
            memcpy(((byte*)out_normals) + (P - 1) * stride, &normals[(n##V - 1)], sizeof(V3f)); \
            memcpy(((byte*)out_uvs)     + (P - 1) * stride, &uvs[(uv##V - 1)],    sizeof(V2f));
            
            AssignIndicesAndUVs(p1, 1);
            AssignIndicesAndUVs(p2, 2);
            AssignIndicesAndUVs(p3, 3);
            AssignIndicesAndUVs(p4, 4);

#define AddIndex(P) indices[*index_count] = (P - 1); *index_count += 1; gj_Assert(*index_count <= indices_max_count);
            AddIndex(p1);
            AddIndex(p2);
            AddIndex(p3);
            AddIndex(p1);
            AddIndex(p3);
            AddIndex(p4);
            
#undef AddIndex
#undef AssignIndicesAndUVs
        }
        else
        {
            while (parse_state.buffer[parse_state.buffer_index++] != '\n')
            {
            }
        }
    }

    g_platform_api.deallocate_memory(normals);
    g_platform_api.deallocate_memory(uvs);
    g_platform_api.close_file_handle(obj_file_handle);
    g_platform_api.deallocate_memory(parse_state.buffer);
}

#undef Pos
#undef PosStrLength
#undef Normal
#undef NormalStrLength
#undef UV
#undef UVStrLength
#undef Face
#undef FaceStrLength

#endif
