#pragma once
#include "player.hpp"

struct Game
{
    std::vector<Player> players;
    std::optional<Animation> animation;

    void activate_next_player(const uint32 current_idx) {
        if (players.size() <= 1ul) {
            Player::active = players.data();
        }
        else if (current_idx + 1 < players.size()) {
            Player::active = &players[current_idx+1];
        }
        else {
            Player::active = players.data();  // first elem(faster access)
        }
    }

    void start();
    void reload();
    void input();
    void loop();
    void render();
    void quit();
};
