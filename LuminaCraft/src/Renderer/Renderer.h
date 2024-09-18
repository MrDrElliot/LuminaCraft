#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <glm/fwd.hpp>


struct Vertex;

class Renderer
{
public:

	Renderer();
	~Renderer();

	static void Init();
	
	static void CreateBuffers(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& Indices, uint32_t& VAO, uint32_t& VBO, uint32_t& EBO);
	static void DrawIndexed(uint32_t modelLoc, const glm::mat4& model, uint32_t VAO, int Count);

	// Submit rendering commands to the queue
	static void Submit(const std::function<void()>& RenderCommand);

	static void BeginFrame();
	static void EndFrame();


private:

	static void ProcessRenderQueue();

private:
	
	std::queue<std::function<void()>> RenderQueue;
	static std::shared_ptr<Renderer> s_Renderer;

};
