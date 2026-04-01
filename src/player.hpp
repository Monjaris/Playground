#pragma once
#include "joystick.hpp"
#include "bullet.hpp"

struct Player
{
    static inline Player* active = nullptr;

    std::vector<Bullet> bullets;

    bool alive = true;

    static inline Color color_A = COLOR3(250, 250, 250);
    static inline Color color_active_A = COLOR3(150, 210, 150);
    static inline Color color_B = COLOR3(35, 55, 80);
    static inline Color color_active_B = COLOR3(1, 2, 5);

    static constexpr f32 radius_ini = 40.0f;
    f32 radius = radius_ini;

    Vec2 pos = {(f32)G::win_w/2, (f32)G::win_h/2};
    Vec2 vel = {0, 0};
    f32 speed = 270;

    Joystick joystick;

    std::optional<Animation> animation;

    // to store shared common bullet properties
    struct {
        Color color_A = COLOR3(220, 190, 25);
        Color color_B = COLOR3(165, 25, 220);
        f32 radius = 14;
        f32 ini_speed = 180;
        f32 accel = 120;
    } bullet_template;

    struct {
        bool rigid = true;
        const char* opt_WASD_RLUD_BOTH = "BOTH";
    } traits;

    enum class Team:int8_t{A=0, B=1} team;

    static bool friends(const Player& p1, const Player& p2) {
        return p1.team == p2.team;
    }

    void destroyBullet(uint32 index, bool animation=true) {
        if (index >= bullets.size()) {
            log "Couldn't destroy bullet: index is too high";
            return;
        }
        auto& bullet = bullets[index];
        if (animation && this->animation.has_value()) {
            this->animation->make(bullet.radius, bullet.radius*1.5, 300);
        }
        // bullets.erase(bullets.begin() + index);
        std::swap(bullet, bullets.back()); bullets.pop_back();
    }

    void start() {
        team = random_choice(Team::A, Team::B);
        joystick.start();
        for (auto& b : bullets) b.start();
    }

    void reload() {
        log "(Team "<<((team==Team::A)?"A":"B")<<" )++\n" << std::flush;

        joystick.reload();
        for (auto& b : bullets) b.reload();
    }

    void input_active() {
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
                    animation = Animation(radius, radius_ini*1.25, 100);
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

        for (uint i=0;  i < bullets.size();  ++i) {
            auto& blt = bullets[i];
            f32 wk = 0.2f;
            trait_handle_outside(blt,
                {0-G::win_w*wk, 0-G::win_h*wk}, {G::win_w*(1.0f+wk), G::win_h*(1.0f+wk)},
                [this, i](){bullets.erase(bullets.begin()+i);}
            );
        }


        /* JOYSTICKS */
        joystick.loop();

        if (joystick.on_fired()) {
            log "Bullet shot! " << vepr(pos) << "\n";
            auto bullet = Bullet{}
                .setPosition(pos)
                .setRadius(bullet_template.radius)
                .setColor(team==Team::A?bullet_template.color_A:bullet_template.color_B)
                .setInitialSpeed(bullet_template.ini_speed)
                .setDirectionAngle(joystick.angle(true))
                .setAccelaration(bullet_template.accel)
            ;
            bullets.push_back((bullet.start(), bullet));
        }


        /* BULLETS */
        for (uint i=0; i<bullets.size();  ++i) {
            auto& b = bullets[i];

            b.loop();
            
            if (b.hit_opponent) {
                b.setSpeed(0);
                static f32 delay = 100;
                b.animation = Animation(radius, radius_ini*1.5, delay);
                if (after(delay)) {
                    destroyBullet(i);
                }
            }
        }
    }

    void render_active() {
        if (team == Team::A) {
            DrawCircle(pos.x, pos.y, radius*1.1, color_A);
            DrawCircle(pos.x, pos.y, radius, color_active_A);
        }
        else if (team == Team::B) {
            DrawCircle(pos.x, pos.y, radius*1.1, color_B);
            DrawCircle(pos.x, pos.y, radius, color_active_B);
        }
        else {
            log "Unknown team type found!\n";
        }

        joystick.render();
    }

    void render() {
        if (active != this)
        {
            if (team == Team::A) {
                DrawCircle(pos.x, pos.y, radius, color_A);
            } else if (team == Team::B) {
                DrawCircle(pos.x, pos.y, radius, color_B);
            } else {
                log "Unknown team type found!\n";
            }
        }
        else {
            render_active();
        }

        for (auto& b : bullets) b.render();
    }

    void quit() {
        joystick.quit();
        for (auto& b : bullets) b.quit();
    }
};
