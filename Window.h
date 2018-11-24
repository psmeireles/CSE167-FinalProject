#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Geometry.h"
#include "Cube.h"
#include "Curve.h"
#include "Terrain.h"
#include "shader.h"
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <cstdlib>

class Window
{
public:
	static int width;
	static int height;
	static float yaw, pitch;
	static float fov;
	static float ratio;
	static int normalColor;
	static bool movement;
	static glm::vec3 camPos;
	static glm::vec2 lastPoint;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static glm::vec3 trackBallMapping(glm::vec2 point);

	static float nearDist, farDist, hNear, wNear, hFar, wFar;
	static glm::vec3 fc, fbl, ftl, fbr, ftr, nc, nbl, ntl, nbr, ntr;
	static float dist(glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3 point);

	static std::vector<glm::vec3> planesNormals;
	static std::vector<glm::vec3> planesPoints;
	static bool culling;
	static GLint objShader, cubeShader;
};

#endif
