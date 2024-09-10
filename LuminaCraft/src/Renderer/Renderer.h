#pragma once
#include <memory>


class Renderer
{
public:

	Renderer();
	~Renderer();

	static void Init();


	static void BeginFrame();
	static void EndFrame();


private:


	static std::shared_ptr<Renderer> s_Renderer;

};