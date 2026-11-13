#include "game.hpp"
#include <cstddef>
#include <vector>

Game::Game()
    : width(640)
    , height(480)
    , score(0) {

    // Initialize aliens
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

    bullets.emplace_back(player.getX() + 15, player.getY() + 21, 2);
}

void Game::updateBullets() {

    for (auto it = bullets.begin(); it != bullets.end();) {

        it->move();

        if (it->getY() >= height) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

std::size_t Game::getScore() const {
    return score;
}

void Game::checkCollisions(
    const Sprite &bulletSprite,
    const SpriteAnimation alienAnimations[3],
    const Sprite &alienDeathSprite) {

    constexpr std::size_t SCALE = 3;

    for (auto bullet = bullets.begin(); bullet != bullets.end();) {

        bool hit = false;

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

        if (hit) {
            bullet = bullets.erase(bullet);
        } else {
            ++bullet;
        }
    }
}
