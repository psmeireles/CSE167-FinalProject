#include "window.h"

const char* window_title = "GLFW Starter Project";
Geometry *redPoint, *greenPoint;
Cube *cube;
Curve *curves[8], *nbCurves[8];
Terrain *terrain;
Transform *world;
GLint Window::objShader, Window::cubeShader, colorShader, terrainShader, treeShader;

std::vector<BoundingSphere*> boundVols;

// Default camera parameters
glm::vec3 Window::camPos(0.0f, 0.0f, 10.0f);		// e  | Position of camera

//Testing water
Water * water;
Transform * waterTranslation;
Transform * waterScale;
Transform * waterRotation;

// Testing water
GLint waterShader;

glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

Transform *cameraScale, *cameraTranslate;
BoundingSphere *cameraBound;

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

bool first = true;
bool firstMouse = true;
bool Window::movement = false;
bool toggleModel = true;
int Window::normalColor = 0;

glm::vec2 Window::lastPoint = glm::vec2(0.0f, 0.0f);
glm::mat4 Window::P;
glm::mat4 Window::V;

float lastTime = 0.0f;
int terrainLength = 513;

//////

std::vector<char> variables = { '0', '1' , '[', ']', '-', '+', '<', '>', '^', '&'};
std::vector<GLfloat> params = { 2.0f, 1.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f};
std::string initString = "0";
std::unordered_map<char, std::string> ruleMap({ {'1', "11"},{'0', "1[^^^^<<0][^^>>0][&<<0]&&&>>0"} });
LSystem * system1 = new LSystem(variables, params, initString, ruleMap);

std::vector<char> variables2 = { '0', '1' , '[', ']', '-', '+', '<', '>', '^', '&'};
std::vector<GLfloat> params2 = { 2.0f, 1.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f};
std::string initString2 = "0";
std::unordered_map<char, std::string> ruleMap2({ {'1', "11"},{'0', "1^[[<0]&>0]&1[&<0]^0"} });
LSystem * system2 = new LSystem(variables2, params2, initString2, ruleMap2);

std::vector<char> variables3 = { '0', '1', '3', '[', ']', '-', '+', '<', '>', '^', '&'};
std::vector<GLfloat> params3 = { 2.0f, 1.0f, 1.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f};
std::string initString3 = "0";

std::unordered_map<char, std::string> ruleMap3({ {'1', "11"} , { '0', "1[^^<<0][^>>0][0][&<<0][&&>>0]" } });
LSystem * system3 = new LSystem(variables3, params3, initString3, ruleMap3);

Tree * tree1;

std::vector<Transform *> treeTransforms;
int maxTrees = 100;
glm::vec3 startPos(0.0f, 0.0f, 0.0f);

Transform * treeScale;
Transform * treeScale2;
Transform * treeScale3;
Tree * t;
Tree * t2;
Tree * t3;

void detectColision();
// I'll keep a list to know which objects are colliding with the player so that I can change colors when they stop colliding
std::set<BoundingSphere*> collidingObjs;

// Sound engine object
ISoundEngine* engine;
ISoundSource *movementSound, *greensleeves;

void Window::initialize_objects()
{
    
    //Play Greensleeves on loop
    engine = createIrrKlangDevice();
	movementSound = engine->addSoundSourceFromFile("../SoundEffects/Walking.wav");
	greensleeves = engine->addSoundSourceFromFile("../SoundEffects/Greensleeves.wav");
    
    //char * dir = getcwd(NULL, 0); // Platform-dependent, see reference link below
    //printf("Current dir: %s", dir);
    
	// Load the shader program. Make sure you have the correct filepath up top
	objShader = LoadShaders("../shader.vert", "../shader.frag");
	cubeShader = LoadShaders("../cubeShader.vert", "../cubeShader.frag");
	colorShader = LoadShaders("../colorShader.vert", "../colorShader.frag");
	terrainShader = LoadShaders("../terrainShader.vert", "../terrainShader.frag");
	treeShader = LoadShaders("../treeShader.vert", "../treeShader.frag");
	
    //Testing water
    waterShader = LoadShaders("../water.vert", "../water.frag");
    
    //Testing water
    water = new Water(0,0, waterShader);
    
	world = new Transform(glm::mat4(1.0f));
	
	cube = new Cube();

	terrain = new Terrain(terrainLength, terrainShader, "../textures/grass.ppm");

	startPos = glm::vec3(0.0f);

	t = new Tree(treeShader, system1, startPos, 0);
	t2 = new Tree(treeShader, system2, startPos, 1);
	t3 = new Tree(treeShader, system3, startPos, 2);
	treeScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
	treeScale2 = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
	treeScale3 = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
	treeScale->addChild(t);
	treeScale2->addChild(t2);
	treeScale3->addChild(t3);

	float maxRadius = t->radius;
	if (t2->radius > maxRadius) {
		maxRadius = t2->radius;
	}
	if (t3->radius > maxRadius) {
		maxRadius = t3->radius;
	}
	for (int i = 0; i < maxTrees; i++)
	{
		int xPos = rand() % terrainLength - terrainLength / 2;
		int zPos = rand() % terrainLength - terrainLength / 2;
		startPos = glm::vec3(xPos, terrain->map[xPos+terrainLength / 2][zPos + terrainLength / 2] - 2, zPos);
		Transform * t = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(xPos, terrain->map[xPos + terrainLength / 2][zPos + terrainLength / 2] + 18, zPos)));

		BoundingSphere *sphere = new BoundingSphere("../obj/sphere.obj", colorShader, glm::vec3(1.0f, 1.0f, 1.0f));
		Transform *boundScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(maxRadius*20)));
		boundScale->addChild(sphere);
		sphere->radius *= maxRadius * 20;
		sphere->center = t->M*glm::vec4(sphere->center, 1.0f);
		boundVols.push_back(sphere);
		t->addChild(boundScale);
		int num = rand() % 5;
		switch (num)
		{
		case 0: case 2:
			t->addChild(treeScale);
			break;
		case 1: case 3:
			t->addChild(treeScale2);
			break;
		case 4: 
			t->addChild(treeScale3);
			break;
		}
		treeTransforms.push_back(t);
		world->addChild(t);
	}

	world->addChild(cube);
	world->addChild(terrain);
	world->radius = 9999999;

	// Set initial eye view at terrain level
	glm::vec3 camDir = glm::normalize(cam_look_at - Window::camPos);
	cameraBound = new BoundingSphere("../obj/sphere.obj", colorShader, glm::vec3(1.0f));
	cameraScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(5.0f)));
	cameraTranslate = new Transform(glm::translate(glm::mat4(1.0f), camPos));
	cameraBound->radius *= 5.0f;
	cameraBound->center = camPos;
	cameraScale->addChild(cameraBound);
	cameraTranslate->addChild(cameraScale);
	world->addChild(cameraTranslate);
	boundVols.push_back(cameraBound);
	moveCamera(camDir, camDir, 0);
    
    // NOTE This doesn't work currently for some reason... can't get object to appear
    //Testing water
    waterTranslation = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, terrain->map[terrainLength / 2][terrainLength / 2] + 20,-10.0f)));
    //waterScale = new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)));
    //waterScale->addChild(water);
    //waterTranslation->addChild(waterScale);
	//waterRotation = new Transform(glm::rotate(glm::mat4(1.0f), 180.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
	//waterRotation->addChild(water);
    waterTranslation->addChild(water);
    //world->addChild(waterTranslation);

	engine->play2D(greensleeves, true);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
	glDeleteProgram(objShader);
	glDeleteProgram(cubeShader);
    
    glDeleteProgram(waterShader);
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
	float now = glfwGetTime();
	float deltaT = now - lastTime;
	lastTime = now;	
	detectColision();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world->draw(objShader, Window::V);
    
    
    
    
    //Testing water
    glUseProgram(waterShader);
    water->draw(waterShader);
    
    
    
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
	float cameraSpeed = 40.0f*deltaT;
	glm::vec3 camDir = glm::normalize(cam_look_at - Window::camPos);
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			// Check if escape was pressed
		case(GLFW_KEY_ESCAPE):
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		case GLFW_KEY_N:
			Window::normalColor = (Window::normalColor + 1) % 2;
			break;
		case GLFW_KEY_W:
			moveCamera(camDir, camDir, cameraSpeed);
			if(!engine->isCurrentlyPlaying(movementSound))
				engine->play2D(movementSound);
			break;
		case GLFW_KEY_A:
			moveCamera(-glm::normalize(glm::cross(camDir, cam_up)), camDir, cameraSpeed);
			if (!engine->isCurrentlyPlaying(movementSound))
				engine->play2D(movementSound);
			break;
		case GLFW_KEY_S:
			moveCamera(-camDir, camDir, cameraSpeed);
			if (!engine->isCurrentlyPlaying(movementSound))
				engine->play2D(movementSound);
			break;
		case GLFW_KEY_D:
			moveCamera(glm::normalize(glm::cross(camDir, cam_up)), camDir, cameraSpeed);
			if (!engine->isCurrentlyPlaying(movementSound))
				engine->play2D(movementSound);
			break;
		case GLFW_KEY_C:
			BoundingSphere::debugMode = !BoundingSphere::debugMode;
			break;
		case GLFW_KEY_T:
			if (mods == GLFW_MOD_SHIFT)
			{
				// Randomize the positions
				for (int i = 0; i < maxTrees; i++)
				{
					int xPos = rand() % terrainLength - terrainLength / 2;
					int zPos = rand() % terrainLength - terrainLength / 2;
					startPos = glm::vec3(xPos, terrain->map[xPos + terrainLength / 2][zPos + terrainLength / 2] + 18, zPos);
					treeTransforms[i]->update(glm::translate(glm::mat4(1.0f), glm::vec3(xPos, terrain->map[xPos + terrainLength / 2][zPos + terrainLength / 2] + 18, zPos)));
					boundVols[i]->center = startPos;
				}
			}
			else // Randomize the branches
			{

				t->updateBuffers();
				t2->updateBuffers();
				t3->updateBuffers();
			}
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W || key == GLFW_KEY_A ||
			key == GLFW_KEY_S || key == GLFW_KEY_D) {
			engine->stopAllSoundsOfSoundSource(movementSound);
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
	/*if (toggleModel) {
		world->scale(yoffset > 0.0f ? 1.1 : 0.9);
	}*/
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
	glm::vec3 lastPos = Window::camPos;

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

	// Updating bounding sphere
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), Window::camPos - lastPos);
	cameraTranslate->M = translation * cameraTranslate->M;
	cameraBound->center = translation * glm::vec4(cameraBound->center, 1.0f);
}

void detectColision() {
	int numCol = 0;
	for (int i = 0; i < boundVols.size(); i++) {
		BoundingSphere* obj1 = boundVols[i];
		for (int j = i + 1; j < boundVols.size(); j++) {
			BoundingSphere* obj2 = boundVols[j];
			if (obj1 != obj2) {
				if (glm::distance(obj1->center, obj2->center) < obj1->radius + obj2->radius) {
					numCol++;
					boundVols[i]->color = glm::vec3(1, 0, 0);
					boundVols[j]->color = glm::vec3(1, 0, 0);

					// check if camera is involved in collision and add object
					if (j == boundVols.size() - 1) {
						// check if object has already been added to colliding list
						if (std::find(collidingObjs.begin(), collidingObjs.end(), boundVols[i]) == collidingObjs.end()) {
							collidingObjs.insert(boundVols[i]);
						}
					}
				}
				else if (j == boundVols.size() - 1) {
					std::set<BoundingSphere*>::iterator it = std::find(collidingObjs.begin(), collidingObjs.end(), boundVols[i]);
					if (it != collidingObjs.end()) {
						collidingObjs.erase(it);
						boundVols[i]->color = glm::vec3(1.0f);
					}
				}
			}
		}
	}
	if (first) {
		printf("%d\n", numCol);
		first = false;
	}
	if (collidingObjs.size() == 0) {
		cameraBound->color = glm::vec3(1.0f);
	}
}
