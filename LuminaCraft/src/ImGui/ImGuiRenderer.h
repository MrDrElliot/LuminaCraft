#pragma once
#include <GLFW/glfw3.h>


class ImGuiRenderer
{
public:

    ImGuiRenderer();
    ~ImGuiRenderer();

    
    void Init(GLFWwindow* InWindow);
    void BeginFrame();
    void Update(double DeltaTime);
    void EndFrame();

    void Shutdown();
    
};
