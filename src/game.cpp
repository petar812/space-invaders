#include "game.hpp"
#include "alien.hpp"
#include "bullet.hpp"
#include "gameState.hpp"
#include "sprite.hpp"

#include <cstddef>
#include <random>
#include <vector>

Game::Game()
    : width(640)
    , height(480)
    , score(0)
    , alienDirection(1)
    , alienSpeed(0.5)
    , alienMovementAccumulator(0.0)
    , gen(rd())
    , state(GameState::MENU) {

    initializeAliens();
}

std::size_t Game::getWidth() const {
    return width;
}

std::size_t Game::getHeight() const {
    return height;
}

Player &Game::getPlayer() {
    return player;
}

std::vector<Alien> &Game::getAliens() {
    return aliens;
}

std::vector<Bullet> &Game::getBullets() {
    return bullets;
}

void Game::addBullet() {
    if (bullets.size() >= MAX_BULLETS) {

        return;
    }

    bullets.emplace_back(player.getX() + 15, player.getY() + 21, 2, PLAYER_BULLET);
}

void Game::updateBullets() {
    for (auto it = bullets.begin(); it != bullets.end();) {

        it->update();

        if (it->getDir() > 0 && it->getY() >= height || it->getDir() < 0 && it->getY() <= 2) {

            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::updateEnemyBullet() {

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = currentTime - lastEnemyShot;

    if (elapsed >= std::chrono::seconds(2)) {

        addEnemyBullet();

        lastEnemyShot = currentTime;
    }
}

std::size_t Game::getScore() const {
    return score;
}

void Game::checkCollisions(
    const Sprite &bulletSprite,
    const SpriteAnimation alienAnimations[3],
    const Sprite &alienDeathSprite,
    const Sprite &playerSprite) {
    constexpr std::size_t SCALE = 3;

    for (auto bullet = bullets.begin(); bullet != bullets.end();) {

        bool hit = false;

        if (bullet->getType() == PLAYER_BULLET) {

            for (Alien &alien : aliens) {

                if (alien.getType() == ALIEN_DEAD) {
                    continue;
                }

                const Sprite &alienSprite = alienAnimations[alien.getType() - 1].getCurrentFrame();

                bool overlap = bullet->getX() < alien.getX() + alienSprite.getWidth() * SCALE &&
                               bullet->getX() + bulletSprite.getWidth() * SCALE > alien.getX() &&
                               bullet->getY() < alien.getY() + alienSprite.getHeight() * SCALE &&
                               bullet->getY() + bulletSprite.getHeight() * SCALE > alien.getY();

                if (overlap) {

                    score += 10 * (4 - alien.getType());

                    alien.kill();

                    std::size_t offset =
                        (alienDeathSprite.getWidth() - alienSprite.getWidth()) * SCALE / 2;

                    alien.setX(alien.getX() - offset);

                    hit = true;
                    break;
                }
            }
        } else if (bullet->getType() == ENEMY_BULLET) {

            bool overlap = bullet->getX() < player.getX() + playerSprite.getWidth() * SCALE &&
                           bullet->getX() + bulletSprite.getWidth() * SCALE > player.getX() &&
                           bullet->getY() < player.getY() + playerSprite.getHeight() * SCALE &&
                           bullet->getY() + bulletSprite.getHeight() * SCALE > player.getY();

            if (overlap) {

                player.loseLife();

                hit = true;
            }
        }

        if (hit) {
            bullet = bullets.erase(bullet);
        } else {
            ++bullet;
        }
    }
}

bool Game::updateAliens() {
    constexpr std::size_t ALIEN_WIDTH = 36;
    constexpr std::size_t DROP_DISTANCE = 12;

    std::size_t aliveAliens = countAliveAliens();
    alienSpeed = 0.5 + (55.0 - aliveAliens) * 0.04;

    alienMovementAccumulator += alienSpeed;
    std::size_t movementStep = static_cast<std::size_t>(alienMovementAccumulator);
    alienMovementAccumulator -= movementStep;

    bool reachEdge = false;
    bool reachPlayer = false;

    // Check Screen Boundaries
    for (const Alien &alien : aliens) {

        if (alien.getType() == ALIEN_DEAD) {

            continue;
        }

        if (alienDirection > 0 && alien.getX() + ALIEN_WIDTH + movementStep >= width) {

            reachEdge = true;
        }

        if (alienDirection < 0 && alien.getX() <= movementStep) {

            reachEdge = true;
        }
    }

    // Check Direction And Move Down
    if (reachEdge) {

        alienDirection *= -1;

        for (Alien &alien : aliens) {

            if (alien.getType() == ALIEN_DEAD) {

                continue;
            }

            alien.setY(alien.getY() - DROP_DISTANCE);
        }
    }

    // Move Aliens Horizontally
    for (Alien &alien : aliens) {

        if (alien.getType() == ALIEN_DEAD) {

            continue;
        }

        if (alienDirection > 0) {

            alien.setX(alien.getX() + movementStep);
        } else {
            alien.setX(alien.getX() - movementStep);
        }
    }

    // Check Player Collision
    for (Alien &alien : aliens) {
        if (alien.getType() == ALIEN_DEAD) {

            continue;
        }
        if (alien.getY() <= player.getY() + 21 && alien.getX() <= player.getX() + 33 &&
            alien.getX() + alien.getWidth() * 3 > player.getX()) {

            return true;
        } else if (alien.getY() + alien.getHeight() * 3 < 36) {

            return true;
        }
    }
    return false;
}

std::size_t Game::countAliveAliens() const {

    std::size_t count = 0;

    for (const Alien &alien : aliens) {

        if (alien.getType() != ALIEN_DEAD) {
            count++;
        }
    }

    return count;
}

void Game::addEnemyBullet() {
    std::vector<Alien *> aliveAliens;

    for (Alien &alien : aliens) {

        if (alien.getType() != ALIEN_DEAD) {

            aliveAliens.push_back(&alien);
        }
    }

    if (aliveAliens.empty()) {

        return;
    }
    std::uniform_int_distribution<int> random(0, aliveAliens.size() - 1);

    int alienToShoot = random(gen);

    bullets.emplace_back(
        aliveAliens[alienToShoot]->getX() + 18,
        aliveAliens[alienToShoot]->getY() - 24,
        -2,
        ENEMY_BULLET);
}

void Game::update() {

    updateBullets();
    updateEnemyBullet();
}

void Game::reset() {

    score = 0;
    alienDirection = 1;
    alienSpeed = 0.5;
    alienMovementAccumulator = 0.0;
    initializeAliens();
    bullets.clear();
    player.resetPlayer();
}
GameState Game::getState() const {
    return state;
}

void Game::setState(GameState newState) {
    state = newState;
}

void Game::initializeAliens() {

    aliens.clear();

    for (std::size_t yi = 0; yi < 5; ++yi) {
        for (std::size_t xi = 0; xi < 11; ++xi) {

            std::size_t x = 36 * xi + 122;
            std::size_t y = 29 * yi + 260;

            std::uint8_t type;

            if (yi == 4) {
                type = ALIEN_TYPE_A;
            } else if (yi >= 2) {
                type = ALIEN_TYPE_B;
            } else {
                type = ALIEN_TYPE_C;
            }

            aliens.emplace_back(x, y, type);
        }
    }
}
