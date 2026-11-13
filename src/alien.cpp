
#include "alien.hpp"

Alien::Alien(std::size_t x, std::size_t y, std::uint8_t type)
    : x(x)
    , y(y)
    , type(type)
    , deathTimer(0) {}

std::size_t Alien::getX() const {
    return x;
}

std::size_t Alien::getY() const {
    return y;
}

std::uint8_t Alien::getType() const {
    return type;
}

void Alien::kill() {
    type = ALIEN_DEAD;
    deathTimer = 10;
}

void Alien::setX(std::size_t new_x) {
    x = new_x;
}

std::size_t Alien::getDeathTimer() const {
    return deathTimer;
}

void Alien::updateDeathTimer() {
    if (deathTimer > 0) {
        --deathTimer;
    }
}
