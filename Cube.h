#ifndef _CUBE_H_
#define _CUBE_H_


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
#include "stb_image.h"

class Cube : public Node
{
private:
	std::vector<float> skyboxVertices;


	std::vector<std::string> faces = {
	"../skybox/right.jpg",
	"../skybox/left.jpg",
	"../skybox/top.jpg",
	"../skybox/bottom.jpg",
	"../skybox/front.jpg",
	"../skybox/back.jpg"
	};

	void shiftAndResizeModel();
	void loadTexture();
public:
	Cube();
	~Cube();

	void update();
	void spin(float);
	void scale(double);
	void parse(const char* filepath);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void updateMinMaxCoordinates(float x, float y, float z);
	void shiftAndResizeSphere();

	glm::mat4 toWorld;

	GLuint VBO, VAO, normalBuffer;
};

#endif