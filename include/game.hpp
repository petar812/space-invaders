#pragma once

#include "alien.hpp"
#include "bullet.hpp"
#include "gameState.hpp"
#include "player.hpp"
#include "sprite.hpp"

#include <chrono>
#include <cstddef>
#include <random>
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

    int alienDirection;
    double alienSpeed;
    double alienMovementAccumulator;

    std::size_t countAliveAliens() const;

    void addEnemyBullet();

    void updateBullets();
    void updateEnemyBullet();
    void updateAliens();

    std::random_device rd;
    std::mt19937 gen;

    std::chrono::steady_clock::time_point lastEnemyShot;

    GameState state;

  public:
    Game();

    std::size_t getWidth() const;
    std::size_t getHeight() const;

    Player &getPlayer();
    std::vector<Alien> &getAliens();
    std::vector<Alien *> getAliveAliens();
    std::vector<Bullet> &getBullets();

    void update();
    void reset();
    void addBullet();

    std::size_t getScore() const;
    void checkCollisions(
        const Sprite &bulletSprite,
        const SpriteAnimation alienAnimations[3],
        const Sprite &alienDeathSprite,
        const Sprite &playerSprite);

    GameState getState() const;
    void setState(GameState newState);

    void initializeAliens();
};
