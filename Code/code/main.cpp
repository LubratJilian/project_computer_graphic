#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <math.h>

#include "render/shader.h"
#include "Camera/Camera.h"
#include <SkyBox.h>
#include <objloader.h>
#include"Bot.h"
#include <TextureLoader.h>
#include<Lights.h>
#include<tinygltf-2.9.3/stb_image_write.h>
#include <CloudsGenerator.h>
#include <NetworkLoader.h>
#include<StreetLamps.h>
#include<Material.h>

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
static 	bool firstMouse = true;

int ScreenSizeX;
int ScreenSizeY;

float xpos;
float ypos;
float lastX;
float lastY;
const float sensitivity = 0.1f;

float one_third = 1./3.;
float two_third = 2./3.;

// View control
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static glm::vec3 eye_center = glm::vec3(300,300,0);
static float cameraMovementSpeed = 0.f;
glm::float32 FoV = 60;
glm::float32 zNear = 0.1f;
glm::float32 zFar = 6000.0f;

Camera camera = Camera(eye_center,viewAzimuth,viewPolar,FoV,zNear,zFar,cameraMovementSpeed);
SkyBox skybox;
TextureLoader textureLoader;
NetworkLoader networkLoader;
glm::vec3 scale = glm::vec3(100,100,100);

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	int Screen_sizeX = 1024;
	int Screen_sizeY = 768;
	window =   glfwCreateWindow(Screen_sizeX,Screen_sizeY, "Project", nullptr	, nullptr);

	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	float deltaTime = 0.0f; // Time between current frame and last frame

	skybox.initialize(camera.get_position(), glm::vec3(3000, 3000, 3000),textureLoader);

	Material test = Material(1,1,1,100);

	std::vector<Object> elements;
	Material material_main_part = Material(0.5,0,1,100);
	Object main_part = Object(glm::vec3(0.,0.,0.), &scale,textureLoader,"main_part.obj","Rocks.jpg",material_main_part); //glm::vec3(150, 150, 150)
	Material material_crystal = Material(1,1,0.5,1);
	Object crystals = Object(glm::vec3(0.,0.,0.), &scale,textureLoader,"crystals.obj","Crystals.png",material_crystal); //glm::vec3(150, 150, 150)
	Material material_road = Material(0.5,0.5,1,1);
	Object road = Object(glm::vec3(0.,0.,0.), &scale,textureLoader,"road.obj","Road.jpg",material_road); //glm::vec3(150, 150, 150)
	Material material_buildings = Material(0.5,1,1,1);
	std::map<std::string,glm::vec3> buildingColors;
	buildingColors.insert({"Panel", glm::vec3(0,0,0.552)});
	buildingColors.insert({"Walls", glm::vec3(0.883,0.886,0.906)});
	buildingColors.insert({"material_1176.025", glm::vec3(0.192,0.268,0.552)});
	buildingColors.insert({"material_0", glm::vec3(1,0,0)});
	Object buildings = Object(glm::vec3(0.,0.,0.), &scale,textureLoader,"buildings.obj",buildingColors,material_buildings); //glm::vec3(150, 150, 150)

	elements.push_back(main_part);
	elements.push_back(crystals);
	elements.push_back(road);
	elements.push_back(buildings);


	Bot bot;
	bot.initialize(glm::vec3(0,500,0),&scale);

	Light l1= Light(glm::vec3(300,300,300),glm::vec3(1.,0.7,0.7),1000000,60,10,1000,glm::vec3(0,0,0),glm::vec3(0,1,0),SUN);
	Light l2= Light(glm::vec3(300,300,-300),glm::vec3(1.,0.7,0.7),100000,60,10,1000,glm::vec3(0,0,0),glm::vec3(0,1,0),SUN);
	Light l3= Light(glm::vec3(-300,300,300),glm::vec3(1.,0.7,0.7),100000,60,200,800,glm::vec3(0,0,0),glm::vec3(0,1,0),SUN);
	Light l4= Light(glm::vec3(-300,300,-300),glm::vec3(1.,0.7,0.7),100000,60,200,800,glm::vec3(0,0,0),glm::vec3(0,1,0),SUN);
	Light l5= Light(glm::vec3(0,300,0),glm::vec3(1.,0.7,0.7),100000,60,200,800,glm::vec3(0,0,0),glm::vec3(0,1,0),SUN);
	Lights lights = Lights(Screen_sizeX,Screen_sizeY,{l1,l2,l3,l4,l5});
	lights.put_data_buffer();

	StreetLamps streetLamps = StreetLamps(textureLoader, networkLoader,test,"streetlamps.obj","StreetLamp.png","../code/Objects/street_lamps.objoriented",&scale,true);

	Material material_clouds = Material(0,0,1,100);
	CloudsGenerator cloudsGenerator = CloudsGenerator(glm::vec3(0,0,0),glm::vec3(3000,100,3000),glm::vec3(50,20,50),material_clouds);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render the SkyBox

		float dist= length(abs(elements[0].get_position()-camera.get_position()));
		if(dist<500) {
			scale = glm::vec3(100,100,100);
		}
		else if(dist>=500) {
			scale = max(glm::vec3(350-0.5*dist,350-0.5*dist,350-0.5*dist),glm::vec3(0.f,0.f,0.f));
		}


		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		camera.set_speed(100.f*deltaTime);

		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Project | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		bot.update(currentTime);

		glBindFramebuffer(GL_FRAMEBUFFER,lights.get_Fbo());
		glViewport(0, 0, Screen_sizeX, Screen_sizeY);
		int i = 0;

		for(Light& light : lights.get_lights()) {
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights.get_shadows(), 0, i);
			glClear(GL_DEPTH_BUFFER_BIT);
			//bot.render(light.get_lightVP(glm::vec3(0,0,0),glm::vec3(0,1,0)));
			for(Object &o : elements) {
				o.render(light.get_lightVP(light.get_LookAt(),light.get_Up()),camera.get_position(),lights);
			}
			streetLamps.render(light.get_lightVP(glm::cross(light.get_Up(),glm::cross(light.get_Up(),light.get_LookAt())),light.get_Up()),camera.get_position(),lights);
			i++;
			switch(light.get_type()) {
				case SUN:
					glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights.get_shadows(), 0, i);
					glClear(GL_DEPTH_BUFFER_BIT);
					for(Object &o : elements) {
						o.render(light.get_lightVP(glm::cross(light.get_Up(),glm::cross(light.get_Up(),light.get_LookAt())),light.get_Up()),camera.get_position(),lights);
					}
					streetLamps.render(light.get_lightVP(glm::cross(light.get_Up(),glm::cross(light.get_Up(),light.get_LookAt())),light.get_Up()),camera.get_position(),lights);
					//bot.render(light.get_lightVP(glm::vec3(500,500,500),glm::vec3(0,1,0)));
					i++;
					break;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,Screen_sizeX,Screen_sizeY);

		bot.render(camera.get_MVP(),camera.get_position(),lights);
		for(Object &o : elements) {
			o.render(camera.get_MVP(),camera.get_position(),lights);
		}
		streetLamps.render(camera.get_MVP(),camera.get_position(),lights);
		skybox.render(camera.get_MVP());
		cloudsGenerator.renderClouds(camera.get_MVP(),camera.get_position(),lights);


		// Swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	skybox.cleanup();
	for(Object &o:elements) {
		o.cleanup();
	}
	bot.cleanup();
	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	float sensitivityX = 0.001;
	float sensitivityY = 0.001;
	xoffset *= sensitivityX;
	yoffset *= sensitivityY;
	camera.mouse_callback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	//std::cout<<cameraMovementSpeed<<std::endl;
	if (key == GLFW_KEY_R && glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS) {
		std::cout << "Reset." << std::endl;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else {
		camera.key_callback(window, key, scancode, action, mode);
		skybox.move(camera.get_position());
	}
}
