#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_


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
#include <vector>
#include "Node.h"
#include "Window.h"

class Geometry:public Node
{
private:
	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texels;
	glm::vec3 color;
	GLuint shader;

	void shiftAndResizeModel();
public:
	Geometry(char* filepath, GLuint shader, glm::vec3 color);
	~Geometry();

	void update();
	void scale(double);
	void parse(const char* filepath);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void updateMinMaxCoordinates(float x, float y, float z);
	bool isVisible(glm::vec3 point, float r);

	glm::mat4 toWorld;

	GLuint VBO, VAO, EBO, normalBuffer, texBuffer;;
};

#endif