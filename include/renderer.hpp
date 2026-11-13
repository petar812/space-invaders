#pragma once

#include "font.hpp"
#include "sprite.hpp"

#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Renderer {

  private:
    std::size_t width;
    std::size_t height;

    std::vector<std::uint32_t> framebuffer;

    std::string loadShader(const char *filepath);

    GLuint shaderProgram = 0;
    GLuint vertexArrayObject = 0;
    GLuint bufferTexture = 0;

    GLuint compileShader(GLenum type, const std::string &source);
    void createShaderProgram();

    GLFWwindow *window = nullptr;
    void initializeOpenGL();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

  public:
    Renderer(std::size_t width, std::size_t height);
    ~Renderer();

    void clear(std::uint32_t color);

    void drawLine(std::size_t y, std::uint32_t color);

    void drawSprite(
        const Sprite &sprite, std::size_t x, std::size_t y, std::uint32_t color, std::size_t scale);

    void drawText( // 8=D
        const Font &font, const char *text, std::size_t x, std::size_t y, std::uint32_t color);

    void drawNumber(
        const Font &font, std::size_t number, std::size_t x, std::size_t y, std::uint32_t color);

    std::uint32_t *getFramebufferData();

    static std::uint32_t rgbToUint32(std::uint8_t r, std::uint8_t g, std::uint8_t b);

    bool initialize();
    void render();
    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    GLFWwindow *getWindow() const;
};
