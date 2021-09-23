#if !defined(GJ_MATH_H)
#define GJ_MATH_H
//
// This file contains common math-stuff I've used in different projects.
// Should compile for both .c and .cpp applications, with less functions
// defined for .c applications since there are some overloaded functions
// for .cpp that are not ported yet.
//

///////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////
#include <gj/gj_base.h>
// TODO: Replace some stuff from here?
#include "math.h"

///////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////
#define PI_F32 3.141592f
#define E_F32  2.7182818284f
#define DEG_TO_RAD (PI_F32 / 180.0f)
#define RAD_TO_DEG (1.0f / DEG_TO_RAD)

///////////////////////////////////////////////////////////////////////////
// Bounds
///////////////////////////////////////////////////////////////////////////
#define gj_Max(a, b)     (((a) > (b)) ? (a) : (b))
#define gj_Min(a, b)     (((a) < (b)) ? (a) : (b))
#define gj_Abs(x)        (((x)<0) ? -(x) : (x))
#define gj_IsPositive(x) ((x) >= 0)

inline b32 gj_float_eq(#define gj_FloatEq(X, Y, Eps) (gj_Abs((X) - (Y)) < Eps)

inline f32 clamp(f32 min, f32 value, f32 max) { return value < min ? min : (value > max ? max : value); }
inline f32 lerp(f32 a, f32 b, f32 x) { return (1.0f - x) * a + x * b; }
inline f32 normalize(f32 a, f32 b, f32 x) { return gj_Min(1.0f, gj_Max(0.0f, (x - a) / (b - a))); }

#if defined(__cplusplus)
inline b32 unknown_bounds_check(f32 x, f32 a, f32 b) { return x >= gj_Min(a, b) && x < gj_Max(a, b); }
inline b32 unknown_bounds_check(f32 x, f32 a, f32 b, f32 delta)
{ return x >= gj_Min(a, b) - delta && x < gj_Max(a, b) + delta; }
#endif
///////////////////////////////////////////////////////////////////////////
// Common Functions
///////////////////////////////////////////////////////////////////////////
inline f32 gj_sqrt(f32 x)        { return sqrtf(x); }
inline f32 gj_pow (f32 x, f32 n) { return powf(x,n); }

///////////////////////////////////////////////////////////////////////////
// Trigonometric Functions
///////////////////////////////////////////////////////////////////////////
inline f32 gj_sin(f32 x) { return sinf(x); }
inline f32 gj_cos(f32 x) { return cosf(x); }
inline f32 gj_tan(f32 x) { return tanf(x); }

///////////////////////////////////////////////////////////////////////////
// Vectors
///////////////////////////////////////////////////////////////////////////
#define V2(name, type)                          \
    typedef union V2##name {                    \
        struct { type x;     type y; };         \
        struct { type width; type height; };    \
        type array[2];                          \
    } V2##name
V2(f, f32);
V2(u, u32);

#define V3(name, type)                          \
    typedef union V3##name {                    \
        struct { type x; type y; type z; };     \
        struct { type r; type g; type b; };     \
        type array[3];                          \
        type xy[2];                             \
        V2##name v2;                            \
    } V3##name
V3(f, f32);

#define V4(name, type)                                          \
    typedef union V4##name {                                    \
        struct { type x;     type y;      type z; type w; };    \
        struct { type r;     type g;      type b; type a; };    \
    } V4##name
V4(f, f32);

inline V2f V2_add              (V2f v0, V2f v1)   { V2f v; v.x = (v0.x + v1.x); v.y = (v0.y + v1.y); return v; }
inline V2f V2_sub              (V2f v0, V2f v1)   { V2f v; v.x = (v0.x - v1.x); v.y = (v0.y - v1.y); return v; }
inline V2f V2_mul              (f32 c, V2f v)     {        v.x = (c * v.x);     v.y = (c * v.y);     return v; }
inline V2f V2_div              (f32 x, V2f v)     {        v.x = (x / v.x);     v.y = (x / v.y);     return v; }
#if defined(__cplusplus)
inline f32 V2_length           (V2f v0)           { return gj_sqrt(v0.x * v0.x + v0.y * v0.y); }
inline f32 V2_length           (f32 x, f32 y)     { return gj_sqrt(x * x + y * y); }
#else
inline f32 V2_length           (V2f v0)           { return gj_sqrt(v0.x * v0.x + v0.y * v0.y); }
#endif
inline V2f V2_normalize        (V2f v)            { f32 l = V2_length(v); v.x = (v.x / l); v.y = (v.y / l); return v; }
inline f32 V2_dot              (V2f v0, V2f v1)   { return v0.x * v1.x + v0.y * v1.y; }
inline f32 V2_det              (V2f v0, V2f v1)   { return v0.x * v1.y + v0.y * v1.x; }
inline V2f V2_reflect          (V2f v, V2f n)     { return V2_sub(v, V2_mul(2.0f, V2_mul(V2_dot(v, n), n))); }
inline f32 V2_get_angle        (V2f v, V2f u)     { return atan2f(V2_det(v, u), V2_dot(v, u)); }
inline f32 V2_get_angle_x_axis (V2f v)            { V2f a; a.x = 1.0f; a.y = 0.0f; return V2_get_angle(v, a); }
inline V2f V2_rotate_90        (V2f v)            { gj_SwapVar(f32, v.x, v.y); v.x = -v.x; return v; }
inline V2f V2_rotate           (V2f v, f32 a)
{
    V2f result;
    result.x = gj_cos(a) * v.x - gj_sin(a) * v.y;
    result.y = gj_sin(a) * v.x + gj_cos(a) * v.y;
    return result;
}

// Transform to other data types
inline V3f V2_to_V3(V2f v, f32 z)                   { V3f v3f; v3f.x = v.x; v3f.y = v.y; v3f.z = z; return v3f; }

#if defined(__cplusplus)
inline V3f V3_add        (V3f v0, V3f v1)             { V3f v; v.x = (v0.x + v1.x); v.y = (v0.y + v1.y); v.z = (v0.z + v1.z); return v; }
inline V3f V3_add        (V3f v0, f32 c)              { V3f v; v.x = (v0.x + c); v.y = (v0.y + c); v.z = (v0.z + c); return v; }
inline V3f V3_add        (f32 c, V3f v0)              { return V3_add(v0, c); }
#else
inline V3f V3_add        (V3f v0, V3f v1)             { V3f v; v.x = (v0.x + v1.x); v.y = (v0.y + v1.y); v.z = (v0.z + v1.z); return v; }
#endif
inline V3f V3_sub        (V3f v0, V3f v1)             { V3f v; v.x = (v0.x - v1.x); v.y = (v0.y - v1.y); v.z = (v0.z - v1.z); return v; }
#if defined(__cplusplus)
inline f32 V3_length     (V3f v0)                     { return gj_sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z); }
inline f32 V3_length     (f32 x, f32 y, f32 z)        { return gj_sqrt(x * x + y * y + z * z); }
#else
inline f32 V3_length     (V3f v0)                     { return gj_sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z); }
#endif
#if defined(__cplusplus)
inline V3f V3_mul        (f32 x1, f32 y1, f32 z1,
                          f32 x2, f32 y2, f32 z2)     { V3f v; v.x = (x1 * x2); v.y = (y1 * y2); v.z = (z1 * z2); return v; }
inline V3f V3_mul        (f32 c, V3f v0)              { V3f v; v.x = (c * v0.x); v.y = (c * v0.y); v.z = (c * v0.z); return v; }
inline V3f V3_mul        (V3f v0, f32 c)              { return V3_mul(c, v0); }
inline V3f V3_mul        (f32 x, f32 y, f32 z, f32 c) { V3f v; v.x = (c * x); v.y = (c * y); v.z = (c * z); return v; }
#else
inline V3f V3_mul        (f32 c, V3f v0)              { V3f v; v.x = (c * v0.x); v.y = (c * v0.y); v.z = (c * v0.z); return v; }
#endif
inline V3f V3_div        (f32 x, V3f v)               { v.x  = (x / v.x); v.y = (x / v.y); v.z = (x / v.z);  return v; }
inline V3f V3_normalize  (V3f v)
{
    f32 l = V3_length(v);
    v.x = (v.x / l); v.y = (v.y / l); v.z = (v.z / l); return v;
}
inline f32 V3_dot        (V3f v0, V3f v1)             { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z; }
inline V3f V3_cross      (V3f v0, V3f v1)             { V3f v; v.x = (v0.y * v1.z - v0.z * v1.y); v.y = (v0.z * v1.x - v0.x * v1.z); v.z = (v0.x * v1.y - v0.y * v1.x); return v; }

V3f V3_triangle_normal(V3f p1, V3f p2, V3f p3)
{
    V3f result;
    gj_ZeroStruct(&result);
    V3f normal_comp_1 = V3_sub(p2, p1);
    V3f normal_comp_2 = V3_sub(p3, p1);
    result = V3_cross(normal_comp_1, normal_comp_2);
    result = V3_normalize(result);
    return result;
}

///////////////////////////////////////////////////////////////////////////
// 2D Collision
///////////////////////////////////////////////////////////////////////////
#define V2_LINE_NO_COLLISION FLT_MAX
// Note: Comes from
// https://web.archive.org/web/20060911055655/http://local.wasp.uwa.edu.au/%7Epbourke/geometry/lineline2d/
internal V2f
V2_line_line_intersection(V2f line1_p1, V2f line1_p2,
                          V2f line2_p1, V2f line2_p2)
{
    V2f result = {V2_LINE_NO_COLLISION, V2_LINE_NO_COLLISION};
    
    f32 x1 = line1_p1.x; f32 y1 = line1_p1.y;
    f32 x2 = line1_p2.x; f32 y2 = line1_p2.y;
    f32 x3 = line2_p1.x; f32 y3 = line2_p1.y;
    f32 x4 = line2_p2.x; f32 y4 = line2_p2.y;

    f32 eps = 0.00001f;
    
    f32 den = (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);
    if (!gj_FloatEq(den, 0.0f, eps))
    {
        f32 ua_num = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
        f32 ua = ua_num / den;
        if (ua > 0.0f && !gj_FloatEq(ua, 0.0f, eps))
        {
            result.x = x1 + ua * (x2 - x1);
            result.y = y1 + ua * (y2 - y1);
        }
    }
    
    return result;
}

internal b32
V2_line_rectangle_intersection(V2f line_p1, V2f line_p2,
                               V2f bottom_left_corner,
                               V2f bottom_right_corner,
                               V2f top_left_corner,
                               V2f top_right_corner,
                               f32* out_t, V2f* out_normal)
{
    b32 result = 0;

#define CheckPoint(P1, P2, Point)                                       \
    {                                                                   \
        f32 t = V2_length(V2_sub(Point, line_p1));                      \
        if (t < running_t &&                                            \
            unknown_bounds_check(Point.x, P1.x, P2.x, 0.0001f) &&       \
            unknown_bounds_check(Point.y, P1.y, P2.y, 0.0001f))         \
        {                                                               \
            result      = 1;                                            \
            running_t   = t;                                            \
            *out_t      = t;                                            \
            *out_normal = V2_rotate_90(V2_normalize(V2_sub(P2, P1)));   \
        }                                                               \
    }

    f32 running_t = FLT_MAX;
    
    V2f left   = V2_line_line_intersection(line_p1, line_p2, bottom_left_corner, top_left_corner);
    CheckPoint(bottom_left_corner, top_left_corner, left);
    V2f top    = V2_line_line_intersection(line_p1, line_p2, top_left_corner, top_right_corner);
    CheckPoint(top_left_corner, top_right_corner, top);
    V2f right  = V2_line_line_intersection(line_p1, line_p2, top_right_corner, bottom_right_corner);
    CheckPoint(top_right_corner, bottom_right_corner, right);
    V2f bottom = V2_line_line_intersection(line_p1, line_p2, bottom_right_corner, bottom_left_corner);
    CheckPoint(bottom_right_corner, bottom_left_corner, bottom);
    
#undef CheckPoint
    
    return result;
}

///////////////////////////////////////////////////////////////////////////
// Matrices
///////////////////////////////////////////////////////////////////////////
typedef union M4x4
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
} M4x4;

inline M4x4 M4x4_identity()
{ M4x4 m; gj_ZeroStruct(&m); m.a[0] = 1.0f; m.a[5] = 1.0f; m.a[10] = 1.0f; m.a[15] = 1.0f; return m; }

#if defined(__cplusplus)
inline M4x4 M4x4_mul(M4x4 m, M4x4 n)
#else
inline M4x4 M4x4_mul_M4x4(M4x4 m, M4x4 n)
#endif
{
    m.m[0][0] = m.a[0]*n.a[0] + m.a[1]*n.a[4] + m.a[2]*n.a[8]  + m.a[3]*n.a[12];
    m.m[0][1] = m.a[0]*n.a[1] + m.a[1]*n.a[5] + m.a[2]*n.a[9]  + m.a[3]*n.a[13];
    m.m[0][2] = m.a[0]*n.a[2] + m.a[1]*n.a[6] + m.a[2]*n.a[10] + m.a[3]*n.a[14];
    m.m[0][3] = m.a[0]*n.a[3] + m.a[1]*n.a[7] + m.a[2]*n.a[11] + m.a[3]*n.a[15];

    m.m[1][0] = m.a[4]*n.a[0] + m.a[5]*n.a[4] + m.a[6]*n.a[8]  + m.a[7]*n.a[12];
    m.m[1][1] = m.a[4]*n.a[1] + m.a[5]*n.a[5] + m.a[6]*n.a[9]  + m.a[7]*n.a[13];
    m.m[1][2] = m.a[4]*n.a[2] + m.a[5]*n.a[6] + m.a[6]*n.a[10] + m.a[7]*n.a[14];
    m.m[1][3] = m.a[4]*n.a[3] + m.a[5]*n.a[7] + m.a[6]*n.a[11] + m.a[7]*n.a[15];

    m.m[2][0] = m.a[8]*n.a[0] + m.a[9]*n.a[4] + m.a[10]*n.a[8]  + m.a[11]*n.a[12];
    m.m[2][1] = m.a[8]*n.a[1] + m.a[9]*n.a[5] + m.a[10]*n.a[9]  + m.a[11]*n.a[13];
    m.m[2][2] = m.a[8]*n.a[2] + m.a[9]*n.a[6] + m.a[10]*n.a[10] + m.a[11]*n.a[14];
    m.m[2][3] = m.a[8]*n.a[3] + m.a[9]*n.a[7] + m.a[10]*n.a[11] + m.a[11]*n.a[15];

    m.m[3][0] = m.a[12]*n.a[0] + m.a[13]*n.a[4] + m.a[14]*n.a[8]  + m.a[15]*n.a[12];
    m.m[3][1] = m.a[12]*n.a[1] + m.a[13]*n.a[5] + m.a[14]*n.a[9]  + m.a[15]*n.a[13];
    m.m[3][2] = m.a[12]*n.a[2] + m.a[13]*n.a[6] + m.a[14]*n.a[10] + m.a[15]*n.a[14];
    m.m[3][3] = m.a[12]*n.a[3] + m.a[13]*n.a[7] + m.a[14]*n.a[11] + m.a[15]*n.a[15];

    return m;
}

#if defined(__cplusplus)
inline V4f M4x4_mul(M4x4 m, V4f v)
#else
inline V4f M4x4_mul_V4f(M4x4 m, V4f v)
#endif
{
    V4f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

#if defined(__cplusplus)
inline V3f M4x4_mul(M4x4 m, V3f v)
#else
inline V3f M4x4_mul_V3f(M4x4 m, V3f v)
#endif
{
    V3f result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];
    return result;
}

// NOTE: All of these use pass-by-reference (C++ only feature)
//       ported some that is used internally in this file to c below
#if defined(__cplusplus)
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
    f32 cos_value = gj_cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = gj_sin(angle_degrees * DEG_TO_RAD);
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
    f32 cos_value = gj_cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = gj_sin(angle_degrees * DEG_TO_RAD);
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
    f32 cos_value = gj_cos(angle_degrees * DEG_TO_RAD);
    f32 sin_value = gj_sin(angle_degrees * DEG_TO_RAD);
    m.a[0] = m.a[0] * cos_value - m.a[4] * sin_value;
    m.a[1] = m.a[1] * cos_value - m.a[5] * sin_value;
    m.a[2] = m.a[2] * cos_value - m.a[6] * sin_value;
    m.a[3] = m.a[3] * cos_value - m.a[7] * sin_value;
    m.a[4] = m.a[0] * sin_value + m.a[4] * cos_value;
    m.a[5] = m.a[1] * sin_value + m.a[5] * cos_value;
    m.a[6] = m.a[2] * sin_value + m.a[6] * cos_value;
    m.a[7] = m.a[3] * sin_value + m.a[7] * cos_value;    
}
#else
inline void M4x4_apply_translate(M4x4* m, f32 x, f32 y, f32 z)
{
    m->a[3]  += m->a[0]*x  + m->a[1]*y  + m->a[2]*z;
    m->a[7]  += m->a[4]*x  + m->a[5]*y  + m->a[6]*z;
    m->a[11] += m->a[8]*x  + m->a[9]*y  + m->a[10]*z;
    m->a[15] += m->a[12]*x + m->a[13]*y + m->a[14]*z;    
}

inline void
M4x4_apply_transpose(M4x4* m)
{
    gj_SwapArray(m->a, f32, 1,  4);
    gj_SwapArray(m->a, f32, 2,  8);
    gj_SwapArray(m->a, f32, 3,  12);
    gj_SwapArray(m->a, f32, 6,  9);
    gj_SwapArray(m->a, f32, 7,  13);
    gj_SwapArray(m->a, f32, 11, 14);
}
#endif

#if defined(__cplusplus)
M4x4 M4x4_model_view_matrix(V3f camera_pos, V3f camera_direction, V3f up, b32 translate = true)
#else
M4x4 M4x4_model_view_matrix(V3f camera_pos, V3f camera_direction, V3f up, b32 translate)
#define M4x4_model_view_matrix(camera_pos, camera_direction, up) M4x4_model_view_matrix(camera_pos, camera_direction, up, true)
#endif
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
#if defined(__cplusplus)
        M4x4_apply_translate(result, -camera_pos.x, -camera_pos.y, -camera_pos.z);
#else
        M4x4_apply_translate(&result, -camera_pos.x, -camera_pos.y, -camera_pos.z);
#endif
    }
    return result;
}

M4x4 M4x4_inverse_model_view_matrix(M4x4 model_view_matrix, V3f camera_pos)
{
    M4x4 result = model_view_matrix;
#if defined(__cplusplus)
    M4x4_apply_translate(result, camera_pos.x, camera_pos.y, camera_pos.z);
    M4x4_apply_transpose(result);
#else
    M4x4_apply_translate(&result, camera_pos.x, camera_pos.y, camera_pos.z);
    M4x4_apply_transpose(&result);
#endif
    return result;
}

M4x4 M4x4_fixed_forward_matrix(V3f camera_pos)
{
    M4x4 result = M4x4_identity();
    result.m[0][3] = -camera_pos.x;
    result.m[1][3] = -camera_pos.y;
    result.m[2][3] =  camera_pos.z;
    return result;
}

M4x4 M4x4_inverse_fixed_forward_matrix(M4x4 fixed_forward_matrix, V3f camera_pos)
{
    M4x4 result = M4x4_identity();
    result.m[0][3] =  camera_pos.x;
    result.m[1][3] =  camera_pos.y;
    result.m[2][3] = -camera_pos.z;
    return result;
}

M4x4 M4x4_projection_matrix(f32 field_of_view_degrees, f32 aspect_w_over_h,
                            f32 near_plane, f32 far_plane)
{
    M4x4 result = M4x4_identity();
    f32 t = gj_tan(field_of_view_degrees * DEG_TO_RAD / 2.0f);
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
    M4x4 result;
    gj_ZeroStruct(&result);
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

///////////////////////////////////////////////////////////////////////////
// 2D/3D Space/Perspective
// TODO: This can be sorted into categories better
///////////////////////////////////////////////////////////////////////////
inline V3f
V3f_fps_forward_vector(f32 yaw_radians, f32 pitch_radians)
{
    V3f v;
    v.x = gj_cos(yaw_radians) * gj_cos(pitch_radians);
    v.y = gj_sin(pitch_radians);
    v.z = gj_sin(yaw_radians) * gj_cos(pitch_radians);
    return v;
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
    if (det > -epsilon && det < epsilon) return gj_False;
    f32 inv_det = 1.0f / det;
    
    V3f tvec = V3_sub(ray_origin, v0);
    f32 u = V3_dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) return gj_False;

    V3f qvec = V3_cross(tvec, edge1); 
    f32 v = V3_dot(ray_direction, qvec) * inv_det; 
    if (v < 0.0f || u + v > 1.0f) return gj_False; 
 
    f32 t = V3_dot(edge2, qvec) * inv_det;
    *pos = V3_add(*pos, V3_mul(t, ray_direction));
        
    return gj_True;
}

b32 ray_rectangle_intersection(V3f min_min_pos, V3f max_min_pos,
                               V3f min_max_pos, V3f max_max_pos,
                               V3f ray_origin, V3f ray_direction,
                               V3f* pos)
{
    b32 result = (ray_triangle_intersection(min_min_pos, min_max_pos, max_max_pos, ray_origin, ray_direction, pos) |
                  ray_triangle_intersection(min_min_pos, max_max_pos, max_min_pos, ray_origin, ray_direction, pos));
    return result;
}

#if defined(__cplusplus)
b32 ray_rectangle_intersection(V3f min_min_pos, V3f max_max_pos,
                               V3f ray_origin, V3f ray_direction,
                               V3f* pos)
{
    b32 result;
    V3f max_min_pos = min_min_pos;
    max_min_pos.x = max_max_pos.x;
    V3f min_max_pos = min_min_pos;
    min_max_pos.y = max_max_pos.y;
    result = ray_rectangle_intersection(min_min_pos, max_min_pos, min_max_pos, max_max_pos,
                                        ray_origin, ray_direction, pos);
    return result;
}
#endif

///////////////////////////////////////////////////////////////////////////
// Grid Math
///////////////////////////////////////////////////////////////////////////
inline V2f
grid_get_cell_coord(V2f pos, f32 cell_width, f32 cell_height)
{
    V2f result;
    result.x = (f32)((s32)(pos.x / cell_width)) * cell_width;
    result.y = (f32)((s32)(pos.y / cell_height)) * cell_height;
    return result;
}

#endif
