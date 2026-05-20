#ifndef LIGHT_H
#define LIGHT_H

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Camera.h"

class Light
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	int type;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

	Light(glm::vec3 _diffuse, glm::vec3 _specular);

	static Light DirectionalLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction);
	static Light PointLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position);
	static Light SpotLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, glm::vec3 _direction, float cutOff, float outerCutOff);
	void UpdateFlashlight(Camera camera);
};

#endif