#include "ImGuiRenderer.h"
#include <imgui.h>
#include "../World/Block.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Application.h"
#include "../Logging/Log.h"
#include "../Player/Player.h"
#include "../Renderer/Renderer.h"

ImGuiRenderer::ImGuiRenderer()
{
}

ImGuiRenderer::~ImGuiRenderer()
{
}

void ImGuiRenderer::Init(GLFWwindow* InWindow)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(InWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    LOG_INFO("ImGuiRenderer Initialized");
}

void ImGuiRenderer::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiRenderer::Update(double DeltaTime)
{
    std::shared_ptr<World> World = Application::GetWorld();
    std::shared_ptr<Player> Player = World->GetPlayer();
    ImGui::Begin("Game Stats");
    ImGui::Checkbox("VSync", &Application::GetApplicationParams().vSync);
    ImGui::Text("Thread Count: %i", Application::GetThreadPool()->get_thread_count());
    ImGui::Spacing();
    ImGui::Text("FPS: %f", Application::GetPerformanceMetrics().FPS);
    ImGui::Text("DeltaTime: %f", DeltaTime);
    ImGui::Spacing();
    ImGui::Text("Player Position: X: %f Y: %f Z: %f", Player->GetPosition().x, Player->GetPosition().y, Player->GetPosition().z);
    ImGui::Text("Block Type: %s", Block::BlockTypeToString(static_cast<Block::EBlockType>(World->GetBlockAtWorldPosition(Player->GetPosition()))).c_str());
    ImGui::End();
}

void ImGuiRenderer::EndFrame()
{
    ImGui::Render();

    Renderer::Submit([]
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
}

void ImGuiRenderer::Shutdown()
{
    ImGui::DestroyContext();
}
