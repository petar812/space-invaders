
#include "font.hpp"

void Font::setGlyph(std::size_t index, const std::uint8_t rows[GLYPH_HEIGHT]) {
    for (std::size_t y = 0; y < GLYPH_HEIGHT; ++y) {
        for (std::size_t x = 0; x < GLYPH_WIDTH; ++x) {
            data[index * GLYPH_WIDTH * GLYPH_HEIGHT + y * GLYPH_WIDTH + x] =
                (rows[y] >> (4 - x)) & 1;
        }
    }
}

Sprite Font::getGlyph(char character) const {
    if (character < 32 || character >= 32 + GLYPH_COUNT) {
        character = ' ';
    }

    std::size_t index = character - 32;
    std::size_t offset = index * GLYPH_WIDTH * GLYPH_HEIGHT;

    return Sprite(GLYPH_WIDTH, GLYPH_HEIGHT, const_cast<std::uint8_t *>(data + offset));
}

Font::Font() {
    // A-F
    const std::uint8_t glyph_A[7] = {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_B[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110};

    const std::uint8_t glyph_C[7] = {0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111};

    const std::uint8_t glyph_D[7] = {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110};

    const std::uint8_t glyph_E[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111};

    const std::uint8_t glyph_F[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000};

    setGlyph('A' - 32, glyph_A);
    setGlyph('B' - 32, glyph_B);
    setGlyph('C' - 32, glyph_C);
    setGlyph('D' - 32, glyph_D);
    setGlyph('E' - 32, glyph_E);
    setGlyph('F' - 32, glyph_F);

    // G-L
    const std::uint8_t glyph_G[7] = {0b01111, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111};

    const std::uint8_t glyph_H[7] = {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_I[7] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111};

    const std::uint8_t glyph_J[7] = {0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100};

    const std::uint8_t glyph_K[7] = {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001};

    const std::uint8_t glyph_L[7] = {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111};

    setGlyph('G' - 32, glyph_G);
    setGlyph('H' - 32, glyph_H);
    setGlyph('I' - 32, glyph_I);
    setGlyph('J' - 32, glyph_J);
    setGlyph('K' - 32, glyph_K);
    setGlyph('L' - 32, glyph_L);

    // M-R
    const std::uint8_t glyph_M[7] = {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_N[7] = {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_O[7] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_P[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000};

    const std::uint8_t glyph_Q[7] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101};

    const std::uint8_t glyph_R[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001};

    setGlyph('M' - 32, glyph_M);
    setGlyph('N' - 32, glyph_N);
    setGlyph('O' - 32, glyph_O);
    setGlyph('P' - 32, glyph_P);
    setGlyph('Q' - 32, glyph_Q);
    setGlyph('R' - 32, glyph_R);

    // S-Z
    const std::uint8_t glyph_S[7] = {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110};

    const std::uint8_t glyph_T[7] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};

    const std::uint8_t glyph_U[7] = {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_V[7] = {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100};

    const std::uint8_t glyph_W[7] = {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010};

    const std::uint8_t glyph_X[7] = {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001};

    const std::uint8_t glyph_Y[7] = {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100};

    const std::uint8_t glyph_Z[7] = {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111};

    setGlyph('S' - 32, glyph_S);
    setGlyph('T' - 32, glyph_T);
    setGlyph('U' - 32, glyph_U);
    setGlyph('V' - 32, glyph_V);
    setGlyph('W' - 32, glyph_W);
    setGlyph('X' - 32, glyph_X);
    setGlyph('Y' - 32, glyph_Y);
    setGlyph('Z' - 32, glyph_Z);

    // 0-9
    const std::uint8_t glyph_0[7] = {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110};

    const std::uint8_t glyph_1[7] = {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110};

    const std::uint8_t glyph_2[7] = {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111};

    const std::uint8_t glyph_3[7] = {0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110};

    const std::uint8_t glyph_4[7] = {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010};

    const std::uint8_t glyph_5[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110};

    const std::uint8_t glyph_6[7] = {0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_7[7] = {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000};

    const std::uint8_t glyph_8[7] = {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_9[7] = {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110};

    setGlyph('0' - 32, glyph_0);
    setGlyph('1' - 32, glyph_1);
    setGlyph('2' - 32, glyph_2);
    setGlyph('3' - 32, glyph_3);
    setGlyph('4' - 32, glyph_4);
    setGlyph('5' - 32, glyph_5);
    setGlyph('6' - 32, glyph_6);
    setGlyph('7' - 32, glyph_7);
    setGlyph('8' - 32, glyph_8);
    setGlyph('9' - 32, glyph_9);
}
