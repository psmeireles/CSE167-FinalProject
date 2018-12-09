#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_


#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include "Node.h"
#include "Window.h"

class Transform:public Node
{
public:
	Transform(glm::mat4 transform);
	~Transform();

	glm::mat4 M;
	std::list<Node*> childNodes;

	void addChild(Node* node);
	void removeChild();
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();
	void scale(double);
	
	GLuint VBO, VAO, EBO, normalBuffer;
};

#endif