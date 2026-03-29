#pragma once
#include <raylib.h>
#include "common.h"

#define DEFINE_IS_MOUSE(POST)                                  \
inline bool isMouse##POST(int _mouse_button=MOUSE_BUTTON_LEFT) \
{ return IsMouseButton##POST(_mouse_button); }                 \
struct __force_semicolon_##POST {}


using Vec2 = Vector2;
using Rect = Rectangle;

constexpr inline Vec2 operator+ (Vec2 v1, Vec2 v2) {
    return {v1.x+v2.x, v1.y+v2.y};
}
constexpr inline Vec2 operator- (Vec2 v1, Vec2 v2) {
    return {v1.x-v2.x, v1.y-v2.y};
}

inline const char* vepr(const Vec2& vec, bool parens=true) {
    thread_local static char buf[64];
    char* p = buf;

    if (parens) *p++ = '(';
    p = std::to_chars(p, buf+sizeof(buf), vec.x).ptr;
    *p++ = ','; *p++ = ' ';
    p = std::to_chars(p, buf+sizeof(buf), vec.y).ptr;
    if (parens) *p++ = ')';
    *p = '\0';

    return buf;
}
inline string veprs(const Vec2& vec, bool parens=true) {
    thread_local static string repr = {};
    (repr.clear(), repr)
        .append((parens)? "(" : "")
        .append(std::to_string(vec.x))
        .append(", ")
        .append(std::to_string(vec.y))
        .append((parens)? ")" : "")
    ;
    return repr;
}

struct Circle {
    Vec2 pos = {};
    f32 r = {};
    // uint32 segments = SEGMENTS;
};

inline bool isKeyPressing(int key) {
    return (IsKeyPressed(key) || IsKeyPressedRepeat(key));
}

DEFINE_IS_MOUSE(Down);
DEFINE_IS_MOUSE(Pressed);
DEFINE_IS_MOUSE(Up);
DEFINE_IS_MOUSE(Released);

inline bool isMouseInside(Vec2 pos, f32 radius) {
    return CheckCollisionPointCircle(
        GetMousePosition(), pos, radius
    );
}

inline bool isInsideCircle(Vec2 pos, Vec2 center, f32 radius) {
    return CheckCollisionPointCircle(pos, center, radius);
}
inline bool isInsideEllipse(Vec2 pos, Vec2 center, Vec2 radius) {
    f32 scale_x = (pos.x - center.x) / radius.x;
    f32 scale_y = (pos.y - center.y) / radius.y;
    return (scale_x*scale_x + scale_y*scale_y) <= 1.0f;
}
inline bool isInsideRect(Vec2 pos, Vec2 rec_pos, Vec2 size) {
    return (
        pos.x >= rec_pos.x &&
        pos.x <= rec_pos.x + size.x &&
        pos.y >= rec_pos.y &&
        pos.y <= rec_pos.y + size.y
    );
}

template<typename Float>
requires std::is_arithmetic_v<Float> LD_NOINLINE
Float randf(Float x, Float y) {
    thread_local static std::mt19937_64 prng(
        std::random_device{}.operator()()  // hate ambiguity
    );
    return std::uniform_real_distribution<Float>(x, y)(prng);
}

constexpr inline f32 distance_2(Vec2 p1, Vec2 p2) {
    return(
    (p2.x - p1.x) * (p2.x - p1.x)
    + (p2.y - p1.y) * (p2.y - p1.y)
    );
}

inline f32 distance(Vec2 p1, Vec2 p2) {
    return std::sqrtf(distance_2(p1, p2));
}


LD_NOINLINE bool checkCollisionEllipses(Vec2 p1, Vec2 rr1, Vec2 p2, Vec2 rr2)
{
    f32 max_dist_1 = std::max(rr1.x, rr1.y);
    f32 max_dist_2 = std::max(rr2.x, rr2.y);
    f32 dist = distance(p1, p2);
    // farther than the sum of the radii
    if (dist > max_dist_1 + max_dist_2) {
        return false;
    }
    else if (isInsideEllipse(p1, p2, rr2)) {
        return true;
    }
    else if (isInsideEllipse(p2, p1, rr1)) {
        return true;
    }

    f32 r = std::max({rr1.x, rr1.y, rr2.x, rr2.y});
    uint slices = (uint)(8.0f + 12.0f * sqrtf(r));  // tweakable
    // loop over slices of ellipse1 and get angled points
    for (uint i=0;  i < slices;  ++i) {
        f32 angle = (2.0f * PI * i) / slices;
        Vec2 point = {
            p1.x + (rr1.x * cosf(angle)),
            p1.y + (rr1.y * sinf(angle))
        };
        if (isInsideEllipse(point, p2, rr2)) {
            return true;
        }
    }
    // now for ellipse2
    for (uint i=0;  i < slices;  ++i) {
        f32 angle = (2.0f * PI * i) / slices;
        Vec2 point = {
            p2.x + (rr2.x * cosf(angle)),
            p2.y + (rr2.y * sinf(angle))
        };
        if (isInsideEllipse(point, p1, rr1)) {
            return true;
        }
    }

    return false;
}


LD_NOINLINE
void trait_move(Vec2& vel, f32 speed, bool diagonals=true)
{
    enum { LEFT, RIGHT, UP, DOWN };
    bool dir[4] = {
        IsKeyDown(KEY_LEFT),
        IsKeyDown(KEY_RIGHT),
        IsKeyDown(KEY_UP),
        IsKeyDown(KEY_DOWN)
    };

    if (diagonals) {
        f32 dx, dy, k;
        dx = dir[RIGHT] - dir[LEFT];
        dy = dir[DOWN] - dir[UP];
        if (dx != 0  &&  dy != 0) {
            k = (f32)inv_sqrt2;
        } else {
            k = 1.0f;
        }
        vel.x += speed * dx * k * G::dt;
        vel.y += speed * dy * k * G::dt;
    } else {
        if      (dir[LEFT])  vel.x -= speed * G::dt;
        else if (dir[RIGHT]) vel.x += speed * G::dt;
        else if (dir[UP])    vel.y -= speed * G::dt;
        else if (dir[DOWN])  vel.y += speed * G::dt;
    }
}


template <class Circle_t>
requires std::same_as<Circle_t, Circle> LD_NOINLINE
bool trait_rigidbody(Circle_t& c1, Circle_t& c2)
{
    f32 dx = c2.pos.x - c1.pos.x;
    f32 dy = c2.pos.y - c1.pos.y;
    f32 dist = std::sqrtf(dx*dx + dy*dy);
    f32 min_dist = c1.r + c2.r;

    // no overlap, return
    if (dist >= min_dist) return false;

    f32 nx, ny;
    if (dist < 0.0001f) {
        // centers are identical, pick random direction
        f32 angle = randf(0.0f, 2.0f*PI);
        nx = cosf(angle);
        ny = sinf(angle);
    }
    else {
        nx = dx / dist;
        ny = dy / dist;
    }

    f32 overlap = (min_dist - dist) * 0.5f;
    c1.pos.x -= nx * overlap;
    c1.pos.y -= ny * overlap;
    c2.pos.x += nx * overlap;
    c2.pos.y += ny * overlap;
    return true;
}


template <class Bound, class Trapped> requires
(std::same_as<Bound, Circle> || std::same_as<Bound, Rect>)
&& requires(Trapped t) {t.pos, t.r;}
void trait_bounding_area(Bound& _bound, Trapped& obj)
{

if constexpr (std::is_same_v<Bound, Circle>)
{
    Circle& bound = _bound;
    Vec2 delta = obj.pos - bound.pos;
    f32 dist = sqrtf(distance_2(obj.pos, bound.pos));
    f32 max_dist = bound.r - obj.r;
    if (dist >= max_dist) {
        f32 scale = (max_dist)/(dist);
        obj.pos.x = bound.pos.x + scale*delta.x;
        obj.pos.y = bound.pos.y + scale*delta.y;
    }
}

else if constexpr (std::is_same_v<Bound, Rect>)
{
    log "Unimplemented! SEE: " << __func__ << "\n";
}

}

