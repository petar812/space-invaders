
#pragma once

#include "sprite.hpp"

#include <cstdint>

class Font {
  private:
    static constexpr std::size_t GLYPH_WIDTH = 5;
    static constexpr std::size_t GLYPH_HEIGHT = 7;
    static constexpr std::size_t GLYPH_COUNT = 65;

    std::uint8_t data[GLYPH_COUNT * GLYPH_WIDTH * GLYPH_HEIGHT]{};

    void setGlyph(std::size_t index, const std::uint8_t rows[GLYPH_HEIGHT]);

  public:
    Font();

    Sprite getGlyph(char character) const;
};
