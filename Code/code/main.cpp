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
#include <Quad.h>

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
static glm::vec3 eye_center = glm::vec3(300,0,300);
static float cameraMovementSpeed = 0.f;
glm::float32 FoV = 60;
glm::float32 zNear = 0.1f;
glm::float32 zFar = 6000.0f;

Camera camera = Camera(eye_center,viewAzimuth,viewPolar,FoV,zNear,zFar,cameraMovementSpeed);
SkyBox skyBox;

TextureLoader textureLoader;

GLuint voxelTexture;
GLuint frame_buffer_3D;
int voxel_scene_size = 128;
int mipLevels = 4;

static bool saveVoxels = false;


 void initialize_FBO() {
 	glGenFramebuffers(1, &frame_buffer_3D);
 	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_3D);
 	glGenTextures(1, &voxelTexture);

 	glBindTexture(GL_TEXTURE_3D, voxelTexture);

 	int width = voxel_scene_size;
 	int height = voxel_scene_size;
 	for (int i = 0; i < 1; i++) {
 		glTexImage3D(GL_TEXTURE_3D,
		 i,                // level
		 GL_RGBA8,         // Internal format (sized format for color)
		 width, height, voxel_scene_size*6, // width, height, depth
		 0,                // border
		 GL_RGBA,          // format
		 GL_UNSIGNED_BYTE, // type
		 nullptr);
 		width = glm::max(1, (width / 2));
 		height = glm::max(1, (height / 2));
 	}
 	// Set texture parameters
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
 	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);

 	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
 	glDrawBuffers(1, drawBuffers);


 	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0);



 	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
 		std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
 	}
 	glBindFramebuffer(GL_FRAMEBUFFER, 0);

 	if (glIsTexture(voxelTexture) == GL_FALSE) {
 		std::cerr << "voxelTexture is not a valid texture!" << std::endl;
 	}
}


glm::mat4 getViewMatrixForFace(int face) {
 	switch (face) {
 		case 0: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)); // +X
 		case 1: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)); // -X
 		case 2: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));  // +Y
 		case 3: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, 1)); // -Y
 		case 4: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)); // +Z
 		case 5: return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)); // -Z
 	}
 	return glm::mat4(1.0f);
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

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	float deltaTime = 0.0f; // Time between current frame and last frame

	skyBox.initialize(camera.get_position(), glm::vec3(1400, 1400, 1400),textureLoader);

	Object o;
	o.initialize(glm::vec3(0.,0.,0.), glm::vec3(100.,100.,100.),textureLoader); //glm::vec3(150, 150, 150)

	Bot bot;
	bot.initialize();


 	std::vector<glm::vec3> aabb_isle = o.get_AABB();

 	std::cout<<aabb_isle[0].x<<","<<aabb_isle[0].y<<","<<aabb_isle[0].z<<std::endl;
 	std::cout<<aabb_isle[1].x<<","<<aabb_isle[1].y<<","<<aabb_isle[1].z<<std::endl;

 	initialize_FBO();






	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER,frame_buffer_3D);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, voxelTexture);


		glViewport(0, 0, voxel_scene_size, voxel_scene_size);

		//voxelTexture
		for(int c=0;c<6;c++){
			glm::mat4 orthoProjection = glm::ortho(aabb_isle[0].x*100.f, aabb_isle[1].x*100.f,aabb_isle[0].y*100.f, aabb_isle[1].y*100.f,aabb_isle[0].z*100.f, aabb_isle[1].z*100.f) * getViewMatrixForFace(c);   // Near, Far()
			for(int k=0;k<voxel_scene_size; ++k) {

				GLenum error = glGetError();

				if (error != GL_NO_ERROR) {
					std::cerr << "OpenGL error 7: " << std::to_string(error) << std::endl;
				}

				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, voxelTexture, 0, k+c*voxel_scene_size);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				o.render(camera.get_MVP(),voxel_scene_size,k,orthoProjection,c,voxelTexture,camera.get_position());
				//bot.render(camera.get_MVP(),voxel_scene_size,k);
				//b.render(camera.get_MVP(),voxel_scene_size,k);

				/*if(saveVoxels) {
					GLubyte* sliceData = new GLubyte[voxel_scene_size * voxel_scene_size * 4]; // RGBA data per slice
					glReadPixels(0, 0, voxel_scene_size, voxel_scene_size, GL_RGBA, GL_UNSIGNED_BYTE, sliceData);
					std::stringstream filename;
					filename << "side_"<<c <<"frame_" << k << ".png";
					stbi_write_png(filename.str().c_str(), voxel_scene_size, voxel_scene_size, 4, sliceData, voxel_scene_size * 4);
				}*/

			}
		}
		glGenerateMipmap(GL_TEXTURE_3D);
		saveVoxels = false;

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,ScreenSizeX,ScreenSizeX);

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

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

		float dist= length(abs(o.get_position()-camera.get_position()));

		if(dist<500) {
			o.set_scale(glm::vec3(100,100,100));
			if(camera.get_FoV()>10) {
				camera.set_FoV(0.16f*dist - 22.f);
			}
			if(camera.get_speedMultiply()>10) {
				camera.set_speedMultiply(0.33f*dist - 65.f);
			}
		}
		else if(dist>=500) {
			o.set_scale(max(glm::vec3(350-0.5*dist,350-0.5*dist,350-0.5*dist),glm::vec3(0.f,0.f,0.f)));
		}
		//std::cout<<dist<<std::endl;
		camera.set_speed(deltaTime);

		bot.update(currentTime);

		bot.render(camera.get_MVP(),0,0);
		o.render(camera.get_MVP(),0,0, glm::mat4(0.),0,voxelTexture,camera.get_position());
		skyBox.render(camera.get_MVP(),0,0);


		// Swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	skyBox.cleanup();
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
		skyBox.move(camera.get_position());
	}
}
