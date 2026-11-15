
#include "bullet.hpp"

Bullet::Bullet(std::size_t x, std::size_t y, int dir, BulletType type)
    : x(x)
    , y(y)
    , dir(dir)
    , type(type) {}

std::size_t Bullet::getX() const {
    return x;
}

std::size_t Bullet::getY() const {
    return y;
}

int Bullet::getDir() const {
    return dir;
}

BulletType Bullet::getType() const {
    return type;
}

void Bullet::update() {
    y += dir;
}
