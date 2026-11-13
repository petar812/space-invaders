
#include "input.hpp"

Input *Input::instance = nullptr;

Input::Input()
    : moveLeft(false)
    , moveRight(false)
    , firePressed(false)
    , gameRunning(true) {}

void Input::initialize(GLFWwindow *window) {
    instance = this;
    glfwSetKeyCallback(window, keyCallback);
}

void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (instance == nullptr) {
        return;
    }

    switch (key) {

    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS) {
            instance->gameRunning = false;
        }
        break;

    case GLFW_KEY_RIGHT:
    case GLFW_KEY_D:
        if (action == GLFW_PRESS) {
            instance->moveRight = true;
        } else if (action == GLFW_RELEASE) {
            instance->moveRight = false;
        }
        break;

    case GLFW_KEY_LEFT:
    case GLFW_KEY_A:
        if (action == GLFW_PRESS) {
            instance->moveLeft = true;
        } else if (action == GLFW_RELEASE) {
            instance->moveLeft = false;
        }
        break;

    case GLFW_KEY_UP:
    case GLFW_KEY_W:
        if (action == GLFW_PRESS) {
            instance->firePressed = true;
        }
        break;

    default:
        break;
    }
}

bool Input::isMovingLeft() const {
    return moveLeft;
}

bool Input::isMovingRight() const {
    return moveRight;
}

bool Input::isFirePressed() const {
    return firePressed;
}

bool Input::isGameRunning() const {
    return gameRunning;
}

void Input::resetFire() {
    firePressed = false;
}
