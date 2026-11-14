
#include "bullet.hpp"

Bullet::Bullet(std::size_t x, std::size_t y, int dir)
    : x(x)
    , y(y)
    , dir(dir) {}

std::size_t Bullet::getX() const {
    return x;
}

std::size_t Bullet::getY() const {
    return y;
}

int Bullet::getDir() const {
    return dir;
}

void Bullet::update() {
    y += dir;
}
