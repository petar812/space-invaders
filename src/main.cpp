#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>

#define GAME_MAX_BULLETS 128

bool game_running = false;
bool move_left = false;
bool move_right = false;
bool fire_pressed = false;

const char *vertex_shader = "#version 330\n"
                            "noperspective out vec2 TexCoord;\n"
                            "void main(void){\n"
                            "    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
                            "    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
                            "    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
                            "}\n";

const char *fragment_shader = "#version 330\n"
                              "uniform sampler2D buffer;\n"
                              "noperspective in vec2 TexCoord;\n"
                              "out vec3 outColor;\n"
                              "void main(void){\n"
                              "    outColor = texture(buffer, TexCoord).rgb;\n"
                              "}\n";

struct Buffer {

    std::size_t width, height;
    std::uint32_t *data;
};

struct Sprite {

    std::size_t width, height;
    std::uint8_t *data;
};

struct SpriteAnimation {

    bool loop;
    std::size_t num_frames;
    std::size_t frame_duration;
    std::size_t time;
    Sprite **frames;
};

enum AlienType : std::uint8_t {
    ALIEN_DEAD = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};

struct Alien {

    std::size_t x, y;
    std::uint8_t type;
};

struct Player {

    std::size_t x, y;
    std::size_t life;
};

struct Bullet {

    std::size_t x, y;
    int dir;
};

struct Game {

    std::size_t width, height;
    std::size_t num_aliens;
    Alien *aliens;
    Player player;
    std::size_t num_bullets;
    Bullet bullets[GAME_MAX_BULLETS];
};

void error_callback(int error, const char *description) {

    // Error Handling
    std::fprintf(stderr, "Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            game_running = false;
        }
        break;

    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
        if (action == GLFW_PRESS) {
            move_right = true;
        } else if (action == GLFW_RELEASE) {
            move_right = false;
        }
        break;

    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
        if (action == GLFW_PRESS) {
            move_left = true;
        } else if (action == GLFW_RELEASE) {
            move_left = false;
        }
        break;

    case GLFW_KEY_UP:
    case GLFW_KEY_W:
        if (action == GLFW_PRESS) {
            fire_pressed = true;
        }
        break;

    default:
        break;
    }
}

std::uint32_t rgb_to_uint32(std::uint8_t r, std::uint8_t g, std::uint8_t b) {

    // Setting Colors
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

void buffer_clear(Buffer *buffer, std::uint32_t color) {

    for (std::size_t i = 0; i < buffer->width * buffer->height; i++) {
        buffer->data[i] = color;
    }
}

void buffer_draw_line(Buffer *buffer, std::size_t y, std::uint32_t color) {
    for (std::size_t x = 0; x < buffer->width; ++x) {
        buffer->data[y * buffer->width + x] = color;
    }
}

void validate_shader(GLuint shader, const char *file = 0) {

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {

        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::fprintf(stderr, "Shader compilation error: %s\n", log);
    }
}

void validate_program(GLuint program) {

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);

        std::fprintf(stderr, "Program linking error: %s\n", log);
    }
}

void buffer_sprite_draw(
    Buffer *buffer,
    const Sprite &sprite,
    std::size_t x,
    std::size_t y,
    std::uint32_t color,
    std::size_t scale) {

    for (std::size_t xi = 0; xi < sprite.width; ++xi) {
        for (std::size_t yi = 0; yi < sprite.height; ++yi) {

            if (!sprite.data[yi * sprite.width + xi])
                continue;

            // Jedes Sprite-Pixel wird zu einem scale x scale Block
            for (std::size_t dx = 0; dx < scale; ++dx) {
                for (std::size_t dy = 0; dy < scale; ++dy) {

                    std::size_t sx = x + xi * scale + dx;
                    std::size_t sy = y + (sprite.height - 1 - yi) * scale + dy;

                    if (sx < buffer->width && sy < buffer->height) {
                        buffer->data[sy * buffer->width + sx] = color;
                    }
                }
            }
        }
    }
}

void buffer_draw_text(
    Buffer *buffer,
    const Sprite &text_spritesheet,
    const char *text,
    std::size_t x,
    std::size_t y,
    uint32_t color) {

    std::size_t xp = x;
    std::size_t stride = text_spritesheet.width * text_spritesheet.height;
    Sprite sprite = text_spritesheet;

    for (const char *charp = text; *charp != '\0'; ++charp) {

        char character = *charp - 32;
        if (character < 0 || character >= 65) {

            continue;
        }

        sprite.data = text_spritesheet.data + character * stride;
        buffer_sprite_draw(buffer, sprite, xp, y, color, 3);
        xp += (sprite.width + 1) * 3;
    }
}

void buffer_draw_number(
    Buffer *buffer,
    const Sprite &font,
    std::size_t number,
    std::size_t x,
    std::size_t y,
    std::uint32_t color) {

    char text[32];
    std::snprintf(text, sizeof(text), "%zu", number);

    buffer_draw_text(buffer, font, text, x, y, color);
}

void game_init(Game *game) {
    // game initialization

    // borders
    game->width = 640;
    game->height = 480;

    // aliens number and positions
    game->num_aliens = 55;
    game->aliens = new Alien[game->num_aliens];

    for (std::size_t yi = 0; yi < 5; ++yi) {
        for (std::size_t xi = 0; xi < 11; ++xi) {

            game->aliens[yi * 11 + xi].x = 36 * xi + 122;
            game->aliens[yi * 11 + xi].y = 29 * yi + 260;

            if (yi == 4) {
                game->aliens[yi * 11 + xi].type = ALIEN_TYPE_A;

            } else if (yi >= 2) {
                game->aliens[yi * 11 + xi].type = ALIEN_TYPE_B;

            } else {
                game->aliens[yi * 11 + xi].type = ALIEN_TYPE_C;
            }
        }
    }
    // player position and lives
    game->player.x = 303;
    game->player.y = 38;
    game->player.life = 3;

    // number of bullets
    game->num_bullets = 0;
}

void game_add_bullet(Game *game) {

    if (game->num_bullets >= GAME_MAX_BULLETS) {

        return;
    }

    Bullet &bullet = game->bullets[game->num_bullets];

    bullet.x = game->player.x + 15;
    bullet.y = game->player.y + 21;
    bullet.dir = 2;

    ++game->num_bullets;
}

bool sprite_overlap_check(
    const Sprite &sp_a,
    std::size_t x_a,
    std::size_t y_a,
    const Sprite &sp_b,
    std::size_t x_b,
    std::size_t y_b,
    std::size_t scale) {

    if (x_a < x_b + sp_b.width * scale && x_a + sp_a.width * scale > x_b &&
        y_a < y_b + sp_b.height * scale && y_a + sp_a.height * scale > y_b) {

        return true;
    }

    return false;
}

void font_set_glyph(Sprite &font, std::size_t index, const std::uint8_t rows[7]) {

    for (std::size_t y = 0; y < 7; ++y) {
        for (std::size_t x = 0; x < 5; ++x) {

            font.data[index * 35 + y * 5 + x] = (rows[y] >> (4 - x)) & 1;
        }
    }
}

int main() {

    Buffer framebuffer;
    framebuffer.width = 640;
    framebuffer.height = 480;
    framebuffer.data = new std::uint32_t[framebuffer.width * framebuffer.height];

    // Hintergrund Farbe (schwarz)
    std::uint32_t clear_color = rgb_to_uint32(0, 0, 0);

    // Alien Sprites A, B, C ----------------------------------------------------
    Sprite alien_sprites[6];

    // Alien A
    alien_sprites[0].width = 8;
    alien_sprites[0].height = 8;
    alien_sprites[0].data =
        new std::uint8_t[64]{0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
                             1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
                             1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0};

    alien_sprites[1].width = 8;
    alien_sprites[1].height = 8;
    alien_sprites[1].data =
        new std::uint8_t[64]{0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
                             1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0,
                             0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1};

    // Alien B
    alien_sprites[2].width = 11;
    alien_sprites[2].height = 8;
    alien_sprites[2].data = new std::uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0};

    alien_sprites[3].width = 11;
    alien_sprites[3].height = 8;
    alien_sprites[3].data = new std::uint8_t[88]{
        0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};

    // Alien C
    alien_sprites[4].width = 12;
    alien_sprites[4].height = 8;
    alien_sprites[4].data = new std::uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};

#
    alien_sprites[5].width = 12;
    alien_sprites[5].height = 8;
    alien_sprites[5].data = new std::uint8_t[96]{
        0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
        0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0};

    // Alien Death Sprite
    Sprite alien_death_sprite;

    alien_death_sprite.width = 13;
    alien_death_sprite.height = 7;

    alien_death_sprite.data = new std::uint8_t[91]{
        0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0};

    // Alien Sprite Animation
    SpriteAnimation alien_animation[3];

    for (std::size_t i = 0; i < 3; ++i) {

        alien_animation[i].loop = true;
        alien_animation[i].num_frames = 2;
        alien_animation[i].frame_duration = 10;
        alien_animation[i].time = 0;

        alien_animation[i].frames = new Sprite *[2];

        alien_animation[i].frames[0] = &alien_sprites[i * 2];
        alien_animation[i].frames[1] = &alien_sprites[i * 2 + 1];
    }

    // Player Sprite -----------------------------------------------
    std::uint8_t player_data[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
                                  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                                  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    Sprite player_sprite;
    player_sprite.width = 11;
    player_sprite.height = 7;
    player_sprite.data = player_data;

    // Bullet Sprite ----------------------------------------------
    std::uint8_t bullet_data[] = {
        1,
        1,
        1,
    };

    Sprite bullet_sprite;
    bullet_sprite.width = 1;
    bullet_sprite.height = 3;
    bullet_sprite.data = bullet_data;

    // Font for the Score
    Sprite font_sprite;
    font_sprite.width = 5;
    font_sprite.height = 7;

    font_sprite.data = new uint8_t[65 * 5 * 7]{};

    // Font: A-F
    const std::uint8_t glyph_A[7] = {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_B[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110};

    const std::uint8_t glyph_C[7] = {0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111};

    const std::uint8_t glyph_D[7] = {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110};

    const std::uint8_t glyph_E[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111};

    const std::uint8_t glyph_F[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000};

    font_set_glyph(font_sprite, 'A' - 32, glyph_A);
    font_set_glyph(font_sprite, 'B' - 32, glyph_B);
    font_set_glyph(font_sprite, 'C' - 32, glyph_C);
    font_set_glyph(font_sprite, 'D' - 32, glyph_D);
    font_set_glyph(font_sprite, 'E' - 32, glyph_E);
    font_set_glyph(font_sprite, 'F' - 32, glyph_F);

    // Font: G-L

    const std::uint8_t glyph_G[7] = {0b01111, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111};

    const std::uint8_t glyph_H[7] = {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_I[7] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b11111};

    const std::uint8_t glyph_J[7] = {0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100};

    const std::uint8_t glyph_K[7] = {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001};

    const std::uint8_t glyph_L[7] = {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111};

    font_set_glyph(font_sprite, 'G' - 32, glyph_G);
    font_set_glyph(font_sprite, 'H' - 32, glyph_H);
    font_set_glyph(font_sprite, 'I' - 32, glyph_I);
    font_set_glyph(font_sprite, 'J' - 32, glyph_J);
    font_set_glyph(font_sprite, 'K' - 32, glyph_K);
    font_set_glyph(font_sprite, 'L' - 32, glyph_L);

    // Font: M-R

    const std::uint8_t glyph_M[7] = {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_N[7] = {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001};

    const std::uint8_t glyph_O[7] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_P[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000};

    const std::uint8_t glyph_Q[7] = {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101};

    const std::uint8_t glyph_R[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001};

    font_set_glyph(font_sprite, 'M' - 32, glyph_M);
    font_set_glyph(font_sprite, 'N' - 32, glyph_N);
    font_set_glyph(font_sprite, 'O' - 32, glyph_O);
    font_set_glyph(font_sprite, 'P' - 32, glyph_P);
    font_set_glyph(font_sprite, 'Q' - 32, glyph_Q);
    font_set_glyph(font_sprite, 'R' - 32, glyph_R);

    // Font S-Z

    const std::uint8_t glyph_S[7] = {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110};

    const std::uint8_t glyph_T[7] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};

    const std::uint8_t glyph_U[7] = {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110};

    const std::uint8_t glyph_V[7] = {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100};

    const std::uint8_t glyph_W[7] = {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010};

    const std::uint8_t glyph_X[7] = {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001};

    const std::uint8_t glyph_Y[7] = {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100};

    const std::uint8_t glyph_Z[7] = {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111};

    font_set_glyph(font_sprite, 'S' - 32, glyph_S);
    font_set_glyph(font_sprite, 'T' - 32, glyph_T);
    font_set_glyph(font_sprite, 'U' - 32, glyph_U);
    font_set_glyph(font_sprite, 'V' - 32, glyph_V);
    font_set_glyph(font_sprite, 'W' - 32, glyph_W);
    font_set_glyph(font_sprite, 'X' - 32, glyph_X);
    font_set_glyph(font_sprite, 'Y' - 32, glyph_Y);
    font_set_glyph(font_sprite, 'Z' - 32, glyph_Z);

    // Font 0-9

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

    font_set_glyph(font_sprite, '0' - 32, glyph_0);
    font_set_glyph(font_sprite, '1' - 32, glyph_1);
    font_set_glyph(font_sprite, '2' - 32, glyph_2);
    font_set_glyph(font_sprite, '3' - 32, glyph_3);
    font_set_glyph(font_sprite, '4' - 32, glyph_4);
    font_set_glyph(font_sprite, '5' - 32, glyph_5);
    font_set_glyph(font_sprite, '6' - 32, glyph_6);
    font_set_glyph(font_sprite, '7' - 32, glyph_7);
    font_set_glyph(font_sprite, '8' - 32, glyph_8);
    font_set_glyph(font_sprite, '9' - 32, glyph_9);

    // Initialize GLFW ----------------------------------------------
    glfwSetErrorCallback(error_callback);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit()) {
        std::fprintf(stderr, "Error: GLFW initialization failed!\n");
        return EXIT_FAILURE;
    }

    // Create Window ------------------------------------------------
    GLFWwindow *window;

    window = glfwCreateWindow(640, 480, "Space Invaders", nullptr, nullptr);

    if (window == nullptr) {

        std::fprintf(stderr, "Error: Window creation failed!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    // Initialize GLEW ----------------------------------------------
    GLenum glew_status = glewInit();

    if (glew_status != GLEW_OK) {
        std::fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Create Shader Program -------------------------------------
    GLuint fullscreen_triangle_vao;

    glGenVertexArrays(1, &fullscreen_triangle_vao);
    glBindVertexArray(fullscreen_triangle_vao);

    GLuint shader_id = glCreateProgram();

    // Vertex Shader ---------------------------------------------
    {
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader_vp, 1, &vertex_shader, nullptr);
        glCompileShader(shader_vp);

        validate_shader(shader_vp);

        glAttachShader(shader_id, shader_vp);
        glDeleteShader(shader_vp);
    }

    // Fragment Shader -------------------------------------------
    {
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader_fp, 1, &fragment_shader, nullptr);
        glCompileShader(shader_fp);

        validate_shader(shader_fp);

        glAttachShader(shader_id, shader_fp);
        glDeleteShader(shader_fp);
    }

    glLinkProgram(shader_id);

    validate_program(shader_id);

    glUseProgram(shader_id);

    GLuint buffer_texture;

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        framebuffer.width,
        framebuffer.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8,
        framebuffer.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);

    std::fprintf(stdout, "OpenGL-Version: %s\n", glGetString(GL_VERSION));

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreen_triangle_vao);

    Game game;
    game_init(&game);

    std::size_t score = 0;

    std::uint8_t *death_counters = new std::uint8_t[game.num_aliens];

    for (std::size_t i = 0; i < game.num_aliens; ++i) {

        death_counters[i] = 10;
    }

    int player_move_dir = 0;
    game_running = true;

    // Game Loop -----------------------------------------------

    while (!glfwWindowShouldClose(window) && game_running) {
        glfwPollEvents();

        buffer_clear(&framebuffer, clear_color);

        buffer_draw_text(&framebuffer, font_sprite, "SCORE", 20, 450, rgb_to_uint32(0, 255, 0));

        buffer_draw_number(&framebuffer, font_sprite, score, 120, 450, rgb_to_uint32(0, 255, 0));

        buffer_draw_text(&framebuffer, font_sprite, "CREDIT 99", 450, 8, rgb_to_uint32(0, 255, 0));

        buffer_draw_line(&framebuffer, 34, rgb_to_uint32(0, 255, 0));

        // Draw Aliens
        for (std::size_t ai = 0; ai < game.num_aliens; ++ai) {

            if (!death_counters[ai])
                continue;

            const Alien &alien = game.aliens[ai];

            if (alien.type == ALIEN_DEAD) {

                buffer_sprite_draw(
                    &framebuffer,
                    alien_death_sprite,
                    alien.x,
                    alien.y,
                    rgb_to_uint32(128, 0, 0),
                    3);

            } else {

                const SpriteAnimation &animation = alien_animation[alien.type - 1];

                std::size_t current_frame = animation.time / animation.frame_duration;

                const Sprite &sprite = *animation.frames[current_frame];

                buffer_sprite_draw(
                    &framebuffer,
                    sprite,
                    alien.x,
                    alien.y,
                    rgb_to_uint32(128, 0, 0),
                    3);
            }
        }

        // Draw Player
        buffer_sprite_draw(
            &framebuffer,
            player_sprite,
            game.player.x,
            game.player.y,
            rgb_to_uint32(128, 0, 0),
            3);

        for (std::size_t ai = 0; ai < game.num_aliens; ++ai) {

            const Alien &alien = game.aliens[ai];

            if (alien.type == ALIEN_DEAD && death_counters[ai]) {

                --death_counters[ai];
            }
        }

        // Draw Bullets
        for (std::size_t bi = 0; bi < game.num_bullets; ++bi) {

            const Bullet &bullet = game.bullets[bi];

            buffer_sprite_draw(
                &framebuffer,
                bullet_sprite,
                bullet.x,
                bullet.y,
                rgb_to_uint32(128, 0, 0),
                3);
        }

        for (std::size_t bi = 0; bi < game.num_bullets;) {

            game.bullets[bi].y += game.bullets[bi].dir;

            if (game.bullets[bi].y >= game.height || game.bullets[bi].y < bullet_sprite.height) {

                game.bullets[bi] = game.bullets[game.num_bullets - 1];
                --game.num_bullets;
                continue;
            }

            for (std::size_t ai = 0; ai < game.num_aliens; ++ai) {

                const Alien &alien = game.aliens[ai];

                if (alien.type == ALIEN_DEAD)
                    continue;

                const SpriteAnimation &animation = alien_animation[alien.type - 1];

                std::size_t current_frame = animation.time / animation.frame_duration;
                const Sprite &alien_sprite = *animation.frames[current_frame];

                bool overlap = sprite_overlap_check(
                    bullet_sprite,
                    game.bullets[bi].x,
                    game.bullets[bi].y,
                    alien_sprite,
                    alien.x,
                    alien.y,
                    3);

                if (overlap) {

                    score += 10 * (4 - game.aliens[ai].type);

                    game.aliens[ai].type = ALIEN_DEAD;

                    // Death Sprite zentrieren
                    game.aliens[ai].x -= (alien_death_sprite.width - alien_sprite.width) * 3 / 2;

                    game.bullets[bi] = game.bullets[game.num_bullets - 1];
                    --game.num_bullets;

                    break;
                }
            }

            ++bi;
        }

        for (std::size_t i = 0; i < 3; ++i) {

            ++alien_animation[i].time;

            if (alien_animation[i].time ==
                alien_animation[i].num_frames * alien_animation[i].frame_duration) {

                if (alien_animation[i].loop) {
                    alien_animation[i].time = 0;
                }
            }
        }

        int player_x = static_cast<int>(game.player.x);
        int player_width = static_cast<int>(player_sprite.width * 3);

        player_move_dir = 0;

        if (move_right) {
            player_move_dir += 1;
        }
        if (move_left) {
            player_move_dir -= 1;
        }

        int new_x = player_x + player_move_dir;

        if (new_x < 0) {
            new_x = 0;
        }

        if (new_x + player_width > game.width) {
            new_x = game.width - player_width;
        }

        game.player.x = static_cast<std::size_t>(new_x);

        if (fire_pressed) {
            game_add_bullet(&game);
            fire_pressed = false;
        }

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            framebuffer.width,
            framebuffer.height,
            GL_RGBA,
            GL_UNSIGNED_INT_8_8_8_8,
            framebuffer.data);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }

    // Clean-Up -------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();

    delete[] framebuffer.data;
    delete[] game.aliens;
    delete[] death_counters;

    for (std::size_t i = 0; i < 6; ++i) {
        delete[] alien_sprites[i].data;
    }

    delete[] alien_death_sprite.data;

    for (std::size_t i = 0; i < 3; ++i) {
        delete[] alien_animation[i].frames;
    }
    return EXIT_SUCCESS;
}
