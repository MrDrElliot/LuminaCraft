#include "ShaderLibrary.h"

std::shared_ptr<ShaderLibrary> ShaderLibrary::s_ShaderLibrary;

ShaderLibrary::ShaderLibrary()
{
}

ShaderLibrary::~ShaderLibrary()
{
}

std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string& InKey)
{
	return Get()->ShaderMap[InKey];
}

std::shared_ptr<ShaderLibrary> ShaderLibrary::Get()
{
	if (!s_ShaderLibrary)
	{
		s_ShaderLibrary = std::make_shared<ShaderLibrary>();
	}

	return s_ShaderLibrary;
}

void ShaderLibrary::PushShader(const std::string& InKey, std::shared_ptr<Shader> InShader)
{
	Get()->ShaderMap.emplace(InKey, InShader);
}

void ShaderLibrary::PopShader(const std::string& InKey)
{
	Get()->ShaderMap.erase(InKey);
}
