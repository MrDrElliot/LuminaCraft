#pragma once

#include <GLFW/glfw3.h>
#include "World\World.h"
#include <memory>
#include "threadpool/BS_thread_pool.hpp"
#include "ImGui/ImGuiRenderer.h"

struct FApplicationParams
{
	const char* ApplicationName;
	int WindowWidth;
	int WindowHeight;
	bool vSync = true;
};

struct FPerformanceMetrics
{
	double LastFrameTime = 0.0;
	double DeltaTime = 0.0;
	double ElapsedTime = 0.0;
	int Frames = 0;
	double FPS = 0.0;
};

class Application
{
public:

	Application(const FApplicationParams& InParams);
	~Application();


	static std::shared_ptr<Application> Create(const FApplicationParams& InParams);
	static std::shared_ptr<Application> Get();

	static GLFWwindow* GetApplicationWindow();
	static std::shared_ptr<BS::thread_pool> GetThreadPool();

	static FApplicationParams& GetApplicationParams();
	static FPerformanceMetrics& GetPerformanceMetrics();
	static std::shared_ptr<World> GetWorld();

	void InitializeGLFW();
	void UpdatePerformanceMetrics(double currentTimeSeconds);
	
	void Init();
	void Run();
	void Terminate();
	

private:

	std::shared_ptr<BS::thread_pool> ThreadPool;

	static std::shared_ptr<Application> s_Application;
	FApplicationParams Params;
	FPerformanceMetrics Metrics;

	std::shared_ptr<World> m_MainWorld;
	GLFWwindow* ApplicationWindow;

	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

	

};