
#pragma once

#include <cstddef>

enum BulletType {

    PLAYER_BULLET,
    ENEMY_BULLET
};

class Bullet {
  private:
    std::size_t x;
    std::size_t y;
    int dir;

    BulletType type;

  public:
    Bullet(std::size_t x, std::size_t y, int dir, BulletType type);

    std::size_t getX() const;
    std::size_t getY() const;
    int getDir() const;
    BulletType getType() const;

    void update();
};
