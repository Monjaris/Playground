#pragma once
#include "joystick.hpp"
#include "bullet.hpp"

struct Player
{
    static inline Player* active = nullptr;

    std::vector<Bullet> bullets;

    bool alive = true;

    Color color = COLOR3(250, 250, 250);
    Color color_active = COLOR3(1, 3, 7);

    static constexpr f32 radius_ini = 40.0f;
    f32 radius = radius_ini;

    Vec2 pos = {(f32)G::win_w/2, (f32)G::win_h/2};
    Vec2 vel = {0, 0};
    f32 speed = 320;

    Joystick joystick;

    std::optional<Animation> animation;

    // to store shared common bullet properties
    struct {
        Color color ={WHITE};
        Vec2 radius = {8, 15};
        f32 ini_speed = 50;
        f32 accel = 20;
    } bullet_template;

    struct {
        bool rigid = true;
    } traits;


    void start() {
        joystick.start();
        for (auto& b : bullets) b.start();
    }

    void reload() {
        joystick.reload();
        for (auto& b : bullets) b.reload();
    }

    void input_active() {
        trait_move(vel, speed);

        if (isKeyPressing(KEY_X)) {
            log "Removing player!\n";
            alive = false;
        }

        joystick.input();
    }

    void input() {
        if (isMouseInside(pos, radius)) {
            if (isMousePressed()) {
                if (active != this) {
                    active = this;
                    animation = Animation::make(radius, radius*1.25, 100);
                }
            }
        }

        if (active == this) {
            input_active();
        }
    }

    void loop() {
        pos.x += vel.x;
        pos.y += vel.y;
        vel = {0, 0};

        pos.x = std::clamp(pos.x, radius, (f32)G::win_w - radius);
        pos.y = std::clamp(pos.y, radius, (f32)G::win_h - radius);

        if (animation.has_value()) {
            if (!animation->done()) {
                radius = animation->value();
            } else {
                animation.reset();
                radius = radius_ini;
            }
        }

        joystick.loop();

        if (joystick.on_fire()) {
            log "Bullet shot! " << vepr(pos) << "\n";
            Bullet bullet = bullet
                .setPosition(pos)
                .setRadius(bullet_template.radius)
                .setColor(bullet_template.color)
                .setInitialSpeed(bullet_template.ini_speed)
                .setDirectionAngle(joystick.angle())
                .setAccelaration(bullet_template.accel)
            ;
            bullets.push_back((bullet.start(), bullet));
        }

        for (auto& b : bullets) b.loop();
    }

    void render_active() {
        // bg circle
        DrawCircle(
            pos.x, pos.y, radius*1.1, color
        );
        // fg circle
        DrawCircle(
            pos.x, pos.y, radius, color_active
        );

        joystick.render();
    }

    void render() {
        DrawCircle(
            pos.x, pos.y, radius, color
        );

        if (active == this) {
            render_active();
        }

        for (auto& b : bullets) b.render();
    }

    void quit() {
        joystick.quit();
        for (auto& b : bullets) b.quit();
    }
};
