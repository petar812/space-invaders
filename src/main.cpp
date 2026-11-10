#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

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

struct Alien {

    std::size_t x, y;
    std::uint8_t type;
};

struct Player {

    std::size_t x, y;
    std::size_t life;
};

struct Game {

    std::size_t width, height;
    std::size_t num_aliens;
    Alien *aliens;
    Player player;
};

void error_callback(int error, const char *description) {

    // Error Handling
    std::fprintf(stderr, "Error %d: %s\n", error, description);
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

    game->width = 640;
    game->height = 480;

    game->num_aliens = 55;
    game->aliens = new Alien[game->num_aliens];

    for (std::size_t yi = 0; yi < 5; ++yi) {
        for (std::size_t xi = 0; xi < 11; ++xi) {

            game->aliens[yi * 11 + xi].x = 36 * xi + 122;
            game->aliens[yi * 11 + xi].y = 29 * yi + 240;
        }
    }

    game->player.x = 303;
    game->player.y = 32;
    game->player.life = 3;
}

int main() {

    Buffer framebuffer;
    framebuffer.width = 640;
    framebuffer.height = 480;
    framebuffer.data = new std::uint32_t[framebuffer.width * framebuffer.height];

    std::uint32_t clear_color = rgb_to_uint32(0, 0, 0);

    // First Alien Sprite ------------------------------------------
    std::uint8_t alien_data[] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
                                 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0,
                                 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0};

    Sprite alien_sprite;
    alien_sprite.width = 11;
    alien_sprite.height = 8;
    alien_sprite.data = alien_data;

    // Second Alien Sprite -----------------------------------------
    std::uint8_t alien_data2[] = {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
                                  1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                                  0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0};

    Sprite alien_sprite2;
    alien_sprite2.width = 11;
    alien_sprite2.height = 8;
    alien_sprite2.data = alien_data2;

    // Alien Sprite Animation
    SpriteAnimation *alien_animation = new SpriteAnimation;

    alien_animation->loop = true;
    alien_animation->num_frames = 2;
    alien_animation->frame_duration = 10;
    alien_animation->time = 0;

    alien_animation->frames = new Sprite *[2];
    alien_animation->frames[0] = &alien_sprite;
    alien_animation->frames[1] = &alien_sprite2;

    // Player Sprite -----------------------------------------------
    std::uint8_t player_data[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
                                  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                                  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    Sprite player_sprite;
    player_sprite.width = 11;
    player_sprite.height = 7;
    player_sprite.data = player_data;

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

    int player_move_dir = 1;

    // Game Loop -----------------------------------------------

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        buffer_clear(&framebuffer, clear_color);

        for (std::size_t ai = 0; ai < game.num_aliens; ++ai) {

            const Alien &current_alien = game.aliens[ai];

            std::size_t current_frame = alien_animation->time / alien_animation->frame_duration;

            const Sprite &sprite = *alien_animation->frames[current_frame];

            buffer_sprite_draw(
                &framebuffer,
                sprite,
                current_alien.x,
                current_alien.y,
                rgb_to_uint32(128, 0, 0),
                3);
        }
        buffer_sprite_draw(
            &framebuffer,
            player_sprite,
            game.player.x,
            game.player.y,
            rgb_to_uint32(128, 0, 0),
            3);

        ++alien_animation->time;

        if (alien_animation->time ==
            alien_animation->num_frames * alien_animation->frame_duration) {

            if (alien_animation->loop) {
                alien_animation->time = 0;
            }
        }

        int player_x = static_cast<int>(game.player.x);
        int player_width = static_cast<int>(player_sprite.width * 3);

        if (player_x + player_width + player_move_dir >= static_cast<int>(game.width)) {

            game.player.x = game.width - player_width;
            player_move_dir *= -1;

        } else if (player_x + player_move_dir <= 0) {

            game.player.x = 0;
            player_move_dir *= -1;

        } else {
            game.player.x += player_move_dir;
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
    delete[] alien_animation->frames;
    delete alien_animation;

    return EXIT_SUCCESS;
}
