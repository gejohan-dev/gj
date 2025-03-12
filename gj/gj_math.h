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

#define SQRT_3_F32 1.73205080757f

#define DEG_TO_RAD (PI_F32 / 180.0f)
#define RAD_TO_DEG (1.0f / DEG_TO_RAD)

///////////////////////////////////////////////////////////////////////////
// Bounds
///////////////////////////////////////////////////////////////////////////
#define gj_Max(a, b)     (((a) > (b)) ? (a) : (b))
#define gj_Min(a, b)     (((a) < (b)) ? (a) : (b))
#define gj_Abs(x)        (((x)<0) ? -(x) : (x))
#define gj_IsPositive(x) ((x) >= 0)

// TODO: Implement more exact version e.g. https://floating-point-gui.de/errors/comparison/
#define GJ_FLOAT_EPS 0.00001f
inline b32 gj_float_eq(f32 x, f32 y, f32 eps = GJ_FLOAT_EPS) { return gj_Abs(x - y) < eps; }
// x <= y
inline b32 gj_float_leq(f32 x, f32 y, f32 eps = GJ_FLOAT_EPS) { return x < y || gj_float_eq(x, y, eps); }
// x >= y
inline b32 gj_float_geq(f32 x, f32 y, f32 eps = GJ_FLOAT_EPS) { return x > y || gj_float_eq(x, y, eps); }

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
inline f32 gj_sqrt (f32 x)        { return sqrtf(x); }
inline f32 gj_pow  (f32 x, f32 n) { return powf(x,n); }
inline s32 gj_round(f32 x)        { return lroundf(x); }
inline s32 gj_floor(f32 x)        { return (s32)floorf(x); }

///////////////////////////////////////////////////////////////////////////
// Trigonometric Functions
///////////////////////////////////////////////////////////////////////////
inline f32 gj_sin(f32 x)          { return sinf(x); }
inline f32 gj_cos(f32 x)          { return cosf(x); }
inline f32 gj_tan(f32 x)          { return tanf(x); }
inline f32 gj_atan(f32 x)         { return atanf(x); }
inline f32 gj_atan2(f32 x, f32 y) { return atan2f(x, y); }

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
V2(i, s32);

#define V3(name, type)                          \
    typedef union V3##name {                    \
        struct { type x; type y; type z; };     \
        struct { type r; type g; type b; };     \
        type array[3];                          \
        type a[3];                              \
        type xy[2];                             \
        V2##name v2;                            \
    } V3##name
V3(f, f32);

#define V4(name, type)                                  \
    typedef union V4##name {                            \
        struct { type x;  type y;  type z;  type w; };  \
        struct { type r;  type g;  type b;  type a; };  \
        struct { type qw; type qx; type qy; type qz; }; \
        type xyz[3];                                    \
        V3##name v3;                                    \
        V2##name v2;                                    \
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
inline V2f V2_rotate           (V2f v, f32 a_radians)
{
    V2f result;
    result.x = gj_cos(a_radians) * v.x - gj_sin(a_radians) * v.y;
    result.y = gj_sin(a_radians) * v.x + gj_cos(a_radians) * v.y;
    return result;
}
inline b32 V2_equal            (V2f v, V2f u)     { return gj_float_eq(v.x, u.x) && gj_float_eq(v.y, u.y); }
inline V2f V2_neg              (V2f v)            { return {-v.x, -v.y}; }

// Transform to other data types
inline V3f V2_to_V3(V2f v, f32 z = 0.0f)                   { V3f v3f; v3f.x = v.x; v3f.y = v.y; v3f.z = z; return v3f; }

// Area = 1/2 * (x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2))
f32 V2_triangle_area(V2f t1, V2f t2, V2f t3)
{
    f32 result;
    f32 a = t1.x * (t2.y - t3.y);
    f32 b = t2.x * (t3.y - t1.y);
    f32 c = t3.x * (t1.y - t2.y);
    result = gj_Abs((a + b + c) / 2.0f);
    return result;
}

inline V3f V3_sub(V3f,V3f);
inline V3f V3_cross(V3f,V3f);
inline f32 V3_dot(V3f,V3f);
bool _same_side(V2f p1, V2f p2, V2f a, V2f b)
{
    bool result;
    V3f ba = V3_sub(V2_to_V3(b), V2_to_V3(a));
    V3f v1 = V3_cross(ba, V3_sub(V2_to_V3(p1), V2_to_V3(a)));
    V3f v2 = V3_cross(ba, V3_sub(V2_to_V3(p2), V2_to_V3(a)));
    result = gj_float_geq(V3_dot(v1, v2), 0.0f);
    return result;
}
bool V2_point_on_triangle(V2f p, V2f t1, V2f t2, V2f t3)
{
    bool result;
    result = (_same_side(p, t1, t2, t3) &&
              _same_side(p, t2, t1, t3) &&
              _same_side(p, t3, t1, t2));
    return result;
}

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
inline V3f V3_mul        (V3f v0, V3f v1)             { V3f v; v.x = (v0.x * v1.x); v.y = (v0.y * v1.y); v.z = (v0.z * v1.z); return v; }
inline V3f V3_mul        (f32 c, V3f v0)              { V3f v; v.x = (c * v0.x); v.y = (c * v0.y); v.z = (c * v0.z); return v; }
inline V3f V3_mul        (V3f v0, f32 c)              { return V3_mul(c, v0); }
inline V3f V3_mul        (f32 x, f32 y, f32 z, f32 c) { V3f v; v.x = (c * x); v.y = (c * y); v.z = (c * z); return v; }
#else
inline V3f V3_mul        (f32 c, V3f v0)              { V3f v; v.x = (c * v0.x); v.y = (c * v0.y); v.z = (c * v0.z); return v; }
#endif
inline V3f V3_div        (V3f v0, V3f v1)             { V3f v; v.x = (v0.x / v1.x); v.y = (v0.y / v1.y); v.z = (v0.z / v1.z); return v; }
inline V3f V3_normalize  (V3f v)
{
    f32 l = V3_length(v);
    if (l != 0.0f)
    {
        v.x = (v.x / l); v.y = (v.y / l); v.z = (v.z / l);
    }
    else
    {
        // TODO: Logging
    }
    return v;
}
inline f32 V3_dot        (V3f v0, V3f v1)             { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z; }
inline V3f V3_cross      (V3f v0, V3f v1)             { V3f v; v.x = (v0.y * v1.z - v0.z * v1.y); v.y = (v0.z * v1.x - v0.x * v1.z); v.z = (v0.x * v1.y - v0.y * v1.x); return v; }
inline V3f V3_neg        (V3f v)                      { return {-v.x, -v.y, -v.z}; }
inline f32 V3_distance   (V3f v0, V3f v1)             { return V3_length(V3_sub(v0, v1)); }
inline b32 V3_equal      (V3f v0, V3f v1)             { return gj_float_eq(v0.x, v1.x) && gj_float_eq(v0.y, v1.y) && gj_float_eq(v0.z, v1.z); }

inline V2f V3_xz(V3f v) { return {v.x, v.z}; }

V3f V3_triangle_normal(V3f p1, V3f p2, V3f p3)
{
    V3f result;
    gj__ZeroStruct(result);
    V3f normal_comp_1 = V3_sub(p2, p1);
    V3f normal_comp_2 = V3_sub(p3, p1);
    result = V3_cross(normal_comp_1, normal_comp_2);
    result = V3_normalize(result);
    return result;
}

V3f V3_project(V3f v0, V3f v1)
{
    V3f result;
    f32 d = V3_dot(v0, v1);
    f32 l = V3_length(v1);
    result = V3_mul(d / (l * l), v1);
    return result;
}

V3f V3_project_onto_triangle(V3f p, V3f t1, V3f t2, V3f t3)
{
    V3f result;
    V3f n = V3_triangle_normal(t1, t2, t3);
    V3f p_orig = V3_sub(p, t1);
    f32 dist = V3_dot(p_orig, n);
    result = V3_sub(p, V3_mul(dist, n));
    /* result = V3_sub(p, V3_project(p, n)); */
    return result;
}

f32 V3_triangle_area(V3f t1, V3f t2, V3f t3)
{
    f32 result;
    V3f t1_t2 = V3_sub(t2, t1);
    V3f t1_t3 = V3_sub(t3, t1);
    V3f c = V3_cross(t1_t2, t1_t3);
    result = V3_length(c) / 2.0f;
    return result;
}

// NOTE: Assumes p is projected onto the plane of the triangle already
bool V3_point_on_triangle(V3f p, V3f t1, V3f t2, V3f t3)
{
    bool result;
    f32 t_area       = V3_triangle_area(t1, t2, t3);
    V3f p_t1         = V3_sub(t1, p);
    V3f p_t2         = V3_sub(t2, p);
    V3f p_t3         = V3_sub(t2, p);
    f32 p_t2_t3_area = V3_triangle_area(p, p_t2, p_t3);
    f32 p_t3_t1_area = V3_triangle_area(p, p_t3, p_t1);
    f32 a            = p_t2_t3_area / t_area;
    f32 b            = p_t3_t1_area / t_area;
    f32 y            = 1 - a - b;
    result = (gj_float_geq(a, 0.0f) && gj_float_leq(a, 1.0f) &&
              gj_float_geq(b, 0.0f) && gj_float_leq(b, 1.0f) &&
              gj_float_geq(y, 0.0f) && gj_float_leq(y, 1.0f) &&
              gj_float_eq(a + b + y, 1.0f));
    return result;
}

inline V4f V4_mul       (f32 c, V4f v0)  { V4f v; v.x = (c * v0.x); v.y = (c * v0.y); v.z = (c * v0.z);  v.w = (c * v0.w); return v; }
inline f32 V4_length    (V4f v0)         { return gj_sqrt(v0.x * v0.x + v0.y * v0.y + v0.z * v0.z + v0.w * v0.w); }
inline V4f V4_normalize (V4f v)
{
    f32 l = V4_length(v);
    if (l != 0.0f)
    {
        v.x = (v.x / l); v.y = (v.y / l); v.z = (v.z / l); v.w = (v.w / l);
    }
    else
    {
        // TODO: Logging
    }
    return v;
}
inline f32 V4_dot       (V4f v0, V4f v1) { return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w; }

///////////////////////////////////////////////////////////////////////////
// 2D Collision
///////////////////////////////////////////////////////////////////////////
#define V2_LINE_NO_COLLISION FLT_MAX
// Note: Comes from
// https://web.archive.org/web/20060911055655/http://local.wasp.uwa.edu.au/%7Epbourke/geometry/lineline2d/
static V2f
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
    if (!gj_float_eq(den, 0.0f, eps))
    {
        f32 ua_num = (x4-x3)*(y1-y3) - (y4-y3)*(x1-x3);
        f32 ua = ua_num / den;
        if (ua > 0.0f && !gj_float_eq(ua, 0.0f, eps))
        {
            result.x = x1 + ua * (x2 - x1);
            result.y = y1 + ua * (y2 - y1);
        }
    }
    
    return result;
}

static b32
#pragma warning(suppress: 4505)
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

inline b32
V2_rectangle_contains_point(V2f rectangle_start, V2f rectangle_end, V2f point)
{
    f32 r_min_x = gj_Min(rectangle_start.x, rectangle_end.x);
    f32 r_max_x = gj_Max(rectangle_start.x, rectangle_end.x);
    f32 r_min_y = gj_Min(rectangle_start.y, rectangle_end.y);
    f32 r_max_y = gj_Max(rectangle_start.y, rectangle_end.y);
    
    return (gj_float_leq(r_min_x, point.x) &&
            gj_float_geq(r_max_x, point.x) &&
            gj_float_leq(r_min_y, point.y) &&
            gj_float_geq(r_max_y, point.y));
}

inline b32
V2_rectangle_contains_rectangle(V2f r1_min_pos, V2f r1_max_pos,
                                V2f r2_min_pos, V2f r2_max_pos)
{
    V2f r1_min_max_pos = {r1_min_pos.x, r1_max_pos.y};
    V2f r1_max_min_pos = {r1_max_pos.x, r1_min_pos.y};

    V2f r2_min_max_pos = {r2_min_pos.x, r2_max_pos.y};
    V2f r2_max_min_pos = {r2_max_pos.x, r2_min_pos.y};
    
    return (V2_rectangle_contains_point(r2_min_pos, r2_max_pos, r1_min_pos) ||
            V2_rectangle_contains_point(r2_min_pos, r2_max_pos, r1_max_pos) ||
            V2_rectangle_contains_point(r2_min_pos, r2_max_pos, r1_min_max_pos) ||
            V2_rectangle_contains_point(r2_min_pos, r2_max_pos, r1_max_min_pos) ||
            V2_rectangle_contains_point(r1_min_pos, r1_max_pos, r2_min_pos) ||
            V2_rectangle_contains_point(r1_min_pos, r1_max_pos, r2_max_pos) ||
            V2_rectangle_contains_point(r1_min_pos, r1_max_pos, r2_min_max_pos) ||
            V2_rectangle_contains_point(r1_min_pos, r1_max_pos, r2_max_min_pos));
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

    struct
    {
        f32 _00; f32 _01; f32 _02; f32 _03;
        f32 _10; f32 _11; f32 _12; f32 _13;
        f32 _20; f32 _21; f32 _22; f32 _23;
        f32 _30; f32 _31; f32 _32; f32 _33;
    };

    struct
    {
        V3f camera_right;   f32 _row_0;
        V3f camera_up;      f32 _row_1;
        V3f camera_forward; f32 _row_2;
        V4f _row_3;
    };
} M4x4;

V3f M4x4_get_x_row(M4x4 m)
{
    return {m._00, m._01, m._02};
}

V3f M4x4_get_y_row(M4x4 m)
{
    return {m._10, m._11, m._12};
}

V3f M4x4_get_z_row(M4x4 m)
{
    return {m._20, m._21, m._22};
}

inline M4x4 M4x4_identity()
{ M4x4 m; gj__ZeroStruct(m); m.a[0] = 1.0f; m.a[5] = 1.0f; m.a[10] = 1.0f; m.a[15] = 1.0f; return m; }

#if defined(__cplusplus)
inline M4x4 M4x4_mul(M4x4 m, M4x4 n)
#else
inline M4x4 M4x4_mul_M4x4(M4x4 m, M4x4 n)
#endif
{
    M4x4 result = {};
    
    result._00 = m._00 * n._00 + m._01 * n._10 + m._02 * n._20 + m._03 * n._30;
    result._01 = m._00 * n._01 + m._01 * n._11 + m._02 * n._21 + m._03 * n._31;
    result._02 = m._00 * n._02 + m._01 * n._12 + m._02 * n._22 + m._03 * n._32;
    result._03 = m._00 * n._03 + m._01 * n._13 + m._02 * n._23 + m._03 * n._33;

    result._10 = m._10 * n._00 + m._11 * n._10 + m._12 * n._20 + m._13 * n._30;
    result._11 = m._10 * n._01 + m._11 * n._11 + m._12 * n._21 + m._13 * n._31;
    result._12 = m._10 * n._02 + m._11 * n._12 + m._12 * n._22 + m._13 * n._32;
    result._13 = m._10 * n._03 + m._11 * n._13 + m._12 * n._23 + m._13 * n._33;

    result._20 = m._20 * n._00 + m._21 * n._10 + m._22 * n._20 + m._23 * n._30;
    result._21 = m._20 * n._01 + m._21 * n._11 + m._22 * n._21 + m._23 * n._31;
    result._22 = m._20 * n._02 + m._21 * n._12 + m._22 * n._22 + m._23 * n._32;
    result._23 = m._20 * n._03 + m._21 * n._13 + m._22 * n._23 + m._23 * n._33;

    result._30 = m._30 * n._00 + m._31 * n._10 + m._32 * n._20 + m._33 * n._30;
    result._31 = m._30 * n._01 + m._31 * n._11 + m._32 * n._21 + m._33 * n._31;
    result._32 = m._30 * n._02 + m._31 * n._12 + m._32 * n._22 + m._33 * n._32;
    result._33 = m._30 * n._03 + m._31 * n._13 + m._32 * n._23 + m._33 * n._33;

    return result;
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

static inline M4x4
M4x4_translation_matrix(V3f translation)
{
    M4x4 result = {
        1.0f, 0.0f, 0.0f, translation.x,
        0.0f, 1.0f, 0.0f, translation.y,
        0.0f, 0.0f, 1.0f, translation.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return result;
}

static inline V3f
M4x4_get_translation(M4x4 m) { return {m.m[0][3], m.m[1][3], m.m[2][3]}; }

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

inline void M4x4_apply_translate(M4x4* m, f32 x, f32 y, f32 z)
{
    m->a[3]  += m->a[0]*x  + m->a[1]*y  + m->a[2]*z;
    m->a[7]  += m->a[4]*x  + m->a[5]*y  + m->a[6]*z;
    m->a[11] += m->a[8]*x  + m->a[9]*y  + m->a[10]*z;
    m->a[15] += m->a[12]*x + m->a[13]*y + m->a[14]*z;
}
inline void M4x4_apply_translate(M4x4* m, V3f v) { M4x4_apply_translate(m, v.x, v.y, v.z); }

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
    f32 _0 = m.a[0] * cos_value - m.a[4] * sin_value;
    f32 _1 = m.a[1] * cos_value - m.a[5] * sin_value;
    f32 _2 = m.a[2] * cos_value - m.a[6] * sin_value;
    f32 _3 = m.a[3] * cos_value - m.a[7] * sin_value;
    f32 _4 = m.a[0] * sin_value + m.a[4] * cos_value;
    f32 _5 = m.a[1] * sin_value + m.a[5] * cos_value;
    f32 _6 = m.a[2] * sin_value + m.a[6] * cos_value;
    f32 _7 = m.a[3] * sin_value + m.a[7] * cos_value;
    m.a[0] = _0;
    m.a[1] = _1;
    m.a[2] = _2;
    m.a[3] = _3;
    m.a[4] = _4;
    m.a[5] = _5;
    m.a[6] = _6;
    m.a[7] = _7;
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

inline V3f
M4x4_get_scaling(M4x4 m)
{
    return {m.m[0][0], m.m[1][1], m.m[2][2]};
}

inline void
M4x4_apply_scaling(M4x4* m, f32 scale)
{
    m->m[0][0] *= scale;
    m->m[1][1] *= scale;
    m->m[2][2] *= scale;
}

#if defined(__cplusplus)
M4x4 M4x4_model_view_matrix(V3f camera_pos, V3f camera_direction, V3f up, b32 translate = true)
#else
M4x4 M4x4_model_view_matrix(V3f camera_pos, V3f camera_direction, V3f up, b32 translate)
#define M4x4_model_view_matrix(camera_pos, camera_direction, up) M4x4_model_view_matrix(camera_pos, camera_direction, up, true)
#endif
{
    V3f camera_z = V3_normalize(camera_direction);
    V3f camera_x = V3_normalize(V3_cross(up, camera_z));
    V3f camera_y = V3_normalize(V3_cross(camera_z, camera_x));
    M4x4 result = {
        camera_x.x, camera_x.y, camera_x.z, 0.0f,
        camera_y.x, camera_y.y, camera_y.z, 0.0f,
        camera_z.x, camera_z.y, camera_z.z, 0.0f,
        0.0f,       0.0f,       0.0f,       1.0f
    };
    if (translate)
    {
        result._03 = -V3_dot(camera_x, camera_pos);
        result._13 = -V3_dot(camera_y, camera_pos);
        result._23 = -V3_dot(camera_z, camera_pos);
    }
    return result;
}

M4x4 M4x4_inverse_model_view_matrix(M4x4 model_view_matrix)
{
    M4x4 result = model_view_matrix;
    result._03 = 0.0f; 
    result._13 = 0.0f;
    result._23 = 0.0f;
#if defined(__cplusplus)
    M4x4_apply_transpose(result);
#else
    M4x4_apply_transpose(&result);
#endif
    V3f t = {model_view_matrix._03, model_view_matrix._13, model_view_matrix._23};
    result._03 = -V3_dot(t, M4x4_get_x_row(result));
    result._13 = -V3_dot(t, M4x4_get_y_row(result));
    result._23 = -V3_dot(t, M4x4_get_z_row(result));
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
    M4x4 result = fixed_forward_matrix;
    gj_AssertDebug(result.a[0] != fixed_forward_matrix.a[0]);
    result.m[0][3] =  camera_pos.x;
    result.m[1][3] =  camera_pos.y;
    result.m[2][3] = -camera_pos.z;
    return result;
}

M4x4 M4x4_projection_matrix(f32 field_of_view_degrees, f32 aspect_w_over_h,
                            f32 near_plane, f32 far_plane)
{
    M4x4 result = M4x4_identity();
#if 0
    f32 t = gj_tan(field_of_view_degrees * DEG_TO_RAD / 2.0f);
    f32 height = near_plane * t;
    f32 width = height * aspect_w_over_h;
    result.m[0][0] = near_plane / width;
    result.m[1][1] = near_plane / height;
    result.m[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
    result.m[2][3] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    result.m[3][2] = -1.0f;
    result.m[3][3] = 0;
#endif
    f32 t = gj_tan(field_of_view_degrees * DEG_TO_RAD / 2.0f);
    result._00 = 1.0f / t;
    result._11 = result._00 * aspect_w_over_h;
    result._22 = far_plane / (far_plane - near_plane);
    result._23 = -far_plane * near_plane / (far_plane - near_plane);
    result._32 = 1.0f;
    result._33 = 0.0f;
    return result;
}

M4x4 M4x4_inverse_projection_matrix(M4x4 projection_matrix)
{
    M4x4 result;
    gj__ZeroStruct(result);
#if 0
    result.m[0][0] = 1.0f / projection_matrix.m[0][0];
    result.m[1][1] = 1.0f / projection_matrix.m[1][1];
    result.m[2][3] = 0.0f;
    result.m[2][3] = -1.0f;
    f32 inv_d = 1.0f / projection_matrix.m[2][3];
    result.m[3][2] = inv_d;
    result.m[3][3] = inv_d * projection_matrix.m[2][2];
#endif
    result._00 = 1.0f / projection_matrix._00;
    result._11 = 1.0f / projection_matrix._11;
    result._23 = 1.0f;
    result._32 = 1.0f /projection_matrix._23;
    result._33 = projection_matrix._22 / projection_matrix._23;
    return result;
}

/* M4x4 M4x4_orthographic_matrix(f32 aspect_w_over_h, f32 near_plane, f32 far_plane) */
M4x4 M4x4_orthographic_matrix(f32 l, f32 r, f32 t, f32 b, f32 n, f32 f)
/* M4x4 M4x4_orthographic_matrix(f32 width, f32 height, f32 near_plane, f32 far_plane) */
{
    M4x4 result = M4x4_identity();
#if 0
    result.m[1][1] = aspect_w_over_h;
    result.m[2][2] = 2.0f / (near_plane - far_plane);
    result.m[2][3] = (near_plane + far_plane) / (near_plane - far_plane);
#elif 0
    result._00 =  2.0f / width;
    result._03 = -1.0f;
    result._11 =  2.0f / height;
    result._13 = -1.0f;
    result._22 = -2.0f / (far_plane - near_plane);
    result._23 = -(far_plane + near_plane) / (far_plane - near_plane);
#elif 0
    result._00 = 2.0f / width;
    result._03 = -1.0f;
    result._11 = 2.0f / height;
    result._13 = -1.0f;
    result._22 = 1.0f / (near_plane - far_plane);
    result._32 = near_plane / (near_plane - far_plane);
#elif 1
    result.m[0][0] = 2 / (r - l);
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2 / (t - b);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = -2 / (f - n);
    result.m[2][3] = 0;

    result.m[3][0] = -(r + l) / (r - l);
    result.m[3][1] = -(t + b) / (t - b);
    result.m[3][2] = -(f + n) / (f - n);
    result.m[3][3] = 1;
#endif
    return result;
}

M4x4 M4x4_inverse_orthographic_matrix(f32 width, f32 height, f32 near_plane, f32 far_plane)
{
    M4x4 result = M4x4_identity();
    result._00 = width / 2.0f;
    result._03 = width / 2.0f;
    result._11 = height / 2.0f;
    result._13 = height / 2.0f;
    result._22 = (far_plane - near_plane) / -2.0f;
    return result;
}

M4x4 M4x4_from_axis_angle(V3f axis, f32 angle)
{
    M4x4 result;
    
    f32 c = gj_cos(angle);
    f32 s = gj_sin(angle);
    f32 t = 1.0 - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	// if (magnitude==0) throw error;
	// axis.x /= magnitude;
	// axis.y /= magnitude;
	// axis.z /= magnitude;

    result._00 = c + axis.x*axis.x*t;
    result._11 = c + axis.y*axis.y*t;
    result._22 = c + axis.z*axis.z*t;


    f32 tmp1 = axis.x*axis.y*t;
    f32 tmp2 = axis.z*s;
    result._10 = tmp1 + tmp2;
    result._01 = tmp1 - tmp2;
    tmp1 = axis.x*axis.z*t;
    tmp2 = axis.y*s;
    result._20 = tmp1 - tmp2;
    result._02 = tmp1 + tmp2;
    tmp1 = axis.y*axis.z*t;
    tmp2 = axis.x*s;
    result._21 = tmp1 + tmp2;
    result._12 = tmp1 - tmp2;

    return result;
}

///////////////////////////////////////////////////////////////////////////
// Quaternion
///////////////////////////////////////////////////////////////////////////
V4f Quat_create(V3f axis, f32 angle_radians)
{
    V4f result;
    axis = V3_normalize(axis);
    f32 half_angle_radians = angle_radians / 2.0f;
    f32 sin = gj_sin(half_angle_radians);
    result.qw = gj_cos(half_angle_radians);
    result.qx = sin * axis.x;
    result.qy = sin * axis.y;
    result.qz = sin * axis.z;
    
    return result;
}

V4f Quat_create(V3f point)
{
    return Quat_create(point, 0);
}

V4f Quat_from_rotation_matrix(M4x4 m)
{
    V4f result;
    f32 trace = m._00 + m._11 + m._22;
    f32 s;
    
    if (trace >= 0.0f)
    {
        s = gj_sqrt(trace + m._33);
        result.qw = s * 0.5f;
        s = 0.5f / s;
        result.qx = (m._21 - m._12) * s;
        result.qy = (m._02 - m._20) * s;
        result.qz = (m._10 - m._01) * s;
    }
    else
    {
        int h = 0;
        if (m._11 > m._00) h = 1;
        if (m._22 > m.m[h][h]) h = 2;
        switch (h) {
#define CaseMacro(i,j,k,I,J,K)                                          \
            case I:                                                     \
                s = gj_sqrt((m.m[I][I] - (m.m[J][J] + m.m[K][K])) + m._33); \
                result.i = s * 0.5f;                                    \
                s = 0.5f / s;                                           \
                result.j = (m.m[I][J] + m.m[J][I]) * s;                 \
                result.k = (m.m[K][I] + m.m[I][K]) * s;                 \
                result.w = (m.m[K][J] - m.m[J][K]) * s;                 \
                break;
            CaseMacro(x, y, z, 0, 1, 2);
            CaseMacro(y, z, x, 1, 2, 0);
            CaseMacro(z, x, y, 2, 0, 1);
#undef CaseMacro
        }
    }

    return result;
}

M4x4 Quat_to_rotation_matrix(V4f q)
{
    f32 _2xx = 2.0f * q.qx * q.qx;
    f32 _2yy = 2.0f * q.qy * q.qy;
    f32 _2zz = 2.0f * q.qz * q.qz;
    f32 _2xy = 2.0f * q.qx * q.qy;
    f32 _2xz = 2.0f * q.qx * q.qz;
    f32 _2yz = 2.0f * q.qy * q.qz;
    f32 _2wx = 2.0f * q.qw * q.qx;
    f32 _2wz = 2.0f * q.qw * q.qz;
    f32 _2wy = 2.0f * q.qw * q.qy;
    M4x4 result = {
        1.0f - _2yy - _2zz, _2xy - _2wz,        _2xz + _2wy,        0.0f,
        _2xy + _2wz,        1.0f - _2xx - _2zz, _2yz - _2wz,        0.0f,
        _2xz - _2wy,        _2yz + _2wx,        1.0f - _2xx - _2yy, 0.0f,
        0.0f,               0.0f,               0.0f,               1.0f};
    return result;
}

V4f Quat_conjugate(V4f q)
{
    return {q.qw, -q.qx, -q.qy, -q.qz};
}

V4f Quat_mul(V4f q1, V4f q2)
{
    return {q1.qw * q2.qw - q1.qx * q2.qx - q1.qy * q2.qy - q1.qz * q2.qz,
            q1.qw * q2.qx + q1.qx * q2.qw + q1.qy * q2.qz - q1.qz * q2.qy,
            q1.qw * q2.qy + q1.qy * q2.qw + q1.qz * q2.qx - q1.qx * q2.qz,
            q1.qw * q2.qz + q1.qz * q2.qw + q1.qx * q2.qy - q1.qy * q2.qx};
}

V4f Quat_mul(V4f q, f32 v)
{
    return {q.qw * v, q.qx * v, q.qy * v, q.qz * v};
}

V4f Quat_inverse(V4f q)
{
    return Quat_mul(Quat_conjugate(q), 1.0f / V4_length(q));
}

V4f Quat_difference(V4f q1, V4f q2)
{
    return Quat_mul(q2, Quat_inverse(q1));
}

V4f Quat_exp(V4f q, f32 exp)
{
    V4f result = q;
    if (fabs(result.w) < 0.9999f)
    {
        f32 a = acosf(result.w);
        f32 na = a * exp;
        result.w = gj_cos(na);
        f32 m = gj_sin(na) / gj_sin(a);
        result.x *= m;
        result.y *= m;
        result.z *= m;
    }
    else
    {
        // TODO: Logging?
    }
    return result;
}

V4f Quat_slerp(V4f q1, V4f q2, f32 t)
{
    V4f result;

    f32 cosOmega = V4_dot(q1, q2);
    if (cosOmega < 0.0f)
    {
        q2.qw = -q2.qw;
        q2.qx = -q2.qx;
        q2.qy = -q2.qy;
        q2.qz = -q2.qz;
        cosOmega = -cosOmega;
    }

    f32 k0;
    f32 k1;
    if (cosOmega > 0.9999f)
    {
        k0 = 1.0f - t;
        k1 = t;
    }
    else
    {
        f32 sinOmega = gj_sqrt(1.0f - cosOmega * cosOmega);
        f32 omega = gj_atan2(sinOmega, cosOmega);
        f32 oneOverSinOmega = 1.0f / sinOmega;
        k0 = gj_sin((1.0f - t) * omega) * oneOverSinOmega;
        k1 = gj_sin(t * omega) * oneOverSinOmega;
    }

    result.qw = q1.qw * k0 + q2.qw * k1;
    result.qx = q1.qx * k0 + q2.qx * k1;
    result.qy = q1.qy * k0 + q2.qy * k1;
    result.qz = q1.qz * k0 + q2.qz * k1;
    
    return V4_normalize(result);
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

b32 V3_box_contains_point(V3f box_start, V3f box_end, V3f pos)
{
    V3f box_half = V3_mul(0.5f, V3_sub(box_end, box_start));
    V3f box_center = V3_add(box_start, box_half);
    V3f dx = {1.0f, 0.0f, 0.0f};
    V3f dy = {0.0f, 1.0f, 0.0f};
    V3f dz = {0.0f, 0.0f, 1.0f};
    V3f d = V3_sub(pos, box_center);
    return (gj_Abs(V3_dot(d, dx)) <= box_half.x &&
            gj_Abs(V3_dot(d, dy)) <= box_half.y &&
            gj_Abs(V3_dot(d, dz)) <= box_half.z);
}

V3f V3f_get_mouse_ray(M4x4 inverse_model_view_matrix, M4x4 inverse_projection_matrix,
                      u32 window_width, u32 window_height,
                      V2f mouse_pos)
{
    V3f result;
    V2f mouse_ndc;
    mouse_ndc.x = (2.0f * mouse_pos.x) / window_width - 1.0f;
    mouse_ndc.y = (2.0f * mouse_pos.y) / window_height - 1.0f;
    V4f mouse_eye = M4x4_mul(inverse_projection_matrix, V4f{mouse_ndc.x, mouse_ndc.y, 1.0f, 1.0f});
    mouse_eye.z = 1.0f;
    mouse_eye.w = 0.0f;
    mouse_eye = M4x4_mul(inverse_model_view_matrix, mouse_eye);
    result = {mouse_eye.x, mouse_eye.y, mouse_eye.z};
    result = V3_normalize(result);
    return result;
}

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
    if (t < 0.0f)
    {
        return gj_False;
    }
    *pos = V3_add(ray_origin, V3_mul(t, ray_direction));
        
    return gj_True;
}

b32 ray_rectangle_intersection(V3f min_min_pos, V3f max_min_pos,
                               V3f min_max_pos, V3f max_max_pos,
                               V3f ray_origin, V3f ray_direction,
                               V3f* pos)
{
    b32 result = (ray_triangle_intersection(min_min_pos, min_max_pos, max_max_pos, ray_origin, ray_direction, pos) ||
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

b32 ray_box_intersection(V3f ray_origin, V3f ray_direction,
                         V3f box_min, V3f box_max,
                         V3f* hit_pos)
{
    V3f t_low   = V3_div(V3_sub(box_min, ray_origin), ray_direction);
    V3f t_high  = V3_div(V3_sub(box_max, ray_origin), ray_direction);
    f32 t_close = gj_Max(gj_Max(gj_Min(t_low.x, t_high.x), gj_Min(t_low.y, t_high.y)), gj_Min(t_low.z, t_high.z));
    f32 t_far   = gj_Min(gj_Min(gj_Max(t_low.x, t_high.x), gj_Max(t_low.y, t_high.y)), gj_Max(t_low.z, t_high.z));
    if (hit_pos)
    {
        *hit_pos = V3_add(ray_origin, V3_mul(t_close, ray_direction));
    }
    return t_close >= 0.0f && gj_float_leq(t_close, t_far);
}

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

///////////////////////////////////////////////////////////////////////////
// Random Number Generation
///////////////////////////////////////////////////////////////////////////
struct RandomSeries
{
    u32 a;
};

/* The state word must be initialized to non-zero */
inline u32
gj_math_get_random_u32(RandomSeries* random_series)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    u32 result = random_series->a;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    return random_series->a = result;
}

inline u32
gj_math_get_bounded_random_u32(RandomSeries* random_series, u32 min, u32 max)
{
    u32 result = gj_math_get_random_u32(random_series);
    result %= max - min;
    result += min;
    return result;
}

inline u32
gj_math_get_bounded_inclusive_random_u32(RandomSeries* random_series, u32 min, u32 max)
{
    u32 result = gj_math_get_bounded_random_u32(random_series, min, max + 1);
    return result;
}

#endif
