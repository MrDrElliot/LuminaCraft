#pragma once
#include "../Application.h"
#include "../Events/KeyCodes.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace Input
{
    inline bool IsMouseButtonPressed(const KeyCode Key)
    {
        GLFWwindow* Window = Application::GetApplicationWindow();
        int State = glfwGetMouseButton(Window, Key);

        return State == GLFW_PRESS;
    }

    inline bool IsKeyPressed(const KeyCode Key)
    {
        GLFWwindow* Window = Application::GetApplicationWindow();
        int State = glfwGetKey(Window, Key);

        return State == GLFW_PRESS;
    }

    inline glm::vec2 GetMousePos()
    {
        GLFWwindow* Window = Application::GetApplicationWindow();
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }
}
