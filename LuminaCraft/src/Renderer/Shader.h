#pragma once

#include <string>

class Shader
{
public:

	Shader(const char* vertexPath, const char* fragmentPath);

	void Use();

	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;

	
	unsigned int GetProgramID() const { return ID; }

private:

	unsigned int ID;
};