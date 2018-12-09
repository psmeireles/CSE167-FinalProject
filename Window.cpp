#include "window.h"

const char* window_title = "GLFW Starter Project";
Geometry *sphere, *redPoint, *greenPoint;
Cube *cube;
Curve *curves[8], *nbCurves[8];
Terrain *terrain;
Transform *world, *boundingSpheres, *anchorTranslations[8], *controlTranslations[16],
*sphereTranslation, *sphereScale, *boundTranslation, *boundScale;
BoundingSphere *boundingSphere;
GLint Window::objShader, Window::cubeShader, colorShader, terrainShader, treeShader;

// Default camera parameters
glm::vec3 Window::camPos(0.0f, 0.0f, 200.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;
float Window::yaw = 0, Window::pitch = 0;
float Window::fov;
float Window::ratio;
float Window::nearDist, Window::farDist, Window::hNear, Window::wNear, Window::hFar, Window::wFar;
glm::vec3 Window::fc, Window::fbl, Window::ftl, Window::fbr, Window::ftr,
	Window::nc, Window::nbl, Window::ntl, Window::nbr, Window::ntr;
std::vector<glm::vec3> Window::planesNormals, Window::planesPoints;
bool Window::culling = true;

bool firstMouse = true;
bool Window::movement = false;
bool toggleModel = true;
int Window::normalColor = 0;
int eyeDir = 0;
int clapCount = 0;
int signal = 1;
int clapSignal = 1;
bool first = true;
bool debugMode = false;

glm::vec2 Window::lastPoint = glm::vec2(0.0f, 0.0f);
glm::mat4 Window::P;
glm::mat4 Window::V;

float lastTime = 0.0f;
int terrainLength = 513;

//////

std::vector<char> variables = { '0', '1' , '[', ']'};
std::vector<GLfloat> params = { 1.0f, 1.0f, 45.0f, 45.0f};
std::string initString = "0";
std::unordered_map<char, std::string> ruleMap({ {'1', "11"},{'0', "1[0]0"} });
LSystem * system1 = new LSystem(variables, params, initString, ruleMap);
std::string result = system1->generateString(3);


Tree * tree1;
glm::vec3 startPos(0.0f, 0.0f, 180.0f);

void Window::initialize_objects()
{
	printf("\nLSystemTest:");
	printf(result.c_str());
	printf("\n\n");
	
	// Load the shader program. Make sure you have the correct filepath up top
	objShader = LoadShaders("../shader.vert", "../shader.frag");
	cubeShader = LoadShaders("../cubeShader.vert", "../cubeShader.frag");
	colorShader = LoadShaders("../colorShader.vert", "../colorShader.frag");
	terrainShader = LoadShaders("../terrainShader.vert", "../terrainShader.frag");
	treeShader = LoadShaders("../treeShader.vert", "../treeShader.frag");

	std::string result2 = system1->generateString(3);
	printf("\nLSystemTest:");
	printf(result2.c_str());
	printf("\n\n");

	sphere = new Geometry("../obj/sphere.obj", colorShader, glm::vec3(1.0f, 0.0f, 0.0f));
	boundingSphere = new BoundingSphere("../obj/sphere.obj", colorShader, glm::vec3(1,1,1));
	/*redPoint = new Geometry("../obj/sphere.obj", colorShader, glm::vec3(1.0f, 0.0f, 0.0f));
	greenPoint = new Geometry("../obj/sphere.obj", colorShader, glm::vec3(0.0f, 1.0f, 0.0f));
	*/
	world = new Transform(glm::mat4(1.0f));
	
	sphereTranslation = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0, 10, 180)));
	sphereScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 5.0f, 5.0f)));
	boundScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)));
	sphereScale->addChild(sphere);
	boundScale->addChild(boundingSphere);
	sphereTranslation->addChild(sphereScale);
	sphereTranslation->addChild(boundScale);
	cube = new Cube();

	terrain = new Terrain(terrainLength, terrainShader, "../textures/grass.ppm");

	startPos = glm::vec3(0, terrain->map[terrainLength/2][150+terrainLength / 2] - 2, 150);
	tree1 = new Tree(treeShader, system1, startPos);

	world->addChild(cube);
	world->addChild(sphereTranslation);
	world->addChild(terrain);
	world->radius = 9999999;

	world->addChild(tree1);

}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(sphere);
	delete(cube);
	glDeleteProgram(objShader);
	glDeleteProgram(cubeShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = height;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		glm::vec3 d = glm::normalize(cam_look_at - camPos);
		glm::vec3 right = glm::normalize(glm::cross(d, cam_up));
		fov = glm::radians(45.0f);
		ratio = (float)width / (float)height;
		nearDist = 0.1f;
		farDist = 1000.0f;
		hNear = 2 * tan(fov / 2.0f) * nearDist;
		wNear = hNear * ratio;
		hFar = 2 * tan(fov / 2.0f) * farDist;
		wFar = hFar * ratio;

		fc = camPos + d * farDist;

		ftl = fc + (cam_up * (hFar / 2.0f)) - (right * (wFar / 2.0f));
		ftr = ftl + right * wFar;
		fbl = ftl - cam_up * hFar;
		fbr = fbl + right * wFar;

		nc = camPos + d * nearDist;

		ntl = nc + (cam_up * (hNear / 2.0f)) - (right * (wNear / 2.0f));
		ntr = ntl + right * wNear;
		nbl = ntl - cam_up * hNear;
		nbr = nbl + right * wNear;

		planesPoints.push_back(nc);		// near
		planesPoints.push_back(fc);		// far
		planesPoints.push_back(fbr);	// right
		planesPoints.push_back(fbl);	// left
		planesPoints.push_back(nbl);	// bottom
		planesPoints.push_back(ntl);	// top

		planesNormals.push_back(glm::normalize(glm::cross(ntl - nbl, nbr - nbl)));	// near
		planesNormals.push_back(glm::normalize(glm::cross(fbr - fbl, ftl - fbl)));	// far
		planesNormals.push_back(glm::normalize(glm::cross(ntr - nbr, fbr - nbr)));	// right
		planesNormals.push_back(glm::normalize(glm::cross(fbl - nbl, ntl - nbl)));	// left
		planesNormals.push_back(glm::normalize(glm::cross(nbr - nbl, fbl - nbl)));	// bottom
		planesNormals.push_back(glm::normalize(glm::cross(ntl - ntr, ftr - ntr)));	// top

		P = glm::perspective(fov, ratio, nearDist, farDist);
		V = glm::lookAt(camPos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	
	//float velocity = 0.0f;
	//if (sphereIsRiding) {
	//	velocity = curvesLength / (8*3);
	//}
	float now = glfwGetTime();
	float deltaT = now - lastTime;
	lastTime = now;
	//distance += velocity * deltaT;
	////printf("%f\n", distance);
	//int nextPointIndex;
	//glm::vec3 newSpherePos;
	//if (sphereIsRiding) {
	//	nextPointIndex = curves[sphereCurve]->getNextPointIndex(spherePoint, &distance);
	//	while (nextPointIndex == -1) {
	//		sphereCurve = (sphereCurve + 1) % 8;
	//		spherePoint = 0;
	//		nextPointIndex = curves[sphereCurve]->getNextPointIndex(spherePoint, &distance);
	//	}
	//}
	//else {
	//	nextPointIndex = spherePoint;
	//}

	//glm::vec3 pointBefore = curves[sphereCurve]->getPoint(nextPointIndex);
	//glm::vec3 pointAfter;
	//if (nextPointIndex == 150) {
	//	pointAfter = curves[sphereCurve+1]->getPoint(1);
	//}
	//else {
	//	pointAfter = curves[sphereCurve]->getPoint(nextPointIndex + 1);
	//}
	//float adjacentPointsD = glm::abs(glm::distance(pointAfter, pointBefore));
	//newSpherePos = (pointBefore*(adjacentPointsD - distance) + pointAfter * distance) / adjacentPointsD;

	////newSpherePos = curves[sphereCurve]->getPoint(nextPointIndex);
	//spherePoint = nextPointIndex;
	//sphereTranslation->M = glm::translate(sphereTranslation->M, -lastSpherePos);
	//sphereTranslation->M = glm::translate(sphereTranslation->M, newSpherePos);
	//lastSpherePos = newSpherePos;
	//

	///*glm::vec3 newSpherePos = curves[sphereMovCounter / 151]->getPoint(sphereMovCounter % 151);
	//sphereTranslation->M = glm::translate(sphereTranslation->M, -lastSpherePos);
	//sphereTranslation->M = glm::translate(sphereTranslation->M, newSpherePos);
	//lastSpherePos = newSpherePos;
	//sphereMovCounter = (sphereMovCounter + 1)%(8*151);*/
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world->draw(objShader, Window::V);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	
	int modifier = mods == GLFW_MOD_SHIFT ? -1 : 1;	
	float now = glfwGetTime();
	float deltaT = now - lastTime;
	lastTime = now;
	float cameraSpeed = 100.0f*deltaT;
	glm::vec3 camDir = glm::normalize(cam_look_at - Window::camPos);
	if (action = GLFW_PRESS) {
		switch (key) {
			// Check if escape was pressed
		case(GLFW_KEY_ESCAPE):
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		case GLFW_KEY_N:
			Window::normalColor = (Window::normalColor + 1) % 2;
			break;
		case GLFW_KEY_TAB:
			Window::culling = !Window::culling;
			break;
		case GLFW_KEY_1:
			if (debugMode == false)
				P = glm::perspective(glm::radians(80.0f), ratio, nearDist, farDist);
			else
				P = glm::perspective(fov, ratio, nearDist, farDist);

			debugMode = !debugMode;
		case GLFW_KEY_W:
			moveCamera(camDir, camDir, cameraSpeed);
			break;
		case GLFW_KEY_A:
			moveCamera(-glm::normalize(glm::cross(camDir, cam_up)), camDir, cameraSpeed);
			break;
		case GLFW_KEY_S:
			moveCamera(-camDir, camDir, cameraSpeed);
			break;
		case GLFW_KEY_D:
			moveCamera(glm::normalize(glm::cross(camDir, cam_up)), camDir, cameraSpeed);
			break;
		case GLFW_KEY_C:
			BoundingSphere::debugMode = !BoundingSphere::debugMode;
			break;
		}
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			movement = true;

			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			lastPoint = glm::vec2(xpos, ypos);
		}
		else if (action == GLFW_RELEASE) {
			movement = false;
		}
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (toggleModel) {
		world->scale(yoffset > 0.0f ? 1.1 : 0.9);
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec3 direction;
	float rot_angle;
	glm::vec3 curPoint;
	glm::vec2 point = glm::vec2(xpos, ypos);
	
	if (firstMouse)
	{
		lastPoint.x = xpos;
		lastPoint.y = ypos;
		firstMouse = false;
	}
	if (movement) {

		float xoffset = xpos - lastPoint.x;
		float yoffset = lastPoint.y - ypos;
		lastPoint.x = xpos;
		lastPoint.y = ypos;

		float sensitivity = 0.1;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cam_look_at = camPos + glm::normalize(front);

		glm::vec3 d = glm::normalize(cam_look_at - camPos);
		glm::vec3 right = glm::normalize(glm::cross(d, cam_up));

		fc = camPos + d * farDist;

		ftl = fc + (cam_up * (hFar / 2.0f)) - (right * (wFar / 2.0f));
		ftr = ftl + right * wFar;
		fbl = ftl - cam_up * hFar;
		fbr = fbl + right * wFar;

		nc = camPos + d * nearDist;

		ntl = nc + (cam_up * (hNear / 2.0f)) - (right * (wNear / 2.0f));
		ntr = ntl + right * wNear;
		nbl = ntl - cam_up * hNear;
		nbr = nbl + right * wNear;

		Window::V = glm::lookAt(camPos, cam_look_at, cam_up);

	}
}

glm::vec3 Window::trackBallMapping(glm::vec2 point)
{
	glm::vec3 v;
	float d;
	v.x = (2.0*point.x - width) / width;
	v.y = (height - 2.0*point.y) / height;
	v.z = 0;
	d = v.length();
	d = (d < 1.0) ? d : sqrt(0.5);
	v.z = camPos.z;
	v.x /= v.length();
	v.y /= v.length();
	v.z /= v.length();
	return v;
}

float Window::dist(glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3 point) {
	float dist = glm::dot(point - planePoint, planeNormal);

	return dist;
}

void Window::moveCamera(glm::vec3 movementDir, glm::vec3 camDir, float speed) {
	float eyeY = 0.0f;
	float xLerpFactor = 0.0;
	float zLerpFactor = 0.0;
	int xIndex, zIndex;
	int heightOffset = 10;
	float lastY = Window::camPos.y;

	movementDir.y = 0;
	movementDir = glm::normalize(movementDir);
	Window::camPos += speed * movementDir;
	xLerpFactor = Window::camPos.x - (int)Window::camPos.x;
	zLerpFactor = Window::camPos.z - (int)Window::camPos.z;
	xIndex = (int)Window::camPos.x + terrainLength / 2;
	zIndex = (int)Window::camPos.z + terrainLength / 2;
	eyeY = (1 - zLerpFactor)*((1 - xLerpFactor)*terrain->map[xIndex][zIndex] + xLerpFactor * terrain->map[xIndex + 1][zIndex])
		+ zLerpFactor * ((1 - xLerpFactor)*terrain->map[xIndex][zIndex + 1] + xLerpFactor * terrain->map[xIndex + 1][zIndex + 1]);
	if (abs(eyeY + heightOffset - lastY) > 1) {
		Window::camPos.y = eyeY + heightOffset;
	}
	cam_look_at = Window::camPos + camDir;
	Window::V = glm::lookAt(Window::camPos, cam_look_at, cam_up);
}