#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/ShapeGen.h"

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;

glm::vec3 bgColor = glm::vec3(0);
float orbitRadius = 5.0f;
float orbitSpeed = 2.0f;
float fieldOfView = 1.0;
float orthographicHeight = 2.0f;
bool isOrthographic = false;

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 getModelMatrix()
	{
		glm::mat4 posMat = glm::mat4(1);
		glm::mat4 rotMat = glm::mat4(0);
		glm::mat4 scaMat = glm::mat4(0);
		glm::mat4 retMat;

		posMat[3][0] = position.r;
		posMat[3][1] = position.g;
		posMat[3][2] = position.b;

		rotMat[0][0] = cos(rotation.b) * cos(rotation.g);
		rotMat[0][1] = sin(rotation.b) * cos(rotation.g);
		rotMat[0][2] = -sin(rotation.g);
		rotMat[1][0] = (-sin(rotation.b) * cos(rotation.r)) + (cos(rotation.b) * sin(rotation.g) * sin(rotation.r));
		rotMat[1][1] = (cos(rotation.b) * cos(rotation.r)) + (sin(rotation.b) * sin(rotation.g) * sin(rotation.r));
		rotMat[1][2] = cos(rotation.g) * sin(rotation.r);
		rotMat[2][0] = (sin(rotation.b) * sin(rotation.r)) + (cos(rotation.b) * sin(rotation.g) * cos(rotation.r));
		rotMat[2][1] = (-cos(rotation.b) * sin(rotation.r)) + (sin(rotation.b) * sin(rotation.g) * cos(rotation.r));
		rotMat[2][2] = cos(rotation.g) * cos(rotation.r);
		rotMat[3][3] = 1;

		scaMat[0][0] = scale.r;
		scaMat[1][1] = scale.g;
		scaMat[2][2] = scale.b;
		scaMat[3][3] = 1;

		return posMat * rotMat * scaMat;
	}
};

struct Camera
{
	glm::vec3 position;
	glm::vec3 target;
	float fOV;
	float orthographicSize;
	bool orthographic;

	float near = 0.001;
	float far = 1000;

	glm::vec3 getForward()
	{
		return target - position;
	}

	glm::vec3 getRight()
	{
		return glm::cross(getForward(), glm::vec3(0, 1, 0));
	}

	glm::vec3 getUp()
	{
		return glm::cross(getRight(), getForward());
	}

	glm::mat4 getViewMatrix()
	{
		glm::mat4 giveBack;
		glm::mat4 givePos = glm::mat4(1);

		giveBack[0][0] = getRight().r;
		giveBack[1][0] = getRight().g;
		giveBack[2][0] = getRight().b;
		giveBack[0][1] = getUp().r;
		giveBack[1][1] = getUp().g;
		giveBack[2][1] = getUp().b;
		giveBack[0][2] = getForward().r;
		giveBack[1][2] = getForward().g;
		giveBack[2][2] = getForward().b;
		giveBack[3][3] = 1;

		givePos[3][0] = -position.r;
		givePos[3][1] = -position.g;
		givePos[3][2] = -position.b;

		return giveBack * givePos;
	}

	float getAspect()
	{
		return (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	}

	glm::mat4 getProjectionMatrix()
	{
		if (orthographic)
			return ortho(orthographicSize, getAspect(), near, far);
		else
			return perspective(fOV, getAspect(), near, far);
	}

	glm::mat4 ortho(float height, float aspectRatio, float nearPlane, float farPlane)
	{
		glm::mat4 giveBack;

		//float t = SCREEN_HEIGHT / 2;
		//float b = -t;
		//float r = (float)(SCREEN_HEIGHT / 2) * aspectRatio;
		//float l = -r;

		float t = height / 2;
		float b = -t;
		float r = (float)(height / 2) * aspectRatio;
		float l = -r;
		
		giveBack[0][0] = 2.0f / (r - l);
		giveBack[1][1] = 2.0f / (t - b);
		giveBack[2][2] = -2.0f / (farPlane - nearPlane);
		giveBack[3][3] = 1;

		giveBack[3][0] = -(r + l) / (r - l);
		giveBack[3][1] = -(t + b) / (t - b);
		giveBack[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);

		return giveBack;
	}

	glm::mat4 perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		glm::mat4 giveBack;

		float c = tan(fov / 2.0f);

		giveBack[0][0] = 1.0f / (aspectRatio * c);
		giveBack[1][1] = 1.0f / c;
		giveBack[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		giveBack[3][2] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
		giveBack[2][3] = -1;
		giveBack[3][3] = 1;

		return giveBack;
	}
};

glm::mat4 m = glm::mat4(0);

const int NUM_CUBES = 8;

float tempo;

Transform transform[NUM_CUBES];
Transform transform2[NUM_CUBES];
Transform transform3[NUM_CUBES];
Transform transform4[NUM_CUBES];
Transform transform5[NUM_CUBES];
int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	Camera cam;

	MeshData cubeMeshData;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData);

	Mesh cubeMesh(&cubeMeshData);

	MeshData cubeMeshData2;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData2);

	Mesh cubeMesh2(&cubeMeshData2);

	MeshData cubeMeshData3;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData3);

	Mesh cubeMesh3(&cubeMeshData3);

	MeshData cubeMeshData4;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData4);

	Mesh cubeMesh4(&cubeMeshData4);

	MeshData cubeMeshData5;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData5);

	Mesh cubeMesh5(&cubeMeshData5);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	cam.position = glm::vec3(9, 0, 0);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//Draw
		shader.use();
		shader.setFloat("_Time", time);

		cam.fOV = fieldOfView;
		cam.orthographicSize = orthographicHeight;
		cam.orthographic = isOrthographic;

		shader.setMat4("_View", cam.getViewMatrix());
		shader.setMat4("_Projection", cam.getProjectionMatrix());

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			transform[i].position = glm::vec3(0, 0, 0);
			transform[i].scale = glm::vec3(.8, .8, .8);
			transform[i].rotation = glm::vec3(1, 0, 0);

			shader.setMat4("_Model", transform[i].getModelMatrix());
			cubeMesh.draw();
		}

		
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			transform2[i].position = glm::vec3(.3, 0, .4);
			transform2[i].scale = glm::vec3(.5, .5, .5);
			transform2[i].rotation = glm::vec3(1, -.4, 0);

			shader.setMat4("_Model", transform2[i].getModelMatrix());
			cubeMesh2.draw();
		}
		
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			transform2[i].position = glm::vec3(-.3, 0, .4);
			transform2[i].scale = glm::vec3(.5, .5, .5);
			transform2[i].rotation = glm::vec3(1, .4, 0);

			shader.setMat4("_Model", transform2[i].getModelMatrix());
			cubeMesh2.draw();
		}

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			transform2[i].position = glm::vec3(.7, 0, 0);
			transform2[i].scale = glm::vec3(.5, .2, .2);
			transform2[i].rotation = glm::vec3(1, 1.9, 0);

			shader.setMat4("_Model", transform2[i].getModelMatrix());
			cubeMesh2.draw();
		}

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			transform2[i].position = glm::vec3(-.7, 0, 0);
			transform2[i].scale = glm::vec3(.5, .2, .2);
			transform2[i].rotation = glm::vec3(1, -1.9, 0);

			shader.setMat4("_Model", transform2[i].getModelMatrix());
			cubeMesh2.draw();
		}

		//Draw UI
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Orbit Radius", &orbitRadius, 1.0f, 10.0f);
		ImGui::SliderFloat("Orbit Speed", &orbitSpeed, 0.0f, 10.0f);
		ImGui::SliderFloat("Field of View", &fieldOfView, 1.0f, 1000.0f);
		ImGui::SliderFloat("Orthographic Height", &orthographicHeight, 0.0f, 1000.0f);
		ImGui::Checkbox("Is Orthographic", &isOrthographic);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
		
		cam.position = glm::vec3(cos(tempo) * orbitRadius, 0, sin(tempo) * orbitRadius);

		tempo += deltaTime * orbitSpeed;
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}