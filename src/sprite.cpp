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
    bool loop, std::size_t num_frames, std::size_t frame_duration, Sprite **frames)
    : loop(loop)
    , num_frames(num_frames)
    , frame_duration(frame_duration)
    , time(0)
    , frames(frames) {}

const Sprite &SpriteAnimation::getCurrentFrame() const {

    std::size_t current_frame = time / frame_duration;
    return *frames[current_frame];
}

void SpriteAnimation::update() {

    ++time;

    if (time == num_frames * frame_duration) {

        if (loop) {

            time = 0;
        }
    }
}
