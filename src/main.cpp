#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

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
        if (action == GLFW_RELEASE) {
            fire_pressed = false;
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
            game->aliens[yi * 11 + xi].y = 29 * yi + 240;

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
    game->player.y = 32;
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
                alien_animation[i].num_frames * alien_animation->frame_duration) {

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
