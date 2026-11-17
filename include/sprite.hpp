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
    double frame_duration;
    double timer;
    Sprite **frames;
    std::size_t current_frame;
    std::size_t getCurrent_frame;

  public:
    SpriteAnimation(bool loop, std::size_t num_frames, double frame_duration, Sprite **frames);

    const Sprite &getCurrentFrame() const;
    void update(double deltaTime);
};
