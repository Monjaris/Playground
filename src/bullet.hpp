#pragma once
#include "core.h"

struct Bullet
{
    Color color = Color{200, 150, 165, 210};
    Vec2 rad = {};

    Vec2 pos = {};
    Vec2 vel = {0, 0};
    f32 dir_angle = 0;

    static inline f32 ini_speed = 120;
    f32 speed = ini_speed;
    f32 accel = 40;

    Bullet& setInitialSpeed(f32 initial_speed) {ini_speed = initial_speed; return *this;}
    Bullet& setColor(Color color) {this->color = color; return *this;}
    Bullet& setRadius(Vec2 radius) {this->rad = radius; return *this;}
    Bullet& setPosition(Vec2 position) {pos = position; return *this;}
    Bullet& setVelocity(Vec2 velocity) {vel = velocity; return *this;}
    Bullet& setDirectionAngle(f32 degrees) {dir_angle  = degrees; return *this;}
    Bullet& setSpeed(f32 speed) {this->speed = speed; return *this;}
    Bullet& setAccelaration(f32 accelaration) {accel = accelaration; return *this;}

    f32 get_angle(bool radians=true) {
        return (radians)?
        (DEG2RAD*dir_angle):(dir_angle);
    }


    void start() {
        setSpeed(ini_speed);
    }

    void reload() {}

    void loop()
    {
        (*this)
            .setSpeed(speed + accel*G::dt)
            .setVelocity({
                speed*cosf(get_angle())*G::dt, speed*sinf(get_angle())*G::dt
            })
            .setPosition(pos + vel)
        ;
    }

    void render()
    {
        DrawEllipseV(
            pos, rad.x*cosf(get_angle()), rad.y*cosf(get_angle()), color
        );
    }

    void quit() {}

};
