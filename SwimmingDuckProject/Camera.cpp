#include "Camera.h"

Camera::Camera(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}
glm::mat4 Camera::view()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
glm::mat4 Camera::projection()
{
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	return projection;
}