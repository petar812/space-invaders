#pragma once

#include <cstddef>
#include <iterator>

class Player {
  private:
    std::size_t x;
    std::size_t y;
    std::size_t life;

  public:
    Player();

    std::size_t getX() const;
    std::size_t getY() const;
    std::size_t getLife() const;

    void move(int direction, std::size_t game_width, std::size_t player_width);
};
