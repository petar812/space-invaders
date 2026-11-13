#pragma once

#include "alien.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "sprite.hpp"

#include <cstddef>
#include <source_location>
#include <vector>

class Game {
  private:
    static constexpr std::size_t MAX_BULLETS = 128;

    std::size_t width;
    std::size_t height;

    Player player;
    std::vector<Alien> aliens;
    std::vector<Bullet> bullets;

    std::size_t score;

  public:
    Game();

    std::size_t getWidth() const;
    std::size_t getHeight() const;

    Player &getPlayer();
    std::vector<Alien> &getAliens();
    std::vector<Bullet> &getBullets();

    void addBullet();
    void updateBullets();

    std::size_t getScore() const;
    void checkCollisions(
        const Sprite &bulletSprite,
        const SpriteAnimation alienAnimations[3],
        const Sprite &alienDeathSprite);
};
