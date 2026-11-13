
#include "player.hpp"

Player::Player()
    : x(303)
    , y(40)
    , life(3) {}

std::size_t Player::getX() const {
    return x;
}

std::size_t Player::getY() const {
    return y;
}

std::size_t Player::getLife() const {
    return life;
}

void Player::move(int direction, std::size_t game_width, std::size_t player_width) {

    int new_x = static_cast<int>(x) + direction;

    if (new_x < 0) {
        new_x = 0;
    }

    if (new_x + static_cast<int>(player_width) > static_cast<int>(game_width)) {

        new_x = static_cast<int>(game_width - player_width);
    }

    x = static_cast<std::size_t>(new_x);
}
