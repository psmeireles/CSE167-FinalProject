#pragma once
#include "Geometry.h"
class BoundingSphere :
	public Node
{
public:
	BoundingSphere();
	BoundingSphere(char* filepath, GLuint shader, glm::vec3 color);
	~BoundingSphere();
	
	void draw(GLuint shaderProgram, glm::mat4 C);

	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texels;
	glm::vec3 color;

	void shiftAndResizeModel();

	void update();
	void scale(double);
	void parse(const char* filepath);
	void updateMinMaxCoordinates(float x, float y, float z);

	static bool debugMode;

	glm::mat4 toWorld;

	GLuint VBO, VAO, EBO, normalBuffer;
	GLuint shader;
};

