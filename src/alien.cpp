
#include "alien.hpp"
#include <cstddef>
#include <system_error>

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

std::size_t Alien::getHeight() const {
    return 8;
}

std::size_t Alien::getWidth() const {

    if (type == ALIEN_TYPE_A) {
        return 8;
    } else if (type == ALIEN_TYPE_B) {
        return 11;
    } else if (type == ALIEN_TYPE_C) {
        return 12;
    }

    return 0;
}

void Alien::kill() {
    type = ALIEN_DEAD;
    deathTimer = 10;
}

void Alien::setX(std::size_t new_x) {
    x = new_x;
}

void Alien::setY(std::size_t new_y) {
    y = new_y;
}

std::size_t Alien::getDeathTimer() const {
    return deathTimer;
}

void Alien::updateDeathTimer() {
    if (deathTimer > 0) {
        --deathTimer;
    }
}
