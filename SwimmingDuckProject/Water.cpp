#include "Water.h"

Water::Water::Water()
{
    waterLevelA.resize(N * N);
	waterLevelB.resize(N * N);
    waterLevelC.resize(N * N);
    normalMap.resize(N * N * 4);
    InitializeSimulation();
	waterLevelB[10000] = 0.25f; // inicjalna fala
    waterMesh = Model();
	waterMesh.Plane(10.0f, glm::mat4(1.0f));
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        N,
        N,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
}

void Water::Water::Draw(Shader& shader)
{
    if ((rand() % 100) < 5)
    {
        int x = rand() % N;
        int y = rand() % N;

        AddDrop(x, y, 0.25f);
    }
    SimulateStep();
    SwapBuffers();
	ComputeNormals();
    shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader.setInt("normalMap", 0);
	shader.setInt("useNormalMap", 1);
    waterMesh.Draw(shader);
	shader.setInt("useNormalMap", 0);
}

void Water::SimulateStep()
{
    for (int i = 1; i < N - 1; i++)
    {
        for (int j = 1; j < N - 1; j++)
        {
            float neighbors =
                (*waterLevel2)[(i + 1) * N + j] +
                (*waterLevel2)[(i - 1) * N + j] +
                (*waterLevel2)[i * N + j + 1] +
                (*waterLevel2)[i * N + j - 1];

            (*waterLevel3)[i * N + j] =
                damping[i][j] *
                (
                    A * neighbors +
                    B * (*waterLevel2)[i * N + j] -
                    (*waterLevel1)[i * N + j]
                    );
        }
    }
}

void Water::InitializeSimulation()
{
    A = (c * c * dt * dt) / (h * h);
    B = 2.0f - 4.0f * A;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            (*waterLevel1)[i * N + j] = 0.0f;
            (*waterLevel2)[i * N + j] = 0.0f;
            (*waterLevel3)[i * N + j] = 0.0f;

            float x = i * h;
            float y = j * h;

            float l = std::min({
                x,
                y,
                2.0f - x,
                2.0f - y
                });

            damping[i][j] =
                0.95f * std::min(1.0f, l / 0.2f);
        }
    }
}

void Water::ComputeNormals()
{
    for (int i = 1; i < N - 1; i++)
    {
        for (int j = 1; j < N - 1; j++)
        {
            float dx = ((*waterLevel2)[(i + 1) * N + j] - (*waterLevel2)[(i - 1) * N + j]) / (2.0f * h);
            float dy = ((*waterLevel2)[i * N + j + 1] - (*waterLevel2)[i * N + j - 1]) / (2.0f * h);

            glm::vec3 n = glm::normalize(glm::vec3(-dx, 1.0f, -dy));

            // mapowanie [-1,1] -> [0,255]
            int idx = (i * N + j) * 4;
            normalMap[idx + 0] = (unsigned char)((n.x * 0.5f + 0.5f) * 255.0f);
            normalMap[idx + 1] = (unsigned char)((n.y * 0.5f + 0.5f) * 255.0f);
            normalMap[idx + 2] = (unsigned char)((n.z * 0.5f + 0.5f) * 255.0f);
            normalMap[idx + 3] = 255;
        }
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        N,
        N,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        normalMap.data()
    );
}

void Water::SwapBuffers()
{
    std::swap(waterLevel1, waterLevel2);
    std::swap(waterLevel2, waterLevel3);
}

void Water::AddDrop(int x, int y, float strength)
{
    if (x <= 1 || x >= N - 2) return;
    if (y <= 1 || y >= N - 2) return;

    (*waterLevel2)[x * N + y] += strength;
}