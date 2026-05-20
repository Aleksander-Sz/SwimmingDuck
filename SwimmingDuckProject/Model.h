#ifndef MODEL_H
#define MODEL_H

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Model
{
public:
	Model();
	Model(std::string path);
	void Draw(Shader& shader);
	void Plane(float size, glm::mat4 position);
	void Cylinder(float radius, float lenght, glm::mat4 position);
	void Room(float size, glm::mat4 position);
	void Duck(glm::mat4 position);
	glm::mat4 model = glm::mat4(1.0f);
private:
	// model data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void setupMesh();
	void loadModel(std::string path);
	unsigned int VAO, VBO, EBO;
	unsigned int texture;
	bool useTexture = false;
};
#endif
