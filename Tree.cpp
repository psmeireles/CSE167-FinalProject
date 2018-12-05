#include "Tree.h"

using namespace std;

Tree::Tree(GLuint shader, LSystem * treeSystem, glm::vec3 startPos)
{
	GLint recursions = 3;
	this->treeSystem = treeSystem;
	this->currentPos = startPos;
	this->currentDir = glm::vec3(0.0f, 1.0f, 0.0f); // tree starts pointing up in y direction
	
	toWorld = glm::mat4(1.0f);
	this->color = glm::vec3(0.0, 1.0, 0.0);
	this->shader = shader;

	std::string language = this->treeSystem->generateString(3);
	printf("language:%s\n", language.c_str());
	generateVertices(language);
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


Tree::~Tree()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &normalBuffer);
}

void Tree::draw(GLuint shaderProgram, glm::mat4 C) {
	
	//printf("treedraw\n");
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

	glLineWidth(40);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, indices[0], indices.size());
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

}

void Tree::update()
{
}

void Tree::rotateDir(GLfloat angle, glm::vec3 axis)
{
	currentDir = glm::vec3(glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis) * glm::vec4(currentDir, 0.0f)) ;
	
	currentDir = glm::normalize(currentDir); // normalizes the direction vector after rotation
}

void Tree::randomize(int range) {
	(rand() % (2 * range)) - range;
}

void Tree::generateVertices(std::string language)
{
	printf("Generatedvertices: %s\n", language.c_str());
	std::vector<char> variables = this->treeSystem->getVariables();
	std::vector<GLfloat> params = this->treeSystem->getParams();
	std::unordered_map<char, GLfloat> variableMap;

	// Adds all variables and corresponding params to the map
	for (int i = 0; i < variables.size(); i++)
	{
		variableMap[variables[i]] = params[i];
	}

	std::pair < glm::vec3, glm::vec3 > temp;
	// Format: 
	//    All letters = move forward
	//    [ symbol = push, rotate left (around z axis)
	//    ] symbol = pop, rotate right (around z axis)
	//    - = rotate left (y axis)
	//    + = rotate right (y axis)
	//    < = rotate left (x axis)
	//    > = rotate right (x axis)
	// randomize the angle from 0.9 to 1.1: ex so 45 deg turn might become 40deg or 50 deg, but still
	// in the same general direction
	printf("chars:");
	for (auto c : language)
	{
		printf("%c ", c);
		
		if (isalpha(c))
		{
			vertices.push_back(currentPos);
			currentPos += currentDir * variableMap.at(c); // scales direction by the param value
			vertices.push_back(currentPos);
		}
		else
		{
			try
			{
				switch (c)
				{
				case '[':
					positionStack.push_back(std::pair < glm::vec3, glm::vec3 >(currentPos, currentDir));
					rotateDir(-variableMap.at(c),glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case ']':
					temp = positionStack.back();
					positionStack.pop_back();
					currentPos = temp.first; // retrieves saved pos and direction from stack
					currentDir = temp.second;
					rotateDir(variableMap.at(c), glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				case '-':
					rotateDir(-variableMap.at(c), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				case '+':
					rotateDir(variableMap.at(c), glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				case '<':
					rotateDir(-variableMap.at(c), glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				case '>':
					rotateDir(variableMap.at(c), glm::vec3(1.0f, 0.0f, 0.0f));
					break;

				}
			}
			catch (const std::exception&)
			{
				printf("%c not found in variableMap\n", c);
			}
		}
	}
	printf("\n");
}

void Tree::updateMinMaxCoordinates(float x, float y, float z) {

}

void Tree::shiftAndResizeSphere() {

}