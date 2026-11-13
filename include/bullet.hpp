
#pragma once

#include <cstddef>

class Bullet {
  private:
    std::size_t x;
    std::size_t y;
    int dir;

  public:
    Bullet(std::size_t x, std::size_t y, int dir);

    std::size_t getX() const;
    std::size_t getY() const;

    void move();
};
