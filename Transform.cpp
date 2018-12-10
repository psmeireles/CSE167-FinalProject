#include "Transform.h"

Transform::Transform(glm::mat4 transform)
{
	this->M = transform;
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

	for (std::list<Node*>::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
		(*it)->draw(shaderProgram, newC);
}

void Transform::update()
{

}
void Transform::update(glm::mat4 transform)
{
	this->M = transform;
}

void Transform::scale(double x)
{
	M = M * glm::scale(glm::mat4(1.0f), glm::vec3(x));
}