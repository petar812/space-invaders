#pragma once

#include <cstddef>
#include <cstdint>

class Sprite {

  private:
    std::size_t width;
    std::size_t height;
    std::uint8_t *data;

  public:
    Sprite(std::size_t width, std::size_t height, std::uint8_t *data);

    std::size_t getWidth() const;
    std::size_t getHeight() const;
    std::uint8_t *getData() const;
};

class SpriteAnimation {

  private:
    bool loop;
    std::size_t num_frames;
    std::size_t frame_duration;
    std::size_t time;
    Sprite **frames;

  public:
    SpriteAnimation(bool loop, std::size_t num_frames, std::size_t frame_duration, Sprite **frames);

    const Sprite &getCurrentFrame() const;
    void update();
};
