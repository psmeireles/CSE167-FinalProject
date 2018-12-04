#include "Transform.h"

int Transform::nRendered = 0;

Transform::Transform(glm::mat4 transform)
{
	this->M = transform;
	min_x = std::numeric_limits<float>::max();
	min_y = std::numeric_limits<float>::max();
	min_z = std::numeric_limits<float>::max();
	max_x = std::numeric_limits<float>::lowest();
	max_y = std::numeric_limits<float>::lowest();
	max_z = std::numeric_limits<float>::lowest();

	parseSphere();
	objIsSelected = true;
	lastState = 0;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), sphereVertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sphereNormals.size() * sizeof(glm::vec3), sphereNormals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		3 * sizeof(GLfloat),                                // stride
		(GLvoid*)0                          // array buffer offset
	);

	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


Transform::~Transform()
{
}

void Transform::addChild(Node* node)
{
	this->childNodes.push_back(node);
}

void Transform::removeChild()
{
	return this->childNodes.pop_back();
}

void Transform::draw(GLuint shaderProgram, glm::mat4 C)
{
	glm::mat4 newC = C * M;
	glm::vec3 newCenter = newC * glm::vec4(center, 1.0f);

	if (!Window::culling || isVisible(newCenter, radius)) {
		if (lastState == 0) {
			lastState = 1;
			Transform::nRendered++;
		}
		if (objIsSelected) {
			for (std::list<Node*>::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
				(*it)->draw(shaderProgram, newC);
		}
		else {
			glUseProgram(shaderProgram);

			// Calculate the combination of the model and view (camera inverse) matrices
			glm::mat4 model = M;
			glm::mat4 view = C;

			// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
			// Consequently, we need to forward the projection, view, and model matrices to the shader programs
			// Get the location of the uniform variables "projection" and "modelview"
			//GLuint uObjectColor = glGetUniformLocation(shaderProgram, "objectColor");

			GLuint uNormalColor = glGetUniformLocation(shaderProgram, "normalColor");
			GLuint uObjIsSelected = glGetUniformLocation(shaderProgram, "objIsSelected");

			GLuint uProjection = glGetUniformLocation(shaderProgram, "projection");
			GLuint uModel = glGetUniformLocation(shaderProgram, "model");
			GLuint uView = glGetUniformLocation(shaderProgram, "view");
			// Now send these values to the shader program
			glUniform1i(uNormalColor, Window::normalColor);
			glUniform1i(uObjIsSelected, false);

			glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
			glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
			glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);

			// Now draw the OBJObject. We simply need to bind the VAO associated with it.
			glBindVertexArray(VAO);
			// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
			glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
			// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
			glBindVertexArray(0);
		}
	}
	else {
		if (lastState == 1) {
			lastState = 0;
			Transform::nRendered--;
		}
	}
}

void Transform::update()
{

}

void Transform::scale(double x)
{
	M = M * glm::scale(glm::mat4(1.0f), glm::vec3(x));
}

void Transform::parseSphere()
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	char c1, c2;
	GLfloat x, y, z;

	FILE *fp = fopen("../obj/sphere.obj", "rb");
	if (fp == NULL) {
		printf("transform:error loading file\n");
		system("pause");
		exit(-1);
	}

	c1 = fgetc(fp);
	while (c1 != EOF) {
		if (c1 == 'v') {
			c2 = fgetc(fp);

			if (c2 == 'n') {
				fscanf(fp, " %f %f %f", &x, &y, &z);
				this->sphereNormals.push_back(glm::vec3(x, y, z));
			}
			else if (c2 == ' ') {
				fscanf(fp, " %f %f %f", &x, &y, &z);
				this->sphereVertices.push_back(glm::vec3(x, y, z));
				updateMinMaxCoordinates(x, y, z);
			}
		}
		else if (c1 == 'f') {
			GLuint f[6];
			fscanf(fp, " %u//%u %u//%u %u//$u", &f[0], &f[1], &f[2], &f[3], &f[4], &f[5]);
			for (int i = 0; i < 3; i++)
				this->sphereIndices.push_back(f[2 * i] - 1);
		}
		c1 = fgetc(fp);
	}
	fclose(fp);

	shiftAndResizeSphere();
}

void Transform::shiftAndResizeSphere() {
	// Find center of model
	GLfloat avg_x = (max_x + min_x) / 2.0f;
	GLfloat avg_y = (max_y + min_y) / 2.0f;
	GLfloat avg_z = (max_z + min_z) / 2.0f;

	// Finding max coordinate
	GLfloat max_abs_x = abs(max_x) > abs(min_x) ? abs(max_x) : abs(min_x);
	GLfloat max_abs_y = abs(max_y) > abs(min_y) ? abs(max_y) : abs(min_y);
	GLfloat max_abs_z = abs(max_z) > abs(min_z) ? abs(max_z) : abs(min_z);

	GLfloat max_coord = max_abs_x;
	if (max_coord < max_abs_y) {
		max_coord = max_abs_y;
	}
	if (max_coord < max_abs_z) {
		max_coord = max_abs_z;
	}

	// Shifting and resizing all vertices
	for (int i = 0; i < sphereVertices.size(); i++) {
		sphereVertices[i].x -= avg_x;
		sphereVertices[i].x /= max_coord/2.5;
		sphereVertices[i].y -= avg_y - 0.5;
		sphereVertices[i].y /= max_coord/2.5;
		sphereVertices[i].z -= avg_z;
		sphereVertices[i].z /= max_coord/2.5;
	}

	radius = 2.5;
	center = glm::vec3(0.0, 0.5, 0.0);
}

void Transform::updateMinMaxCoordinates(float x, float y, float z)
{
	if (x > max_x) max_x = x;
	if (x < min_x) min_x = x;
	if (y > max_y) max_y = y;
	if (y < min_y) min_y = y;
	if (z > max_z) max_z = z;
	if (z < min_z) min_z = z;
}

bool Transform::isVisible(glm::vec3 point, float r)
{
	for (int i = 0; i < Window::planesNormals.size(); i++) {
		float dist = Window::dist(Window::planesNormals[i], Window::planesPoints[i], point);
		if (dist < -r) {
			return false;
		}
	}
	return true;
}