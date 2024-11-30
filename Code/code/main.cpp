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

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
static 	bool firstMouse = false;

int ScreenSizeX = 1024;
int ScreenSizeY = 768;

float xpos;
float ypos;
float lastX;
float lastY;
const float sensitivity = 0.1f;

float one_third = 1./3.;
float two_third = 2./3.;

// View control
static float viewAzimuth = 2.5f;
static float viewPolar = 0.f;
static glm::vec3 eye_center = glm::vec3(500,0,500);;
static float cameraMovementSpeed = 0.f;
glm::float32 FoV = 60;
glm::float32 zNear = 0.1f;
glm::float32 zFar = 6000.0f;

Camera camera = Camera(eye_center,viewAzimuth,viewPolar,FoV,zNear,zFar,cameraMovementSpeed);
SkyBox b;

TextureLoader textureLoader;

GLuint voxelTexture;
GLuint frame_buffer_3D;
int voxel_scene_size = 128;

static bool saveDepth = true;

Bot save;

 void initialize_FBO() {
 	glGenFramebuffers(1, &frame_buffer_3D);

 	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_3D);

 	glGenTextures(1, &voxelTexture);
 	glBindTexture(GL_TEXTURE_2D_ARRAY, voxelTexture);

 	// Allocate 3D texture with proper parameters
 	glTexImage3D(GL_TEXTURE_2D_ARRAY,
		 0,                // level
		 GL_RGBA8,         // Internal format (sized format for color)
		 voxel_scene_size, voxel_scene_size, voxel_scene_size, // width, height, depth
		 0,                // border
		 GL_RGBA,          // format
		 GL_UNSIGNED_BYTE, // type
		 nullptr);         // initial data (null)

 	// Set texture parameters
 	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0);
 	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
 	glDrawBuffers(1, drawBuffers);
 	glReadBuffer(GL_NONE);

 	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
 		std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
 	}
 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// This function retrieves and stores the depth map of the default frame buffer
// or a particular frame buffer (indicated by FBO ID) to a PNG image.

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
	window = glfwCreateWindow(ScreenSizeX, ScreenSizeY, "Project", NULL, nullptr);
	//ScreenSizeX = glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	//ScreenSizeY = glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
	//window =   glfwCreateWindow(1024,768, "Project", nullptr	, nullptr);

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
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_3D);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	float deltaTime = 0.0f; // Time between current frame and last frame
	b.initialize(camera.get_position(), glm::vec3(3000, 3000, 3000),textureLoader);
	Object o;

	o.initialize(glm::vec3(0.,0.,0.), glm::vec3(100.,100.,100.),textureLoader); //glm::vec3(150, 150, 150)

	Bot bot;
	bot.initialize();

	//initialize_FBO();
 	glm::mat4 orthoProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, -100.f, 100.f);
 	GLubyte* sliceData = new GLubyte[voxel_scene_size * voxel_scene_size * 4]; // RGBA data per slice

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render the SkyBox

		float dist= length(abs(o.get_position()-camera.get_position()));
		if(dist<500) {
			o.set_scale(glm::vec3(100,100,100));
		}
		else if(dist>=500) {
			o.set_scale(max(glm::vec3(450-0.5*dist,450-0.5*dist,450-0.5*dist),glm::vec3(0.f,0.f,0.f)));
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

		if(saveDepth) {
			glBindFramebuffer(GL_FRAMEBUFFER,frame_buffer_3D);
			glBindTexture(GL_TEXTURE_3D, voxelTexture);

			glViewport(0,0,voxel_scene_size,voxel_scene_size);
			//voxelTexture
			for(int k=0;k<voxel_scene_size; ++k) {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0, k);

				o.render(camera.get_MVP(),voxel_scene_size,k,orthoProjection);
				glReadPixels(0, 0, voxel_scene_size, voxel_scene_size, GL_RGBA, GL_UNSIGNED_BYTE, sliceData);
				std::stringstream filename;
				filename << "frame_" << k << ".png";
				stbi_write_png(filename.str().c_str(), voxel_scene_size, voxel_scene_size, 4, sliceData, voxel_scene_size * 4);
				//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0, k);

				//save.saveTextureFrames(frame_buffer_3D,voxelTexture, voxel_scene_size, "niquetoi",k);

				//bot.render(camera.get_MVP(),voxel_scene_size,k);
				//b.render(camera.get_MVP(),voxel_scene_size,k);
				// Optional: Debug by filling with a specific color


			}
			saveDepth = false;
		}
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,ScreenSizeX,ScreenSizeX);

		bot.render(camera.get_MVP(),0,0);
		o.render(camera.get_MVP(),0,0, glm::mat4(0.));
		b.render(camera.get_MVP(),0,0);


		// Swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	b.cleanup();
	o.cleanup();
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
	if (key == GLFW_KEY_R && glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS) {
		std::cout << "Reset." << std::endl;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else {
		camera.key_callback(window, key, scancode, action, mode);
		b.move(camera.get_position());
	}
}
