
#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.inl>

#include "Vertex.h"
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

void Renderer::CreateBuffers(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& Indices, uint32_t& VAO, uint32_t& VBO, uint32_t& EBO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, posX)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texGridX)));
	glEnableVertexAttribArray(1);
	
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32_t), Indices.data(), GL_STATIC_DRAW);
}

void Renderer::DrawIndexed(uint32_t modelLoc, const glm::mat4& model, uint32_t VAO, int Count)
{
	Submit([modelLoc, model, VAO, Count]
	{
		glBindVertexArray(VAO);
		
		// Pass the model matrix to the shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		// Draw the chunk using the indices
		glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, 0);
	});
}

void Renderer::Submit(const std::function<void()>& RenderCommand)
{
	s_Renderer->RenderQueue.push(RenderCommand);
}

void Renderer::BeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
	ProcessRenderQueue();

	
	glfwSwapBuffers(Application::GetApplicationWindow());
	glfwPollEvents();
}

void Renderer::ProcessRenderQueue()
{
	while (!s_Renderer->RenderQueue.empty())
	{
		std::function<void()> command = s_Renderer->RenderQueue.front();
		s_Renderer->RenderQueue.pop();
        
		command();
	}
}
