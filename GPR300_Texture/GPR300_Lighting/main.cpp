#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/EwMath.h"
#include "EW/Camera.h"
#include "EW/Mesh.h"
#include "EW/Transform.h"
#include "EW/ShapeGen.h"

#include "EW/Material.h"
#include "EW/LightSystem.h"

const float COLR = 22.5f;
const float ROTA = 45.0f;
const float TOWA = -60.0f;

void processInput(GLFWwindow* window);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

Material matter;
system512 syst;

GLint setTo;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;
const float CAMERA_MOVE_SPEED = 5.0f;
const float CAMERA_ZOOM_SPEED = 3.0f;

const char* SAND = "Um.png";
const char* STONES = "Umu.png";

int WIDTH = 8192;
int HEIGHT = 8192;
int CHANNELS = 3;

unsigned int sandyTexture;
unsigned int brickTexture;

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

glm::vec3 bgColor = glm::vec3(0);
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);

bool wireFrame = false;

std::string curnt;

glm::vec3 colorWheel(float col)
{
	float pul = col - floor(col);
	return glm::vec3(abs(pul * 6. - 3.) - 1., 2. - abs(pul * 6. - 2.), 2. - abs(pul * 6. - 4.));
}

float glRand()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float randPos(float ri)
{
	return ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (2.0f * ri)) - ri;
}

GLuint createTexture(const char* filePath)
{
	GLuint text;
	glGenTextures(1, &text);
	glBindTexture(GL_TEXTURE_2D, text);

	stbi_set_flip_vertically_on_load(true);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* textureData = stbi_load(filePath, &WIDTH, &HEIGHT, &CHANNELS, 0);

	switch (CHANNELS)
	{
		case 1:
			setTo = GL_R;
			break;
		case 2:
			setTo = GL_RG;
			break;
		case 3:
			setTo = GL_RGB;
			break;
		case 4:
			setTo = GL_RGBA;
			break;
		default:
			setTo = NULL;
			break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, setTo, WIDTH, HEIGHT, 0, setTo, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	
	//glBindTexture(GL_TEXTURE_2D, textureData);
	return text;
}

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lighting", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	//Used to draw shapes. This is the shader you will be completing.
	Shader litShader("shaders/defaultLit.vert", "shaders/defaultLit.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	ew::MeshData cubeMeshData;
	ew::createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	ew::MeshData sphereMeshData;
	ew::createSphere(0.5f, 64, sphereMeshData);
	ew::MeshData cylinderMeshData;
	ew::createCylinder(1.0f, 0.5f, 64, cylinderMeshData);
	ew::MeshData planeMeshData;
	ew::createPlane(1.0f, 1.0f, planeMeshData);

	ew::Mesh cubeMesh(&cubeMeshData);
	ew::Mesh sphereMesh(&sphereMeshData);
	ew::Mesh planeMesh(&planeMeshData);
	ew::Mesh cylinderMesh(&cylinderMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Initialize shape transforms
	ew::Transform cubeTransform;
	ew::Transform sphereTransform;
	ew::Transform planeTransform;
	ew::Transform cylinderTransform;
	ew::Transform lightTransform[syst.NUM];
	ew::Transform cLightTransform[syst.NUM];

	cubeTransform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
	sphereTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);

	planeTransform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	planeTransform.scale = glm::vec3(10.0f);

	cylinderTransform.position = glm::vec3(2.0f, 0.0f, 0.0f);

	matter.color = glm::vec3(1.0, 1.0, 1.0);
	matter.ambientK = 0.5;
	matter.diffuseK = 0.5;
	matter.specularK = 0.5;
	matter.shininess = 1;

	sandyTexture = createTexture(SAND);
	brickTexture = createTexture(STONES);

	

	for (int i = 0; i < syst.NUM; i++)
	{
		syst.dLights[i].color = glm::vec3(glRand(), glRand(), glRand());
		syst.dLights[i].direction = glm::normalize(glm::vec3(glRand(), glRand(), glRand()));
		syst.dLights[i].intensity = glRand();
		syst.pLights[i].color = glm::vec3(glRand(), glRand(), glRand());
		syst.pLights[i].intensity = glRand();
		syst.pLights[i].position = glm::vec3(randPos(12), randPos(12), randPos(12));
		syst.pLights[i].radius = 20;
		syst.cLights[i].color = glm::vec3(glRand(), glRand(), glRand());
		syst.cLights[i].direction = glm::normalize(glm::vec3(glRand(), glRand(), glRand()));
		syst.cLights[i].fallOff = 2;
		syst.cLights[i].intensity = 0.5;
		syst.cLights[i].maxAngle = 60;
		syst.cLights[i].minAngle = 30;
		syst.cLights[i].position = glm::vec3(randPos(12), randPos(12), randPos(12));
		syst.cLights[i].range = 16;
		lightTransform[i].scale = glm::vec3(0.5f);
		lightTransform[i].position = syst.pLights[i].position;
		cLightTransform[i].scale = glm::vec3(0.5f);
		cLightTransform[i].position = syst.cLights[i].position;
		cLightTransform[i].rotation = syst.cLights[i].direction;

		//glm::radians(22.5 * i)
	}

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sandyTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		//Draw
		litShader.use();
		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setVec3("material._Color", matter.color);
		litShader.setFloat("material._AmbientK", matter.ambientK);
		litShader.setFloat("material._DiffuseK", matter.diffuseK);
		litShader.setFloat("material._SpecularK", matter.specularK);
		litShader.setFloat("material._Shininess", matter.shininess);

		litShader.setInt("_SandTexture", 0);
		litShader.setInt("_BrickTexture", 1);

		//litShader.setVec3("_LightPos", lightTransform.position);
		//litShader.setVec3("_Light", lightColor);
		
		for (int i = 0; i < syst.NUM; i++)
		{
			curnt = std::to_string(i);
			litShader.setVec3("dLights[" + curnt + "].color", syst.dLights[i].color);
			litShader.setVec3("dLights[" + curnt + "].direction", syst.dLights[i].direction);
			litShader.setFloat("dLights[" + curnt + "].intensity", syst.dLights[i].intensity);
			litShader.setVec3("pLights[" + curnt + "].color", syst.pLights[i].color);
			litShader.setFloat("pLights[" + curnt + "].intensity", syst.pLights[i].intensity);
			litShader.setVec3("pLights[" + curnt + "].position", syst.pLights[i].position);
			litShader.setFloat("pLights[" + curnt + "].radius", syst.pLights[i].radius);
			litShader.setVec3("cLights[" + curnt + "].color", syst.cLights[i].color);
			litShader.setVec3("cLights[" + curnt + "].direction", syst.cLights[i].direction);
			litShader.setFloat("cLights[" + curnt + "].fallOff", syst.cLights[i].fallOff);
			litShader.setFloat("cLights[" + curnt + "].intensity", syst.cLights[i].intensity);
			litShader.setFloat("cLights[" + curnt + "].maxAngle", syst.cLights[i].maxAngle);
			litShader.setFloat("cLights[" + curnt + "].minAngle", syst.cLights[i].minAngle);
			litShader.setVec3("cLights[" + curnt + "].position", syst.cLights[i].position);
			litShader.setFloat("cLights[" + curnt + "].range", syst.cLights[i].range);
			lightTransform[i].scale = glm::vec3(0.5f);
			lightTransform[i].position = syst.pLights[i].position;
			cLightTransform[i].scale = glm::vec3(0.5f);
			cLightTransform[i].position = syst.cLights[i].position;
			cLightTransform[i].rotation = syst.cLights[i].direction;
		}

		//Draw cube
		litShader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		//Draw sphere
		litShader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		//Draw cylinder
		litShader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Draw plane
		litShader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		//Draw light as a small sphere using unlit shader, ironically.
		unlitShader.use();

		for (int i = 0; i < syst.NUM; i++)
		{
			lightTransform[i].scale = glm::vec3(0.5f);
			lightTransform[i].position = syst.pLights[i].position;
			cLightTransform[i].scale = glm::vec3(0.5f);
			cLightTransform[i].position = syst.cLights[i].position;
			cLightTransform[i].rotation = syst.cLights[i].direction;

			unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
			unlitShader.setMat4("_View", camera.getViewMatrix());
			unlitShader.setMat4("_Model", lightTransform[i].getModelMatrix());
			unlitShader.setVec3("_Color", syst.pLights[i].color * syst.pLights[i].intensity);
			sphereMesh.draw();

			unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
			unlitShader.setMat4("_View", camera.getViewMatrix());
			unlitShader.setMat4("_Model", cLightTransform[i].getModelMatrix());
			unlitShader.setVec3("_Color", syst.cLights[i].color * syst.pLights[i].intensity);
			cylinderMesh.draw();
		}

		

		//Draw UI
		ImGui::Begin("Material Settings");
		ImGui::ColorEdit3("Material Color", &matter.color.r);
		ImGui::SliderFloat("Ambient Coefficient", &matter.ambientK, 0.0, 1.0);
		ImGui::SliderFloat("Diffuse Coefficient", &matter.diffuseK, 0.0, 1.0);
		ImGui::SliderFloat("Specular Coefficient", &matter.specularK, 0.0, 1.0);
		ImGui::SliderFloat("Shininess", &matter.shininess, 1.0, 512.0);
		ImGui::End();

		ImGui::Begin("Directional Lights");
		for (int i = 0; i < syst.NUM; i++)
		{
			curnt = std::to_string(i + 1);
			ImGui::ColorEdit3(("Directional Light Color" + curnt).c_str(), &syst.dLights[i].color.r);
			ImGui::SliderFloat3(("Directional Light Direction" + curnt).c_str(), &syst.dLights[i].direction.r, -1, 1);
			ImGui::SliderFloat(("Directional Light Intensity" + curnt).c_str(), &syst.dLights[i].intensity, 0, 1);
		}
		ImGui::End();

		ImGui::Begin("Point Lights");
		for (int i = 0; i < syst.NUM; i++)
		{
			curnt = std::to_string(i + 1);
			ImGui::ColorEdit3(("Point Light Color" + curnt).c_str(), &syst.pLights[i].color.r);
			ImGui::SliderFloat3(("Point Light Direction" + curnt).c_str(), &syst.pLights[i].position.r, -12, 12);
			ImGui::SliderFloat(("Point Light Radius" + curnt).c_str(), &syst.pLights[i].radius, 1, 12);
			ImGui::SliderFloat(("Point Light Intensity" + curnt).c_str(), &syst.pLights[i].intensity, 0, 1);
		}
		ImGui::End();

		ImGui::Begin("Cone Lights");
		for (int i = 0; i < syst.NUM; i++)
		{
			curnt = std::to_string(i + 1);
			ImGui::ColorEdit3(("Cone Light Color" + curnt).c_str(), &syst.cLights[i].color.r);
			ImGui::SliderFloat3(("Directional Light Direction" + curnt).c_str(), &syst.cLights[i].direction.r, -1, 1);
			ImGui::SliderFloat3(("Cone Light Direction" + curnt).c_str(), &syst.cLights[i].position.r, -12, 12);
			ImGui::SliderFloat(("Cone Light Radius" + curnt).c_str(), &syst.cLights[i].range, 1, 12);
			ImGui::SliderFloat(("Cone Light Intensity" + curnt).c_str(), &syst.cLights[i].intensity, 0, 1);
			ImGui::SliderFloat(("Cone Light Fall Off" + curnt).c_str(), &syst.cLights[i].fallOff, 0, 1);
			ImGui::SliderFloat(("Cone Light Min Angle" + curnt).c_str(), &syst.cLights[i].minAngle, 0, 30);
			ImGui::SliderFloat(("Cone Light Max Angle" + curnt).c_str(), &syst.cLights[i].maxAngle, 60, 120);
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
//Author: Eric Winebrenner
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);
	glViewport(0, 0, width, height);
}
//Author: Eric Winebrenner
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Reset camera
	if (keycode == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.setPosition(glm::vec3(0, 0, 5));
		camera.setYaw(-90.0f);
		camera.setPitch(0.0f);
		firstMouseInput = false;
	}
	if (keycode == GLFW_KEY_1 && action == GLFW_PRESS) {
		wireFrame = !wireFrame;
		glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
	}
}
//Author: Eric Winebrenner
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (abs(yoffset) > 0) {
		float fov = camera.getFov() - (float)yoffset * CAMERA_ZOOM_SPEED;
		camera.setFov(fov);
	}
}
//Author: Eric Winebrenner
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}
	if (!firstMouseInput) {
		prevMouseX = xpos;
		prevMouseY = ypos;
		firstMouseInput = true;
	}
	float yaw = camera.getYaw() + (float)(xpos - prevMouseX) * MOUSE_SENSITIVITY;
	camera.setYaw(yaw);
	float pitch = camera.getPitch() - (float)(ypos - prevMouseY) * MOUSE_SENSITIVITY;
	pitch = glm::clamp(pitch, -89.9f, 89.9f);
	camera.setPitch(pitch);
	prevMouseX = xpos;
	prevMouseY = ypos;
}
//Author: Eric Winebrenner
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//Toggle cursor lock
	if (button == MOUSE_TOGGLE_BUTTON && action == GLFW_PRESS) {
		int inputMode = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, inputMode);
		glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
	}
}

//Author: Eric Winebrenner
//Returns -1, 0, or 1 depending on keys held
float getAxis(GLFWwindow* window, int positiveKey, int negativeKey) {
	float axis = 0.0f;
	if (glfwGetKey(window, positiveKey)) {
		axis++;
	}
	if (glfwGetKey(window, negativeKey)) {
		axis--;
	}
	return axis;
}

//Author: Eric Winebrenner
//Get input every frame
void processInput(GLFWwindow* window) {

	float moveAmnt = CAMERA_MOVE_SPEED * deltaTime;

	//Get camera vectors
	glm::vec3 forward = camera.getForward();
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	glm::vec3 position = camera.getPosition();
	position += forward * getAxis(window, GLFW_KEY_W, GLFW_KEY_S) * moveAmnt;
	position += right * getAxis(window, GLFW_KEY_D, GLFW_KEY_A) * moveAmnt;
	position += up * getAxis(window, GLFW_KEY_Q, GLFW_KEY_E) * moveAmnt;
	camera.setPosition(position);
}
