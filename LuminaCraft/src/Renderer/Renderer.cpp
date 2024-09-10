
#include "Renderer.h"
#include <glad/glad.h>
#include "../Logging/Log.h"
#include "../Application.h"


std::shared_ptr<Renderer> Renderer::s_Renderer;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Init()
{
	s_Renderer = std::make_shared<Renderer>();

	LOG_INFO("Renderer Initialized");
}

void Renderer::BeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
	glfwSwapBuffers(Application::GetApplicationWindow());
	glfwPollEvents();
}
