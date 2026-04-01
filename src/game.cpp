#include "game.hpp"

void Game::start()
{
    log "\n\n\nStarting game!\n";
    players.reserve(1024);
    for (auto& p : players) p.start();

    Parser conf;
    if (conf.init("../.config"));
    else {
        log "Parser: failed to open the configuration!\n";
    }
}

void Game::input()
{
    if (isKeyPressing(KEY_F)) {
        log "FPS: " <<  G::fps << "\n";
    }

    if (isKeyPressing(KEY_N)) {
        log "Creating new player!\n";

        Player player = {};
        players.push_back((player.start(), player));
        Player::active = &players.back();
    }

    if (IsKeyPressed(KEY_C)) {
        log "Player count: " << players.size() << "\n";
    }

    for (auto& p : players) p.input();
}

void Game::loop()
{

    {
        uint pcount = players.size();
        // for each player
        for (uint i=0; i<pcount; ++i) {
            auto& p = players[i];
            p.loop();

            // for each other player
            for (uint j=i+1; j<pcount;  ++j) {
                auto& otherp = players[j];

                // for each of their bullets
                for (uint k=0;  k<otherp.bullets.size(); ++k) {
                    auto& ob = otherp.bullets[k];
                    // check collision against p
                    if (CheckCollisionCircles(ob.pos, ob.radius, p.pos, p.radius)) {
                        if (!Player::friends(p, otherp)) {
                            log "[" << j << "] hit [" << i << "]\n";
                            ob.hit_opponent  = true;
                        }
                    }
                }
            }
        }
    }

    // erase player if it is not alive
    uint32 i = 0;
    while (i < players.size()) {
        auto& p = players[i];

        if (!p.alive) {
            bool was_active = (Player::active == &p);
            std::swap(p, players.back());
            players.pop_back();
            if (was_active) {
                activate_next_player(i);
            }
        } else {
            ++i;
        }
    }

    // apply traits
    const usize p_count = players.size();
    for (usize i=0;  i < p_count;  ++i) {
        auto& p = players[i];
        for (usize j=i+1;  j < p_count;  ++j) {
            auto& other = players[j];
            Circle circle = {p.pos, p.radius};
            Circle other_circle = {other.pos, other.radius};
            if (p.traits.rigid && (p_count>=2)) {
                // precheck distance then apply rigidbody logic(faster)
                f32 r_sum = p.radius + other.radius;
                if (distance_2(p.pos, other.pos) < (r_sum*r_sum)) {
                    trait_rigidbody(circle, other_circle);
                    p.pos = circle.pos;
                    p.radius = circle.r;
                    other.pos = other_circle.pos;
                    other.radius = other_circle.r;
                }
            }
        }
        if (strcmp(p.traits.opt_WASD_RLUD_BOTH, "") && Player::active==&p)
        {
            const char*& move_method = p.traits.opt_WASD_RLUD_BOTH;
            bool moved = false;
    
            if (!moved && (!strcmp(move_method, "WASD") || !strcmp(move_method, "BOTH"))) {
                moved = trait_move(p.vel, p.speed, {KEY_A, KEY_D, KEY_W, KEY_S});
            }
            if (!moved && (!strcmp(move_method, "RLUD") || !strcmp(move_method, "BOTH"))) {
                moved = trait_move(p.vel, p.speed, {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN});
            }
        }
        if (true) {
            trait_apply_animations(p.radius, p.animation, [&p]() {
                p.radius = p.radius_ini;
            });

            for (uint j=0; j<p.bullets.size(); ++j) {
                auto& b = p.bullets[j];

                trait_apply_animations(b.radius, p.animation);
            }
        }
    }
}

void Game::render()
{
    for (auto& p : players) p.render();
}

void Game::quit()
{
    for (auto& p : players) p.quit();
    log "\n\nQuitting game!\n\n\n";
}
