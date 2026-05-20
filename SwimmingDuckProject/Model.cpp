#include "Model.h"

void Model::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Model::Draw(Shader& shader)
{
	shader.use();
	shader.setMat4("model", model);
	if (useTexture)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture);
		shader.setInt("colorTexture", 1);
		shader.setInt("useTexture", 1);
	}
	else
		shader.setInt("useTexture", 0);
	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Model::loadModel(std::string path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open the file\n";
		return;
	}

	int vertexCount;
	file >> vertexCount;  // read integer directly

	std::cout << vertexCount << ".\n";
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> vertexTexCoords;

	for (int i = 0; i < vertexCount; i++)
	{
		float x, y, z;
		Vertex newVertex;
		file >> x >> y >> z;  // read 3 floats
		newVertex.position = glm::vec3(x, y, z);
		file >> x >> y >> z;  // read 3 floats
		newVertex.normal = glm::vec3(x, y, z);
		file >> x >> y;  // read 2 floats
		newVertex.texCoords = glm::vec2(x, y);
		vertices.push_back(newVertex);
	}

	int triangleCount;
	file >> triangleCount;
	std::cout << triangleCount << ".\n";

	for (int i = 0; i < triangleCount; i++)
	{
		int a, b, c;
		file >> a >> b >> c;
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	file.close();
	setupMesh();
}

Model::Model()
{
	;
}
Model::Model(std::string path)
{
	loadModel(path);
}

void Model::Plane(float size, glm::mat4 position)
{
	vertices.clear();
	indices.clear();

	model = position;
	size /= 2.0f;
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	vertex.position = glm::vec3(-size, 0.0f, -size);
	vertex.texCoords = glm::vec2(0.0f, 0.0f);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, -size);
	vertex.texCoords = glm::vec2(1.0f, 0.0f);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(-size, 0.0f, size);
	vertex.texCoords = glm::vec2(0.0f, 1.0f);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, size);
	vertex.texCoords = glm::vec2(1.0f, 1.0f);
	vertices.push_back(vertex);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(2);

	setupMesh();
}

void Model::Cylinder(float radius, float length, glm::mat4 position)
{
	vertices.clear();
	indices.clear();

	model = position;
	for (size_t i = 0; i < 24; i++)
	{
		float x = radius * std::sin(2.0f * 3.14159f * i / 24.0f);
		float z = radius * std::cos(2.0f * 3.14159f * i / 24.0f);
		Vertex newVertex;
		newVertex.position = glm::vec3(x, -length / 2.0f, z);
		newVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
		vertices.push_back(newVertex);
		newVertex.position.y = length / 2.0f;
		vertices.push_back(newVertex);
		indices.push_back(i * 2);
		indices.push_back((i + 1) % 24 * 2);
		indices.push_back(i * 2 + 1);
		indices.push_back(i * 2 + 1);
		indices.push_back((i + 1) % 24 * 2);
		indices.push_back((i + 1) % 24 * 2 + 1);
	}
	Vertex bottomCenter;
	bottomCenter.position = glm::vec3(0.0f, -length / 2.0f, 0.0f);
	bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
	Vertex topCenter;
	topCenter.position = glm::vec3(0.0f, length / 2.0f, 0.0f);
	topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	size_t baseIndex = vertices.size();

	// centers
	vertices.push_back(bottomCenter); // baseIndex
	vertices.push_back(topCenter);    // baseIndex + 1

	// ring
	for (size_t i = 0; i < 24; i++)
	{
		float x = radius * std::sin(2.0f * 3.14159f * i / 24.0f);
		float z = radius * std::cos(2.0f * 3.14159f * i / 24.0f);

		Vertex v;

		v.position = glm::vec3(x, -length / 2.0f, z);
		v.normal = glm::vec3(0, -1, 0);
		vertices.push_back(v); // baseIndex + 2 + i*2

		v.position = glm::vec3(x, length / 2.0f, z);
		v.normal = glm::vec3(0, 1, 0);
		vertices.push_back(v); // baseIndex + 3 + i*2
	}
	for (size_t i = 0; i < 24; i++)
	{
		size_t curr = baseIndex + 2 + i * 2;
		size_t next = baseIndex + 2 + ((i + 1) % 24) * 2;

		// bottom cap (flip winding if needed)
		indices.push_back(baseIndex);
		indices.push_back(next);
		indices.push_back(curr);

		// top cap
		indices.push_back(baseIndex + 1);
		indices.push_back(curr + 1);
		indices.push_back(next + 1);
	}
	setupMesh();
}

void Model::Room(float size, glm::mat4 position)
{
	model = position;
	size /= 2.0f;

	vertices.clear();
	indices.clear();

	Vertex v;

	// ---- FRONT (+Z)
	v.normal = glm::vec3(0, 0, -1);
	v.position = glm::vec3(-size, size, size); vertices.push_back(v);
	v.position = glm::vec3(size, size, size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, size); vertices.push_back(v);
	v.position = glm::vec3(-size, -size, size); vertices.push_back(v);

	// ---- BACK (-Z)
	v.normal = glm::vec3(0, 0, 1);
	v.position = glm::vec3(size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(-size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(-size, -size, -size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, -size); vertices.push_back(v);

	// ---- LEFT (-X)
	v.normal = glm::vec3(1, 0, 0);
	v.position = glm::vec3(-size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(-size, size, size); vertices.push_back(v);
	v.position = glm::vec3(-size, -size, size); vertices.push_back(v);
	v.position = glm::vec3(-size, -size, -size); vertices.push_back(v);

	// ---- RIGHT (+X)
	v.normal = glm::vec3(-1, 0, 0);
	v.position = glm::vec3(size, size, size); vertices.push_back(v);
	v.position = glm::vec3(size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, -size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, size); vertices.push_back(v);

	// ---- TOP (+Y)
	v.normal = glm::vec3(0, -1, 0);
	v.position = glm::vec3(-size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(size, size, -size); vertices.push_back(v);
	v.position = glm::vec3(size, size, size); vertices.push_back(v);
	v.position = glm::vec3(-size, size, size); vertices.push_back(v);

	// ---- BOTTOM (-Y)
	v.normal = glm::vec3(0, 1, 0);
	v.position = glm::vec3(-size, -size, size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, size); vertices.push_back(v);
	v.position = glm::vec3(size, -size, -size); vertices.push_back(v);
	v.position = glm::vec3(-size, -size, -size); vertices.push_back(v);

	// ---- INDICES (2 triangles per face)
	for (int i = 0; i < 6; i++)
	{
		int base = i * 4;

		indices.push_back(base + 0);
		indices.push_back(base + 1);
		indices.push_back(base + 2);

		indices.push_back(base + 0);
		indices.push_back(base + 2);
		indices.push_back(base + 3);
	}

	setupMesh();
}

void Model::Duck(glm::mat4 position)
{
	model = position;
	loadModel("Models/duck.txt");
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Models/ducktex.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
	useTexture = true;
}