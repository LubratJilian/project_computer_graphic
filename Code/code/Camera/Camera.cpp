#include "Camera.h"

Camera::Camera() {
    Camera(glm::vec3(0,0,0),0,0,45,10.,1000.,10.);

}

Camera::Camera(glm::vec3 position, float azimuth, float polar, float FoV, float near, float far, float cameraMovementSpeed) {
    _Position = position;
    _Azimuth = azimuth;
    _Polar = polar;
    _FoV = FoV;
    _Near = near;
    _Far = far;
	_CameraMovementSpeed = cameraMovementSpeed;
	_Up = glm::vec3(0,1,0);
    calculate_vector();
}

void Camera::calculate_vector() {

	_LookAt.x=_Position.x+cos(_Azimuth)*cos(_Polar);
	_LookAt.y=_Position.y+sin(_Polar);
	_LookAt.z=_Position.z+sin(_Azimuth)*cos(_Polar);

	_Up.x=cos(_Azimuth)*sin(-_Polar);
	_Up.y=cos(-_Polar);
	_Up.z=sin(_Azimuth)*sin(-_Polar);

	//std::cout<<_LookAt.x<<","<<_LookAt.y<<","<<_LookAt.z<<"   "<<_Up.x<<","<<_Up.y<<","<<_Up.z<<std::endl;
}

void Camera::set_azimuth(float azimuth) {
    _Azimuth = azimuth;
    calculate_vector();
}

void Camera::set_polar(float polar){
    _Polar = polar;
    calculate_vector();
}

void Camera::set_position(glm::vec3 position) {
    _Position = position;
}

void Camera::set_up(glm::vec3 up) {
    _Up = up;
}

void Camera::set_speed(float cameraMovementSpeed) {
    _CameraMovementSpeed = cameraMovementSpeed;
}

glm::mat4 Camera::get_MVP() {
	return glm::perspective(glm::radians(_FoV), 4.0f / 3.0f, _Near, _Far) * glm::lookAt(_Position, _LookAt, _Up);
}

void Camera::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_W && action == GLFW_PRESS)|| glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
	{
		glm::vec3 move = _CameraMovementSpeed * (_LookAt - _Position);
		_LookAt += move;
		_Position += move;
	}

	if ((key == GLFW_KEY_S && action == GLFW_PRESS)||glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
	{
		glm::vec3 move = _CameraMovementSpeed * (_LookAt - _Position);
		_LookAt -= move;
		_Position -= move;
	}

	if ((key == GLFW_KEY_A && action == GLFW_PRESS)|| glfwGetKey(window,GLFW_KEY_A)== GLFW_PRESS)
	{
		glm::vec3 move = _CameraMovementSpeed * glm::cross((_LookAt - _Position),_Up);
		_LookAt -= move;
		_Position -= move;
	}

	if ((key == GLFW_KEY_D && action == GLFW_PRESS)|| glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
	{
		glm::vec3 move = _CameraMovementSpeed * glm::cross((_LookAt - _Position),_Up);
		_LookAt += move;
		_Position += move;
	}
}
void Camera::mouse_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_Azimuth += xoffset;
	_Polar += yoffset;
	if(_Polar > 89.0*3.14/180)
		_Polar = 89.0*3.14/180;
	if(_Polar < -89.0*3.14/180)
		_Polar = -89.0*3.14/180;
	calculate_vector();
}

