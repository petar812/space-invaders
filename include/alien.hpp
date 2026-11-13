
#pragma once

#include <cstddef>
#include <cstdint>

enum AlienType : std::uint8_t {
    ALIEN_DEAD = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};

class Alien {
  private:
    std::size_t x;
    std::size_t y;
    std::uint8_t type;
    std::size_t deathTimer;

  public:
    Alien(std::size_t x, std::size_t y, std::uint8_t type);

    std::size_t getX() const;
    std::size_t getY() const;
    std::uint8_t getType() const;

    void kill();
    void setX(std::size_t new_x);

    std::size_t getDeathTimer() const;
    void updateDeathTimer();
};
