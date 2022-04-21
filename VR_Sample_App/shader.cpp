#include "shader.h"
#include "gtc/type_ptr.hpp"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try { // open files 
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file�s buffer contents into streams 
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers 
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	// vertex Shader 
	vertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any 
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	// similiar for Fragment Shader [...]
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	// shader Program 
	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	glLinkProgram(this->ID);

	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete the shaders as they�re linked into our program now and no longer necessery 
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const GLchar* computePath)
{

	std::string computeCode;


	std::ifstream cShaderFile;
	// ensure ifstream objects can throw exceptions:

	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try { // open files 

		cShaderFile.open(computePath);
		std::stringstream  cShaderStream;
		// read file�s buffer contents into streams 

		cShaderStream << cShaderFile.rdbuf();
		// close file handlers 

		cShaderFile.close();
		// convert stream into string

		computeCode = cShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* cShaderCode = computeCode.c_str();
	GLuint  compute;
	int success;
	char infoLog[512];
	//compute shader
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &cShaderCode, NULL);
	glCompileShader(compute);
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	// shader Program 
	this->ID = glCreateProgram();

	glAttachShader(this->ID, compute);
	glLinkProgram(this->ID);

	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// delete the shaders as they�re linked into our program now and no longer necessery 
	glDeleteShader(compute);
}



void Shader::use() {
	glUseProgram(ID);
}
void Shader::setVec3(const std::string& name, glm::vec3 value) {

	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, glm::mat4 value) {

	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setInt(const std::string& name, const int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setIntARB(const std::string& name, const int value) {
	glUniform1iARB(glGetUniformLocation(ID, name.c_str()), value);
}