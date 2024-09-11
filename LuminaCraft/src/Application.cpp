#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "Renderer/Shader.h"
#include "World\World.h"
#include "Logging/Log.h"
#include "Renderer/Renderer.h"
#include "Renderer/ShaderLibrary.h"
#include "Application.h"

#include <glad/glad.h>


std::shared_ptr<Application> Application::s_Application;


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Application::Get()->GetApplicationParams().WindowWidth = width;
	Application::Get()->GetApplicationParams().WindowHeight = height;

	glViewport(0, 0, width, height);
}


Application::Application(const FApplicationParams& InParams)
	: Params(InParams), ApplicationWindow(nullptr)
{
}

Application::~Application()
{
}

std::shared_ptr<Application> Application::Create(const FApplicationParams& InParams)
{
	s_Application = std::make_shared<Application>(InParams);
	return s_Application;
}

std::shared_ptr<Application> Application::Get()
{
	return s_Application;
}

GLFWwindow* Application::GetApplicationWindow()
{
	return Get()->ApplicationWindow;
}

FApplicationParams& Application::GetApplicationParams()
{
	return Get()->Params;
}

FPerformanceMetrics& Application::GetPerformanceMetrics()
{
	return Get()->Metrics;
}

std::shared_ptr<World> Application::GetWorld()
{
	return Get()->m_MainWorld;
}


void Application::InitializeGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Application::UpdatePerformanceMetrics(double currentTimeSeconds)
{
	// Calculate delta time
	Metrics.DeltaTime = currentTimeSeconds - Metrics.LastFrameTime;
	Metrics.LastFrameTime = currentTimeSeconds;
        
	// Update elapsed time and frame count
	Metrics.ElapsedTime += Metrics.DeltaTime;
	Metrics.Frames++;

	Metrics.FPS = Metrics.Frames / Metrics.ElapsedTime;
	Metrics.ElapsedTime = 0.0;
	Metrics.Frames = 0;
}

void Application::Init()
{
	Lumina::Log::Init();

	Renderer::Init();
	
	m_ImGuiRenderer = std::make_shared<ImGuiRenderer>();

	InitializeGLFW();
	
	// Create window
	ApplicationWindow = glfwCreateWindow(Params.WindowWidth, Params.WindowHeight, Params.ApplicationName, nullptr, nullptr);
	if (ApplicationWindow == nullptr)
	{
		LOG_CRITICAL("Failed to initialize GLFW Window");
		glfwTerminate();
	}


	glfwMakeContextCurrent(ApplicationWindow);
	glfwSwapInterval(1);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_CRITICAL("Failed to initialize GLFW Window");
	}

	m_ImGuiRenderer->Init(ApplicationWindow);
	
	// Configure viewport and rendering
	glViewport(0, 0, Params.WindowWidth, Params.WindowHeight);
	glfwSetFramebufferSizeCallback(ApplicationWindow, framebufferSizeCallback);

	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);  

	glCullFace(GL_GREATER);
	glDepthFunc(GL_LESS);
	
	glFrontFace(GL_CW);


	std::shared_ptr<Shader> PrimaryShader = std::make_shared<Shader>("assets/shaders/vertex_shader.glsl", "assets/shaders/fragment_shader.glsl");
	ShaderLibrary::PushShader("Primary", PrimaryShader);
	PrimaryShader->Use();

	PrimaryShader->SetFloat("texMultiplier", 0.5f);

	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("assets/sprites/block_map.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		LOG_ERROR("Failed to load texture: {0}", "assets/sprites/block_map.png");
	}

	stbi_image_free(data);

	glfwSetInputMode(ApplicationWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_MainWorld = World::CreateWorld("Elliot World");
	

}


void Application::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	m_ImGuiRenderer->Shutdown();
	glfwTerminate();
}

void Application::Run()
{
	auto startTime = std::chrono::high_resolution_clock::now();


	while (!glfwWindowShouldClose(ApplicationWindow))
	{
		// Get current time
		auto currentTime = std::chrono::high_resolution_clock::now();
		double currentTimeSeconds = std::chrono::duration<double>(currentTime - startTime).count();
        
		UpdatePerformanceMetrics(currentTimeSeconds);
		
		// Begin frame rendering
		Renderer::BeginFrame();
        
		// Update the main planet
		m_MainWorld->Update(Metrics.DeltaTime);

		// Update ImGui
		m_ImGuiRenderer->BeginFrame();
		m_ImGuiRenderer->Update(Metrics.DeltaTime);
		m_ImGuiRenderer->EndFrame();
        
		// End frame rendering
		Renderer::EndFrame();
	}
}
