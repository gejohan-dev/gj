#if !defined(GJ_MATH_H)
#define GJ_MATH_H

#define PI_F32 3.141592f
#define E_F32  2.7182818284f
#define DEG_TO_RAD (PI_F32 / 180.0f)
#define RAD_TO_DEG (1.0f / DEG_TO_RAD)

// TODO: Replace some stuff from here?
#include "math.h"

#define gj_Max(a, b)  (((a) > (b)) ? (a) : (b))
#define gj_Min(a, b)  (((a) < (b)) ? (a) : (b))
#define gj_Abs(x) (((x)<0) ? -(x) : (x))

inline f32 sqrt(f32 x)        { return sqrtf(x); }
inline u32 pow2(u32 x)        { return (u32)powf(2,(f32)x); }
inline f32 pow2(f32 x)        { return powf(2,x); }
inline f32 pow (f32 x, f32 n) { return powf(x,n); }

// Trigonometric functions
inline f32 sin(f32 x) { return sinf(x); }
inline f32 cos(f32 x) { return cosf(x); }
inline f32 tan(f32 x) { return tanf(x); }

inline f32 clamp(f32 min, f32 value, f32 max) { return value < min ? min : (value > max ? max : value); }
inline f32 lerp(f32 a, f32 b, f32 x) { return (1.0f - x) * a + x * b; }
inline f32 normalize(f32 a, f32 b, f32 x) { return gj_Min(1.0f, gj_Max(0.0f, (x - a) / (b - a))); }

// Vectors
#define V2(name, type)                          \
    union V2##name {                            \
        struct { type x;     type y; };         \
        struct { type width; type height; };    \
        type array[2];                          \
    }
V2(f, f32);
V2(u, u32);

#define V3(name, type)                          \
    union V3##name {                            \
        struct { type x; type y; type z; };     \
        struct { type r; type g; type b; };     \
        type array[3];                          \
    }
V3(f, f32);

#define V4(name, type)                                  \
    union V4##name {                                    \
        struct { type x;     type y;      type z; type w; };    \
        struct { type r;     type g;      type b; type a; };    \
    }
V4(f, f32);

inline V2f V2_add              (V2f v0, V2f v1)   { return {v0.x + v1.x, v0.y + v1.y}; }
inline V2f V2_sub              (V2f v0, V2f v1)   { return {v0.x - v1.x, v0.y - v1.y}; }
inline V2f V2_mul              (f32 c, V2f v)     { return {c * v.x, c * v.y}; }
inline V2f V2_div              (f32 x, V2f v)     { return {x / v.x, x / v.y}; }
inline f32 V2_length           (V2f v0)           { return sqrt(v0.x * v0.x + v0.y * v0.y); }
inline f32 V2_length           (f32 x, f32 y)     { return sqrt(x * x + y * y); }
inline V2f V2_normalize        (V2f v0)           { f32 l = V2_length(v0); return {v0.x / l, v0.y / l}; }
inline f32 V2_dot              (V2f v0, V2f v1)   { return v0.x * v1.x + v0.y * v1.y; }
inline V3f V2_to_V3            (V2f v, f32 z)     { return {v.x, v.y, z}; }
inline V2f V2_reflect          (V2f v, V2f n)     { return V2_sub(v, V2_mul(2.0f, V2_mul(V2_dot(v, n), n))); }
inline f32 V2_get_angle        (V2f v, V2f u)     { return acosf(V2_dot(v, u) / (V2_length(v) * V2_length(u))); }
inline f32 V2_get_angle_x_axis (V2f v)            { return V2_get_angle(v, {1.0f, 0.0f}); }
inline V2f V2_rotate           (V2f v, f32 a)     { return {V2_dot(v, {cos(a), -sin(a)}), V2_dot(v, {sin(a), cos(a)})}; }
inline V2f V2_rotate_90        (V2f v)            { return {-v.y, v.x}; }

inline V3f V3_add      (V3f v0, V3f v1)             { return {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z}; }
inline V3f V3_add      (V3f v0, f32 c)              { return {v0.x + c, v0.y + c, v0.z + c}; }
inline V3f V3_add      (f32 c, V3f v0)              { return V3_add(v0, c); }
inline V3f V3_sub      (V3f v0, V3f v1)             { return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z}; }
inline f32 V3_length   (V3f v0)                     { return sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z); }
inline f32 V3_length   (f32 x, f32 y, f32 z)        { return sqrt(x * x + y * y + z * z); }
inline V3f V3_mul      (f32 x1, f32 y1, f32 z1,
                        f32 x2, f32 y2, f32 z2)     { return {x1 * x2, y1 * y2, z1 * z2}; }
inline V3f V3_mul      (f32 c, V3f v0)              { return {c * v0.x, c * v0.y, c * v0.z}; }
inline V3f V3_mul      (V3f v0, f32 c)              { return V3_mul(c, v0); }
inline V3f V3_mul      (f32 x, f32 y, f32 z, f32 c) { return {c * x, c * y, c * z}; }
inline V3f V3_div      (f32 x, V3f v)               { return {x / v.x, x / v.y, x / v.z}; }
inline V3f V3_normalize(V3f v0)                     { f32 l = V3_length(v0); return {v0.x / l, v0.y / l, v0.z / l}; }
inline f32 V3_dot      (V3f v0, V3f v1)             { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z; }
inline V3f V3_cross    (V3f v0, V3f v1)             { return {v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x}; }

V3f V3_triangle_normal(V3f p1, V3f p2, V3f p3)
{
    V3f result = {};    
    V3f normal_comp_1 = V3_sub(p2, p1);
    V3f normal_comp_2 = V3_sub(p3, p1);
    result = V3_cross(normal_comp_1, normal_comp_2);
    result = V3_normalize(result);
    return result;
}

// Matrices
union M4x4
{
    // 0: 0 1 2 3
    // 1: 0 1 2 3
    // 2: 0 1 2 3
    // 3: 0 1 2 3
    f32 m[4][4];

    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    f32 a[16];
};

inline M4x4 M4x4_identity() { return {1.0f,0,0,0, 0,1.0f,0,0, 0,0,1.0f,0, 0,0,0,1.0f}; }

inline M4x4 M4x4_mul(M4x4 m, M4x4 n)
{ return {m.a[0] *n.a[0] + m.a[1] *n.a[4] + m.a[2] *n.a[8] + m.a[3] *n.a[12],  m.a[0] *n.a[1] + m.a[1] *n.a[5] + m.a[2] *n.a[9] + m.a[3] *n.a[13],  m.a[0] *n.a[2] + m.a[1] *n.a[6] + m.a[2] *n.a[10] + m.a[3] *n.a[14],  m.a[0] *n.a[3] + m.a[1] *n.a[7] + m.a[2] *n.a[11] + m.a[3] *n.a[15],
          m.a[4] *n.a[0] + m.a[5] *n.a[4] + m.a[6] *n.a[8] + m.a[7] *n.a[12],  m.a[4] *n.a[1] + m.a[5] *n.a[5] + m.a[6] *n.a[9] + m.a[7] *n.a[13],  m.a[4] *n.a[2] + m.a[5] *n.a[6] + m.a[6] *n.a[10] + m.a[7] *n.a[14],  m.a[4] *n.a[3] + m.a[5] *n.a[7] + m.a[6] *n.a[11] + m.a[7] *n.a[15],
          m.a[8] *n.a[0] + m.a[9] *n.a[4] + m.a[10]*n.a[8] + m.a[11]*n.a[12],  m.a[8] *n.a[1] + m.a[9] *n.a[5] + m.a[10]*n.a[9] + m.a[11]*n.a[13],  m.a[8] *n.a[2] + m.a[9] *n.a[6] + m.a[10]*n.a[10] + m.a[11]*n.a[14],  m.a[8] *n.a[3] + m.a[9] *n.a[7] + m.a[10]*n.a[11] + m.a[11]*n.a[15],
          m.a[12]*n.a[0] + m.a[13]*n.a[4] + m.a[14]*n.a[8] + m.a[15]*n.a[12],  m.a[12]*n.a[1] + m.a[13]*n.a[5] + m.a[14]*n.a[9] + m.a[15]*n.a[13],  m.a[12]*n.a[2] + m.a[13]*n.a[6] + m.a[14]*n.a[10] + m.a[15]*n.a[14],  m.a[12]*n.a[3] + m.a[13]*n.a[7] + m.a[14]*n.a[11] + m.a[15]*n.a[15]}; }

inline V4f M4x4_mul(M4x4 m, V4f v)
{
    V4f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

inline V3f M4x4_mul(M4x4 m, V3f v)
{
    V3f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];
    return result;
}

inline void M4x4_apply_translate(M4x4& m, f32 x, f32 y, f32 z)
{
    m.a[3]  += m.a[0]*x  + m.a[1]*y  + m.a[2]*z;
    m.a[7]  += m.a[4]*x  + m.a[5]*y  + m.a[6]*z;
    m.a[11] += m.a[8]*x  + m.a[9]*y  + m.a[10]*z;
    m.a[15] += m.a[12]*x + m.a[13]*y + m.a[14]*z;    
}
inline void M4x4_apply_translate(M4x4& m, V3f v) { M4x4_apply_translate(m, v.x, v.y, v.z); }

inline void
M4x4_apply_transpose(M4x4& m)
{
    gj_SwapArray(m.a, f32, 1,  4);
    gj_SwapArray(m.a, f32, 2,  8);
    gj_SwapArray(m.a, f32, 3,  12);
    gj_SwapArray(m.a, f32, 6,  9);
    gj_SwapArray(m.a, f32, 7,  13);
    gj_SwapArray(m.a, f32, 11, 14);
}

inline void M4x4_apply_rotate_x(M4x4& m, f32 angle_degrees)
{
    f32 cos_value = cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = sin(angle_degrees * DEG_TO_RAD);
    m.a[4]  = m.a[4] * cos_value - m.a[8]  * sin_value;
    m.a[5]  = m.a[5] * cos_value - m.a[9]  * sin_value;
    m.a[6]  = m.a[6] * cos_value - m.a[10] * sin_value;
    m.a[7]  = m.a[7] * cos_value - m.a[11] * sin_value;
    m.a[8]  = m.a[4] * sin_value + m.a[8]  * cos_value;
    m.a[9]  = m.a[5] * sin_value + m.a[9]  * cos_value;
    m.a[10] = m.a[6] * sin_value + m.a[10] * cos_value;
    m.a[11] = m.a[7] * sin_value + m.a[11] * cos_value;    
}

inline void M4x4_apply_rotate_y(M4x4& m, f32 angle_degrees)
{
    f32 cos_value = cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = sin(angle_degrees * DEG_TO_RAD);
    m.a[0]  =  m.a[0] * cos_value + m.a[8]  * sin_value;
    m.a[1]  =  m.a[1] * cos_value + m.a[9]  * sin_value;
    m.a[2]  =  m.a[2] * cos_value + m.a[10] * sin_value;
    m.a[3]  =  m.a[3] * cos_value + m.a[11] * sin_value;
    m.a[8]  = -m.a[0] * sin_value + m.a[8]  * cos_value;
    m.a[9]  = -m.a[1] * sin_value + m.a[9]  * cos_value;
    m.a[10] = -m.a[2] * sin_value + m.a[10] * cos_value;
    m.a[11] = -m.a[3] * sin_value + m.a[11] * cos_value;    
}

inline void M4x4_apply_rotate_z(M4x4& m, f32 angle_degrees)
{
    f32 cos_value = cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = sin(angle_degrees * DEG_TO_RAD);
    m.a[0] = m.a[0] * cos_value - m.a[4] * sin_value;
    m.a[1] = m.a[1] * cos_value - m.a[5] * sin_value;
    m.a[2] = m.a[2] * cos_value - m.a[6] * sin_value;
    m.a[3] = m.a[3] * cos_value - m.a[7] * sin_value;
    m.a[4] = m.a[0] * sin_value + m.a[4] * cos_value;
    m.a[5] = m.a[1] * sin_value + m.a[5] * cos_value;
    m.a[6] = m.a[2] * sin_value + m.a[6] * cos_value;
    m.a[7] = m.a[3] * sin_value + m.a[7] * cos_value;    
}

// 3D stuff
M4x4 M4x4_model_view_matrix(V3f camera_pos, V3f camera_direction, V3f up, b32 translate = true)
{
    V3f camera_right = V3_normalize(V3_cross(up, camera_direction));
    V3f camera_up = V3_cross(camera_direction, camera_right);
    M4x4 result = {
        camera_right.x,     camera_right.y,     camera_right.z,     0.0f,
        camera_up.x,        camera_up.y,        camera_up.z,        0.0f,
        camera_direction.x, camera_direction.y, camera_direction.z, 0.0f,
        0.0f,               0.0f,               0.0f,               1.0f
    };
    if (translate)
    {
        M4x4_apply_translate(result, -camera_pos.x, -camera_pos.y, -camera_pos.z);
    }
    return result;
}

M4x4 M4x4_inverse_model_view_matrix(M4x4 model_view_matrix, V3f camera_pos)
{
    M4x4 result = model_view_matrix;
    M4x4_apply_translate(result, camera_pos.x, camera_pos.y, camera_pos.z);
    M4x4_apply_transpose(result);
    return result;
}

M4x4 M4x4_fixed_forward_matrix(V3f camera_pos)
{
    M4x4 result = M4x4_identity();
    M4x4_apply_translate(result, -camera_pos.x, -camera_pos.y, camera_pos.z);
    return result;
}

M4x4 M4x4_inverse_fixed_forward_matrix(M4x4 fixed_forward_matrix, V3f camera_pos)
{
    M4x4 result = M4x4_identity();
    M4x4_apply_translate(result, camera_pos.x, camera_pos.y, -camera_pos.z);
    return result;
}

M4x4 M4x4_projection_matrix(f32 field_of_view_degrees, f32 aspect_w_over_h,
                            f32 near_plane, f32 far_plane)
{
    M4x4 result = M4x4_identity();
    f32 t = tan(field_of_view_degrees * DEG_TO_RAD / 2.0f);
    f32 height = near_plane * t;
    f32 width = height * aspect_w_over_h;
    result.a[0]  = near_plane / width;
    result.a[5]  = near_plane / height;
    result.a[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    result.a[11] = -(2 * far_plane * near_plane) / (far_plane - near_plane);
    result.a[14] = -1;
    result.a[15] = 0;
    return result;
}

M4x4 M4x4_inverse_projection_matrix(M4x4 projection_matrix)
{
    M4x4 result = {};
    result.a[0]  = 1.0f / projection_matrix.a[0];
    result.a[5]  = 1.0f / projection_matrix.a[5];
    result.a[10] = 0.0f;
    result.a[11] = -1.0f;
    f32 inv_d = 1.0f / projection_matrix.a[11];
    result.a[14] = inv_d;
    result.a[15] = inv_d * projection_matrix.a[10];
    return result;
}

M4x4 M4x4_orthographic_matrix(f32 width, f32 height,
                              f32 near_plane, f32 far_plane)
{
    M4x4 result = M4x4_identity();
    result.a[0] = 2.0f / width;
    result.a[5] = 2.0f / height;
    result.a[10] = -2.0f / (far_plane - near_plane);
    result.a[3]  = -1.0f;
    result.a[7]  = -1.0f;
    result.a[11] = -1.0f;
    return result;
}

M4x4 M4x4_inverse_orthographic_matrix(M4x4 orthographic_matrix)
{
    M4x4 result = M4x4_identity();
    return result;
}

inline V3f
V3f_fps_forward_vector(f32 yaw_radians, f32 pitch_radians)
{
    return {
        cos(yaw_radians) * cos(pitch_radians),
        sin(pitch_radians),
        sin(yaw_radians) * cos(pitch_radians)
    };
}

inline b32
rectangle_bounds_check(f32 px, f32 py, f32 rx1, f32 rx2, f32 ry1, f32 ry2) { return px >= rx1 && px < rx2 && py >= ry1 && py < ry2; }

f32 ray_plane_collision(V3f v0, V3f v1, V3f v2,
                    V3f ray_origin, V3f ray_direction)
{
    f32 result = 0.0f;
    V3f normal = V3_triangle_normal(v0, v1, v2);
    result = (V3_dot(V3_sub(v0, ray_origin), normal) /
              V3_dot(ray_direction, normal));
    return result;
}

// https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
b32 ray_triangle_intersection(V3f v0, V3f v1, V3f v2,
                              V3f ray_origin, V3f ray_direction,
                              V3f* pos)
{
    f32 epsilon = 0.000001f;
    V3f edge1 = V3_sub(v1, v0);
    V3f edge2 = V3_sub(v2, v0);
    V3f pvec = V3_cross(ray_direction, edge2);
    f32 det = V3_dot(edge1, pvec);
    if (det > -epsilon && det < epsilon) return false;
    f32 inv_det = 1.0f / det;
    
    V3f tvec = V3_sub(ray_origin, v0);
    f32 u = V3_dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) return false;

    V3f qvec = V3_cross(tvec, edge1); 
    f32 v = V3_dot(ray_direction, qvec) * inv_det; 
    if (v < 0.0f || u + v > 1.0f) return false; 
 
    f32 t = V3_dot(edge2, qvec) * inv_det;
    *pos = V3_add(*pos, V3_mul(t, ray_direction));
        
    return true;
}

b32 ray_rectangle_intersection(V3f min_min_pos, V3f max_max_pos,
                               V3f ray_origin, V3f ray_direction,
                               V3f* pos)
{
    b32 result;
    V3f max_min_pos = min_min_pos;
    max_min_pos.x = max_max_pos.x;
    V3f min_max_pos = min_min_pos;
    min_max_pos.y = max_max_pos.y;
    result = (ray_triangle_intersection(min_min_pos, min_max_pos, max_max_pos, ray_origin, ray_direction, pos) |
              ray_triangle_intersection(min_min_pos, max_max_pos, max_min_pos, ray_origin, ray_direction, pos));
    return result;
}

#endif
