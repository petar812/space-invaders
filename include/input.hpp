
#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Input {
  private:
    bool moveLeft;
    bool moveRight;
    bool firePressed;
    bool gameRunning;

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static Input *instance;

  public:
    Input();

    void initialize(GLFWwindow *window);

    bool isMovingLeft() const;
    bool isMovingRight() const;
    bool isFirePressed() const;
    bool isGameRunning() const;

    void resetFire();
};
