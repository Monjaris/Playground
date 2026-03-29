#include "game.hpp"

void Game::start()
{
    log "\n\n\nStarting game!\n";
    players.reserve(1024);
    for (auto& p : players) p.start();
}

void Game::input()
{
    if (isKeyPressing(KEY_F)) {
        log "FPS: " <<  G::fps << "\n";
    }

    if (isKeyPressing(KEY_N)) {
        log "Creating new player!\n";

        Player player = {};
        players.emplace_back(player);
        Player::active = &players.back();
    }

    if (IsKeyPressed(KEY_C)) {
        log "Player count: " << players.size() << "\n";
    }

    for (auto& p : players) p.input();
}

void Game::loop()
{
    for (auto& p : players) {
        p.loop();
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
        for (usize j=i+1;  j < p_count;  ++j) {
            auto& p = players[i];
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
