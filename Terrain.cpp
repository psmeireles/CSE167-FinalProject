#include "Terrain.h"

using namespace std;

void dsa(double **m, int size);
void squareStep(double **m, int x, int z, int reach);
void diamondStep(double **m, int x, int z, int reach);
float random(int range);
int terrainSize = 0;

Terrain::Terrain(int size, GLuint shader)
{
	toWorld = glm::mat4(1.0f);
	min_x = std::numeric_limits<float>::max();
	min_y = std::numeric_limits<float>::max();
	min_z = std::numeric_limits<float>::max();
	max_x = std::numeric_limits<float>::lowest();
	max_y = std::numeric_limits<float>::lowest();
	max_z = std::numeric_limits<float>::lowest();
	
	map = (double **)malloc(size*sizeof(double*));
	for (int i = 0; i < size; i++) {
		map[i] = (double *)malloc(size * sizeof(double));
	}

	map[0][0] = 0;
	map[0][size-1] = 0;
	map[size-1][0] = 0;
	map[size-1][size-1] = 0;

	terrainSize = size;
	srand(glfwGetTime());
	dsa(map, size);

	parse(size);

	for (int i = 0; i < size; i++) {
		free(map[i]);
	}
	free(map);

	this->color = glm::vec3(0.0, 1.0, 0.0);
	this->shader = shader;
	objIsSelected = true;

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &texBuffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
	glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(glm::vec2), texels.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		2 * sizeof(GLfloat),                                // stride
		(GLvoid*)0                          // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		3 * sizeof(GLfloat),                                // stride
		(GLvoid*)0                          // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &normalBuffer);
}

void Terrain::draw(GLuint shaderProgram, glm::mat4 C) {

	glUseProgram(shader);

	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 model = glm::inverse(Window::V)*C*toWorld;
	glm::mat4 view = Window::V;

	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	GLuint uProjection = glGetUniformLocation(shader, "projection");
	GLuint uColor = glGetUniformLocation(shader, "color");
	GLuint uModel = glGetUniformLocation(shader, "model");
	GLuint uView = glGetUniformLocation(shader, "view");
	GLuint uCamPos = glGetUniformLocation(shader, "cameraPos");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
	glUniform3fv(uCamPos, 1, &Window::camPos[0]);
	glUniform3fv(uColor, 1, &this->color[0]);

	// Now draw the OBJObject. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, indices[0], indices.size());
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

}

void Terrain::update()
{
	//spin(1.0f);
}

void Terrain::scale(double x)
{
	toWorld = toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(x));
}

void Terrain::parse(int size)
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			float x = i - size / 2;
			float z = j - size / 2;
			this->vertices.push_back(glm::vec3(x, map[i][j], z));
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			glm::vec3 normal;
			if (j == size-1 && i == size-1) {
				normal = glm::cross(this->vertices[size*(i - 1) + j] - this->vertices[size*i + j], this->vertices[size*i + (j - 1)] - this->vertices[size*i + j]);
			}
			else if (i == size-1) {
				normal = glm::cross(this->vertices[size*(i - 1) + j] - this->vertices[size*i + j], this->vertices[size*i + (j + 1)] - this->vertices[size*i + j]);
			}
			else if (j == size-1) {
				normal = glm::cross(this->vertices[size*(i + 1) + j] - this->vertices[size*i + j], this->vertices[size*i + (j - 1)] - this->vertices[size*i + j]);
			}
			else {
				normal = glm::cross(this->vertices[size*(i + 1) + j] - this->vertices[size*i + j], this->vertices[size*i + (j + 1)] - this->vertices[size*i + j]);
			}
			this->normals.push_back(glm::normalize(normal));
		}
	}

	for (int i = 0; i < size-1; i++) {
		for (int j = 0; j < size - 1; j++) {
			this->indices.push_back(i * size + j);
			this->indices.push_back((i + 1)*size + j);
			this->indices.push_back(i *size + (j + 1));
			this->indices.push_back((i + 1)*size + j);
			this->indices.push_back(i *size + (j + 1));
			this->indices.push_back((i+1) * size + (j+1));
		}
	}
}

void Terrain::updateMinMaxCoordinates(float x, float y, float z) {

}

void Terrain::shiftAndResizeSphere() {

}

void dsa(double **m, int size) {
	int half = size / 2;
	if (half < 1)
		return;

	// Square
	for (int j = half; j < terrainSize; j += 2*half) {
		for (int i = half; i < terrainSize; i += 2*half) {
			squareStep(m, i%terrainSize, j%terrainSize, half);
		}
	}

	// Diamond
	int col = 0;
	for (int i = 0; i < terrainSize; i += half) {
		col++;
		if (col % 2 == 1) {
			for (int j = half; j < terrainSize; j += 2*half) {
				diamondStep(m, i%terrainSize, j%terrainSize, half);
			}
		}
		else {
			for (int j = 0; j < terrainSize; j += 2*half) {
				diamondStep(m, i%terrainSize, j%terrainSize, half);
			}
		}
	}

	dsa(m, size / 2);

}

void squareStep(double **m, int x, int z, int reach) {
	int count = 0;
	float avg = 0.0f;

	if (x - reach >= 0 && z - reach >= 0) {
		avg += m[x - reach][z - reach];
		count++;
	}
	if (x - reach >= 0 && z + reach < terrainSize) {
		avg += m[x - reach][z + reach];
		count++;
	}
	if (x + reach < terrainSize && z - reach >= 0) {
		avg += m[x + reach][z - reach];
		count++;
	}
	if (x + reach < terrainSize && z + reach < terrainSize) {
		avg += m[x + reach][z + reach];
		count++;
	}

	avg += random(reach);
	avg /= count;
	m[x][z] = avg;
}

void diamondStep(double **m, int x, int z, int reach) {
	int count = 0;
	float avg = 0.0f;

	if (x - reach >= 0) {
		avg += m[x - reach][z];
		count++;
	}
	if (x + reach < terrainSize) {
		avg += m[x + reach][z];
		count++;
	}
	if (z - reach >= 0) {
		avg += m[x][z - reach];
		count++;
	}
	if (z + reach < terrainSize) {
		avg += m[x][z + reach];
		count++;
	}

	avg += random(reach);
	avg /= count;
	m[x][z] = avg;
}

float random(int range) {
	return (rand() % (2 * range)) - range;
}