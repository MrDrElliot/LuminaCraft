#pragma once
#include <memory>
#include <unordered_map>
#include "Shader.h"


class ShaderLibrary
{
public:


	ShaderLibrary();
	~ShaderLibrary();


	static std::shared_ptr<Shader> GetShader(const std::string& InKey);
	static std::shared_ptr<ShaderLibrary> Get();

	static void PushShader(const std::string& InKey, std::shared_ptr<Shader> InShader);
	static void PopShader(const std::string& InKey);


private:

	static std::shared_ptr<ShaderLibrary> s_ShaderLibrary;

	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderMap;

};