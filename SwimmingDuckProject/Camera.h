#ifndef CAMERA_H
#define CAMERA_H
#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera
{
public:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = -90.0f;
	float pitch = 0.0f;
	float zoom = 45.0f;

	int windowWidth;
	int windowHeight;

	Camera(int windowWidth = 800.0f, int windowHeight = 600.0f);
	glm::mat4 view();
	glm::mat4 projection();
};

#endif
