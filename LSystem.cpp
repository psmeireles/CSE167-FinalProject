#include "LSystem.h"



LSystem::LSystem(std::vector<char> variables, std::vector<GLfloat> params, std::string initString, std::unordered_map<char, std::string> ruleMap)
{
	this->variables = variables;
	this->params = params;
	this->initString = initString;
	this->ruleMap = ruleMap;
}


LSystem::~LSystem()
{
}

std::string LSystem::generateString(GLint recursions)
{
	std::string finalString = initString;
	for (GLint iteration = 0; iteration < recursions; iteration++)
	{
		for (int i = finalString.length() - 1; i >= 0; i--)
		{
			char c = finalString[i];
			std::string val;
			try
			{
				// Replace the variable in the string using the rule
				val = ruleMap.at(c);
				finalString.replace(i, 1, val);
			}
			catch (const std::exception&) // throws exception if char is not found in string
			{
				//printf("%c not found in map\n",c);
			}
		}
	}

	return finalString;
}

std::vector<char> LSystem::getVariables()
{
	return this->variables;
}

std::vector<GLfloat> LSystem::getParams()
{
	return this->params;
}