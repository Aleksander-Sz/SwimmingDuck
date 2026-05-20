#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Light.h"
#include "../Model.h"
#include "../Water.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

#define WINDOW_WIDHT 1200
#define WINDOW_HEIGHT 800

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMovement = true;
Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.cameraPos += cameraSpeed * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.cameraPos -= cameraSpeed * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.cameraPos -= cameraSpeed * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.cameraPos += cameraSpeed * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.cameraPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.cameraPos.y -= cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMovement)
	{
		firstMovement = false;
		lastX = xpos;
		lastY = ypos;
	}
	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	camera.yaw += xOffset;
	camera.pitch -= yOffset;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom -= (float)yoffset;
	if (camera.zoom < 1.0f)
		camera.zoom = 1.0f;
	if (camera.zoom > 45.0f)
		camera.zoom = 45.0f;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDHT, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// 2. Center the window
	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);
	int windowWidth = WINDOW_WIDHT; // Your desired width
	int windowHeight = WINDOW_HEIGHT; // Your desired height

	glfwSetWindowPos(
		window,
		monitorX + (mode->width - windowWidth) / 2,
		monitorY + (mode->height - windowHeight) / 2
	);

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);
	camera = Camera(fbWidth, fbHeight);

	// Rendering commands here

	Shader ourShader("Shaders/VertexShader.glsl","Shaders/FragmentShader.glsl");
	ourShader.use();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	Water water;
	Model room;
	room.Room(8.0f, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	Model mirror;
	Model mirrorBack;
	Model cylinder;
	cylinder.Cylinder(0.3f, 3.0f, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, -1.0f)), 3.14159f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)));

	// Lighting Shader
	// point light cube
	Shader lightsShader("Shaders/LightsVertexShader.glsl","Shaders/LightsFragmentShader.glsl");
	lightsShader.use();
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	// directional light arrow
	// -----

	// Light properties
	Light light = Light::PointLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.3f), glm::vec3(2.0f, 1.0f, 0.0f));
	glm::vec3 targetPosition;
	glm::vec3 targetNormal(0.0f, 1.0f, 0.0f);
	float r = 0.5f;
	glm::mat4 circleTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), -0.785398163397448309615660845819875721f - 0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
	targetNormal = glm::vec3(circleTranslationMatrix * glm::vec4(targetNormal, 0.0f));
	mirror.Plane(1.5f, circleTranslationMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.07f, 0.0f)));
	mirrorBack.Plane(1.5f, circleTranslationMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.07f, 0.0f)) * glm::rotate(glm::mat4(1.0f), 3.14159f, glm::vec3(0.0f, 0.0f, 1.0f)));

	// Mirrored world matrix
	glm::mat4 mirroredWorldMatrix = circleTranslationMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)) * glm::inverse(circleTranslationMatrix);

	int frame = 0;
	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//rendering commands here
		ourShader.use();
		ourShader.setInt("useNormalMap", 0);
		glClearColor(0.0f, 0.1f, 0.0f, 1.0f);

		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		ourShader.setMat4("view", camera.view());
		ourShader.setMat4("projection", camera.projection());

		ourShader.setVec3( "light.ambient",     light.ambient);
		ourShader.setVec3( "light.diffuse",     light.diffuse);
		ourShader.setVec3( "light.specular",    light.specular);
		ourShader.setVec3( "light.position",    light.position);
		ourShader.setVec3( "light.direction",   light.direction);
		ourShader.setFloat("light.cutOff",      light.cutOff);
		ourShader.setFloat("light.outerCutOff", light.outerCutOff);
		ourShader.setInt(  "light.type",        light.type); // 0: directional light, 1: point light, 2: spotlight
		ourShader.setFloat("light.constant",    light.constant);
		ourShader.setFloat("light.linear",      light.linear);
		ourShader.setFloat("light.quadratic",   light.quadratic);

		ourShader.setMat4("model", glm::mat4(1.0f));
		ourShader.setVec3("viewPos", camera.cameraPos);
		//material properties
		ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setFloat("material.shininess", 32.0f);
		float now = glfwGetTime();

		// Rendering the scene normally
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);

		ourShader.setMat4("view", camera.view());

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		mirror.Draw(ourShader);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		room.Draw(ourShader);
		mirrorBack.Draw(ourShader);
		ourShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder.Draw(ourShader);

		water.Draw(ourShader);
	
		frame++;
		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}