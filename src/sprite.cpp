#include "sprite.hpp"
#include <cstddef>
#include <cstdint>
#include <iterator>

Sprite::Sprite(std::size_t width, std::size_t height, std::uint8_t *data)
    : width(width)
    , height(height)
    , data(data) {}

std::size_t Sprite::getWidth() const {

    return width;
}

std::size_t Sprite::getHeight() const {

    return height;
}

std::uint8_t *Sprite::getData() const {

    return data;
}

SpriteAnimation::SpriteAnimation(
    bool loop, std::size_t num_frames, double frame_duration, Sprite **frames)
    : loop(loop)
    , num_frames(num_frames)
    , frame_duration(frame_duration)
    , timer(0.0)
    , frames(frames)
    , current_frame(0) {}

const Sprite &SpriteAnimation::getCurrentFrame() const {

    return *frames[current_frame];
}

void SpriteAnimation::update(double deltaTime) {

    timer += deltaTime;

    while (timer >= frame_duration) {

        if (!loop && current_frame == num_frames - 1) {

            return;
        }

        if (loop) {
            if (current_frame < num_frames - 1) {
                current_frame++;
            } else {
                current_frame = 0;
            }
            timer -= frame_duration;
        } else {
            current_frame = num_frames - 1;
            timer -= frame_duration;
        }
    }
}
