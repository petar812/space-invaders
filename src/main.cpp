
#include "font.hpp"
#include "game.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "sprite.hpp"

#include <cstdint>
#include <cstdlib>

int main() {

    Game game;
    Renderer renderer(640, 480);
    Input input;
    Font font;

    // Player Sprite -------------------------------------------------------
    std::uint8_t playerData[] = {
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, // .....@.....
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, // ....@@@....
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, // ....@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
    };

    // Bullet Sprite ---------------------------------------------------------
    std::uint8_t bulletData[] = {1, 1, 1};

    Sprite bulletSprite(1, 3, bulletData);

    // Alien Sprites ------------------------------------------------------------

    Sprite playerSprite(11, 7, playerData);

    // Alien A - Frame 1
    std::uint8_t alienAFrame1[64] = {
        0, 0, 0, 1, 1, 0, 0, 0, // ...@@...
        0, 0, 1, 1, 1, 1, 0, 0, // ..@@@@..
        0, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@.
        1, 1, 0, 1, 1, 0, 1, 1, // @@.@@.@@
        1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@
        0, 1, 0, 1, 1, 0, 1, 0, // .@.@@.@.
        1, 0, 0, 0, 0, 0, 0, 1, // @......@
        0, 1, 0, 0, 0, 0, 1, 0  // .@....@.
    };

    // Alien A - Frame 2
    std::uint8_t alienAFrame2[64] = {
        0, 0, 0, 1, 1, 0, 0, 0, // ...@@...
        0, 0, 1, 1, 1, 1, 0, 0, // ..@@@@..
        0, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@.
        1, 1, 0, 1, 1, 0, 1, 1, // @@.@@.@@
        1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@
        0, 0, 1, 0, 0, 1, 0, 0, // ..@..@..
        0, 1, 0, 1, 1, 0, 1, 0, // .@.@@.@.
        1, 0, 1, 0, 0, 1, 0, 1  // @.@..@.@
    };

    // Alien B - Frame 1
    std::uint8_t alienBFrame1[88] = {
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, // ...@...@...
        0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, // ..@@@@@@@..
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, // .@@.@@@.@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, // @.@@@@@@@.@
        1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, // @.@.....@.@
        0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0  // ...@@.@@...
    };

    // Alien B - Frame 2
    std::uint8_t alienBFrame2[88] = {
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, // @..@...@..@
        1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, // @.@@@@@@@.@
        1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, // @@@.@@@.@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@.
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, // ..@.....@..
        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0  // .@.......@.
    };

    // Alien C - Frame 1
    std::uint8_t alienCFrame1[96] = {
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, // ....@@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, // @@@..@@..@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, // ...@@..@@...
        0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, // ..@@.@@.@@..
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1  // @@........@@
    };

    // Alien C - Frame 2
    std::uint8_t alienCFrame2[96] = {
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, // ....@@@@....
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // .@@@@@@@@@@.
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, // @@@..@@..@@@
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // @@@@@@@@@@@@
        0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, // ..@@@..@@@..
        0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, // .@@..@@..@@.
        0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0  // ..@@....@@..
    };

    Sprite alienSprites[6] = {
        Sprite(8, 8, alienAFrame1),
        Sprite(8, 8, alienAFrame2),
        Sprite(11, 8, alienBFrame1),
        Sprite(11, 8, alienBFrame2),
        Sprite(12, 8, alienCFrame1),
        Sprite(12, 8, alienCFrame2)};

    // Alien Death Sprite
    std::uint8_t alienDeathData[91] = {
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, // .@..@...@..@.
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, // ..@..@.@..@..
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, // ...@.....@...
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, // @@.........@@
        0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, // ...@.....@...
        0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, // ..@..@.@..@..
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0  // .@..@...@..@.
    };

    Sprite alienDeathSprite(13, 7, alienDeathData);

    Sprite *alienAFrames[] = {&alienSprites[0], &alienSprites[1]};

    Sprite *alienBFrames[] = {&alienSprites[2], &alienSprites[3]};

    Sprite *alienCFrames[] = {&alienSprites[4], &alienSprites[5]};

    SpriteAnimation alienAnimations[3] = {
        SpriteAnimation(true, 2, 10, alienAFrames),
        SpriteAnimation(true, 2, 10, alienBFrames),
        SpriteAnimation(true, 2, 10, alienCFrames)};

    if (!renderer.initialize()) {
        return EXIT_FAILURE;
    }

    input.initialize(renderer.getWindow());

    // Game Loop ------------------------------------------------
    while (!renderer.shouldClose() && input.isGameRunning()) {

        renderer.pollEvents();

        int playerMoveDirection = 0;

        if (input.isMovingRight()) {
            playerMoveDirection += 1;
        }

        if (input.isMovingLeft()) {
            playerMoveDirection -= 1;
        }

        game.getPlayer().move(playerMoveDirection, game.getWidth(), playerSprite.getWidth() * 3);

        if (input.isFirePressed()) {
            game.addBullet();
            input.resetFire();
        }

        renderer.clear(Renderer::rgbToUint32(0, 0, 0));

        std::uint32_t uiColor = Renderer::rgbToUint32(0, 255, 0);

        renderer.drawText(font, "SCORE", 30, 440, uiColor);
        renderer.drawNumber(font, game.getScore(), 40 + 6 * 5 * 3, 440, uiColor);

        renderer.drawText(font, "CREDIT 99", 450, 8, uiColor);

        renderer.drawLine(36, uiColor);

        renderer.drawSprite(
            playerSprite,
            game.getPlayer().getX(),
            game.getPlayer().getY(),
            Renderer::rgbToUint32(128, 0, 0),
            3);

        for (const Bullet &bullet : game.getBullets()) {

            renderer.drawSprite(
                bulletSprite,
                bullet.getX(),
                bullet.getY(),
                Renderer::rgbToUint32(128, 0, 0),
                3);
        }

        game.updateBullets();

        game.checkCollisions(bulletSprite, alienAnimations, alienDeathSprite);

        for (const Alien &alien : game.getAliens()) {
            if (alien.getType() == ALIEN_DEAD) {
                if (alien.getDeathTimer() == 0) {
                    continue;
                }

                renderer.drawSprite(
                    alienDeathSprite,
                    alien.getX(),
                    alien.getY(),
                    Renderer::rgbToUint32(128, 0, 0),
                    3);

                continue;
            }

            const Sprite &sprite = alienAnimations[alien.getType() - 1].getCurrentFrame();

            renderer.drawSprite(
                sprite,
                alien.getX(),
                alien.getY(),
                Renderer::rgbToUint32(128, 0, 0),
                3);
        }

        for (Alien &alien : game.getAliens()) {
            alien.updateDeathTimer();
        }

        for (SpriteAnimation &animation : alienAnimations) {

            animation.update();
        }

        renderer.render();
        renderer.swapBuffers();
    }

    return EXIT_SUCCESS;
}
