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
private:
	void parseSphere();
public:
	Transform(glm::mat4 transform);
	~Transform();

	glm::mat4 M;
	std::list<Node*> childNodes;

	float radius;
	glm::vec3 center;

	void addChild(Node* node);
	void removeChild();
	void draw(GLuint shaderProgram, glm::mat4 C);
	void update();
	void update(glm::mat4 transform);
	void scale(double);
	void updateMinMaxCoordinates(float x, float y, float z);
	void shiftAndResizeSphere();
	bool isVisible(glm::vec3 point, float r);

	bool objIsSelected;

	int lastState;
	static int nRendered;

	GLuint VBO, VAO, EBO, normalBuffer;
};

#endif