#include "Light.h"

Light::Light(glm::vec3 _diffuse, glm::vec3 _specular)
{
	position = glm::vec3(0.0f);
	direction = glm::vec3(0.0f);
	cutOff = 0.0f;
	outerCutOff = 0.0f;
	type = 0;
	ambient = _diffuse * 0.2f;
	diffuse = _diffuse;
	specular = _specular;
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
}

Light Light::DirectionalLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction)
{
	Light light(_diffuse, _specular);
	light.type = 0;
	light.direction = _direction;
	return light;
}
Light Light::PointLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position)
{
	Light light(_diffuse, _specular);
	light.type = 1;
	light.position = _position;
	return light;
}
Light Light::SpotLight(glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, glm::vec3 _direction, float cutOff, float outerCutOff)
{
	Light light(_diffuse, _specular);
	light.type = 2;
	light.position = _position;
	light.direction = _direction;
	light.cutOff = cutOff;
	light.outerCutOff = outerCutOff;
	return light;
}
void Light::UpdateFlashlight(Camera camera)
{
	if (type != 2)
		return;
	position = camera.cameraPos;
	direction = camera.cameraFront;
}