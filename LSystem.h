#ifndef _LSYSTEM_H_
#define _LSYSTEM_H_


#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <vector>
#include <unordered_map>

class LSystem
{
private:
	std::vector<char> variables;
	std::vector<GLfloat> params;
	std::string initString;
	std::unordered_map<char, std::string> ruleMap;

public:


	// variables, parameters, initial string, production rules
	// array of chars for v
	LSystem(std::vector<char> variables, std::vector<GLfloat> params, std::string initString, std::unordered_map<char, std::string> ruleMap);
	~LSystem();

	std::string generateString(GLint recursions);
	std::vector<char> getVariables();
	std::vector<GLfloat> getParams();

};

#endif