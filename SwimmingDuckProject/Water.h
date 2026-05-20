#pragma once

#include <algorithm>
#include <glm.hpp>
#include "Model.h"
#define N 256

class Water
{
public:
	Water();
	void Draw(Shader& shader);
private:
	std::vector<float> waterLevelA;
	std::vector<float> waterLevelB;
	std::vector<float> waterLevelC;
	std::vector<float>* waterLevel1 = &waterLevelA;
	std::vector<float>* waterLevel2 = &waterLevelB;
	std::vector<float>* waterLevel3 = &waterLevelC;
	float damping[N][N];
	float h = 2.0f / (N - 1);
	float c = 1.0f;
	float dt = 1.0f / N;
	float A;
	float B;
	void SimulateStep();
	void InitializeSimulation();
	std::vector<unsigned char> normalMap;
	void ComputeNormals();
	void SwapBuffers();
	void AddDrop(int x, int y, float strength);

	Model waterMesh;
	unsigned int texture;
};