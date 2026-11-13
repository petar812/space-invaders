
#include "renderer.hpp"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

Renderer::Renderer(std::size_t width, std::size_t height)
    : width(width)
    , height(height)
    , framebuffer(width * height) {}

Renderer::~Renderer() {

    if (window != nullptr) {

        glfwMakeContextCurrent(window);

        if (bufferTexture != 0) {
            glDeleteTextures(1, &bufferTexture);
        }

        if (vertexArrayObject != 0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        if (shaderProgram != 0) {
            glDeleteProgram(shaderProgram);
        }

        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

std::uint32_t Renderer::rgbToUint32(std::uint8_t r, std::uint8_t g, std::uint8_t b) {

    return (r << 24) | (g << 16) | (b << 8) | 255;
}

void Renderer::clear(std::uint32_t color) {

    for (std::size_t i = 0; i < framebuffer.size(); ++i) {
        framebuffer[i] = color;
    }
}

void Renderer::drawLine(std::size_t y, std::uint32_t color) {

    if (y >= height) {
        return;
    }

    for (std::size_t x = 0; x < width; ++x) {
        framebuffer[y * width + x] = color;
    }
}

void Renderer::drawSprite(
    const Sprite &sprite, std::size_t x, std::size_t y, std::uint32_t color, std::size_t scale) {

    for (std::size_t xi = 0; xi < sprite.getWidth(); ++xi) {
        for (std::size_t yi = 0; yi < sprite.getHeight(); ++yi) {

            if (!sprite.getData()[yi * sprite.getWidth() + xi]) {
                continue;
            }

            for (std::size_t dx = 0; dx < scale; ++dx) {
                for (std::size_t dy = 0; dy < scale; ++dy) {

                    std::size_t sx = x + xi * scale + dx;
                    std::size_t sy = y + (sprite.getHeight() - 1 - yi) * scale + dy;

                    if (sx < width && sy < height) {
                        framebuffer[sy * width + sx] = color;
                    }
                }
            }
        }
    }
}

void Renderer::drawText(
    const Font &font, const char *text, std::size_t x, std::size_t y, std::uint32_t color) {

    std::size_t xp = x;

    for (const char *charp = text; *charp != '\0'; ++charp) {

        Sprite glyph = font.getGlyph(*charp);

        drawSprite(glyph, xp, y, color, 3);

        xp += (glyph.getWidth() + 1) * 3;
    }
}

void Renderer::drawNumber(
    const Font &font, std::size_t number, std::size_t x, std::size_t y, std::uint32_t color) {

    char text[32];

    std::snprintf(text, sizeof(text), "%zu", number);

    drawText(font, text, x, y, color);
}

std::uint32_t *Renderer::getFramebufferData() {
    return framebuffer.data();
}

std::string Renderer::loadShader(const char *filepath) {

    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open shader file!");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint Renderer::compileShader(GLenum type, const std::string &source) {

    GLuint shader = glCreateShader(type);

    const char *source_code = source.c_str();

    glShaderSource(shader, 1, &source_code, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {

        char log[1024];

        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::fprintf(stderr, "Shader compilation error: %s\n", log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Renderer::createShaderProgram() {

    std::string vertex_source = loadShader("shaders/vertex.glsl");

    std::string fragment_source = loadShader("shaders/fragment.glsl");

    GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_source);

    GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_source);

    if (!vertex_shader || !fragment_shader) {
        throw std::runtime_error("Shader compilation failed!");
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertex_shader);
    glAttachShader(shaderProgram, fragment_shader);

    glLinkProgram(shaderProgram);

    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {

        char log[1024];

        glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);

        std::fprintf(stderr, "Program linking error: %s\n", log);

        throw std::runtime_error("Shader linking failed!");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(shaderProgram);

    GLint location = glGetUniformLocation(shaderProgram, "buffer");

    glUniform1i(location, 0);
}

static void errorCallback(int error, const char *description) {

    std::fprintf(stderr, "Error %d: %s\n", error, description);
}

bool Renderer::initialize() {

    glfwSetErrorCallback(errorCallback);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit()) {
        std::fprintf(stderr, "GLFW initialization failed!\n");
        return false;
    }

    window = glfwCreateWindow(
        static_cast<int>(width),
        static_cast<int>(height),
        "Space Invaders",
        nullptr,
        nullptr);

    if (window == nullptr) {
        std::fprintf(stderr, "Window creation failed!\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    GLenum glew_status = glewInit();

    if (glew_status != GLEW_OK) {

        std::fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(glew_status));

        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();

        return false;
    }

    initializeOpenGL();

    return true;
}

void Renderer::initializeOpenGL() {

    // Create Shader Program
    createShaderProgram();

    // Create Vertex Array Object
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create Framebuffer Texture
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &bufferTexture);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8,
        framebuffer.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glDisable(GL_DEPTH_TEST);

    std::fprintf(stdout, "OpenGL-Version: %s\n", glGetString(GL_VERSION));
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();
}

void Renderer::swapBuffers() {
    glfwSwapBuffers(window);
}

void Renderer::render() {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        width,
        height,
        GL_RGBA,
        GL_UNSIGNED_INT_8_8_8_8,
        framebuffer.data());

    glBindVertexArray(vertexArrayObject);

    glUseProgram(shaderProgram);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLFWwindow *Renderer::getWindow() const {
    return window;
}
