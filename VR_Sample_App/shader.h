#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GL/glew.h"
#include "glm.hpp"

class Shader
{
public:
	unsigned int ID;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* computePath);
	void use();
	void setVec3(const std::string& name, glm::vec3 value);
	void setInt(const std::string& name, const int value);
	void setIntARB(const std::string& name, const int value);
	void setMat4(const std::string& name, glm::mat4 value);
};