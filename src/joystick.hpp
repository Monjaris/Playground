#pragma once
#include "core.h"

struct Joystick
{
    Vec2 base_pos = {G::win_w * 0.825f, G::win_h * 0.625f};
    Vec2 outer_pos = base_pos;
    Vec2 pos = base_pos;

    f32 radius = G::win_w/29;
    f32 outer_rad_k = 1.9;
    bool bound_enabled = true;

    Color color = {45, 78, 255, 210};
    Color aiming_color = {45, 78, 255, 210};
    Color outer_color = {45, 78, 255, 210};

    bool pressed_on = false;
    bool aiming = false;
    bool fired = false;
    bool fire_frame = false;

    f32 area_r() {
        return radius * outer_rad_k;
    }
    f32 angle(bool radians=true) {
        Vec2 delta = pos - outer_pos;
        return (radians? (1.0f):(RAD2DEG)) * atan2f(delta.y, delta.x);
    }
    bool on_fire() {
        return fire_frame;
    }

    void start() {}
    void reload() {}

    void input() {
        if (!pressed_on && isMousePressed() && isMouseInside(outer_pos, area_r())) {
            pressed_on = true;
            outer_pos = pos = GetMousePosition(); // snap on initial press
        }

        aiming = pressed_on && isMouseDown();

        if (isMouseReleased()) {
            fired = true;
            pressed_on = false;
            aiming = false;
        }
    }

    void loop() {
        fire_frame = false;

        if (aiming) {
            pos = GetMousePosition();
        }
        if (fired) {
            log "fired! (" << angle(false) << ")\n";
            pos = outer_pos = base_pos;
            fire_frame = true;
            fired = false;
        }

        if (bound_enabled) {
            Circle bound = {outer_pos, area_r()+radius};
            Circle trapped = {pos, radius};
            trait_bounding_area(bound, trapped);
            outer_pos = bound.pos;
            pos = trapped.pos;
        }
    }

    void render() {
        if (aiming) {
            DrawCircle(pos.x, pos.y, radius, aiming_color);
        } else {
            DrawCircle(pos.x, pos.y, radius, color);
        }

        DrawCircleLinesV(outer_pos, area_r(), outer_color);
    }

    void quit() {}
};
