#ifndef _NODE_H_
#define _NODE_H_


#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <vector>

class Node
{
public:
	GLfloat min_x, max_x;
	GLfloat min_y, max_y;
	GLfloat min_z, max_z;
	bool objIsSelected;

	std::vector<GLuint> sphereIndices;
	std::vector<glm::vec3> sphereVertices;
	std::vector<glm::vec3> sphereNormals;

	Node();
	~Node();

	virtual void draw(GLuint shaderProgram, glm::mat4 C)=0;
	virtual void update()=0;
	virtual void shiftAndResizeSphere()=0;
	virtual void updateMinMaxCoordinates(float x, float y, float z)=0;
};

#endif