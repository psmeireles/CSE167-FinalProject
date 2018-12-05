#ifndef _TERRAIN_H_
#define _TERRAIN_H_


#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Node.h"
#include "Window.h"
#include <time.h>

class Terrain :
	public Node
{
private:
	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texels;
	glm::vec3 color;
	GLuint shader;
public:
	Terrain(int size, GLuint shader, char* texturePath);
	~Terrain();
	double **map;
	void update();
	void scale(double);
	void parse(int size);
	void draw(GLuint shaderProgram, glm::mat4 C);
	void updateMinMaxCoordinates(float x, float y, float z);
	void shiftAndResizeSphere();
	void loadTexture(char* path);

	glm::mat4 toWorld;

	GLuint VBO, VAO, EBO, normalBuffer, texBuffer;
};

#endif