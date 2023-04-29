#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/EwMath.h"
#include "EW/Camera.h"
#include "EW/ShadowCamera.h"
#include "EW/Mesh.h"
//#include "EW/PostProcess.h"
#include "EW/Transform.h"
#include "EW/ShapeGen.h"
#include "EW/ShadowToggle.h"
#include "EW/Material.h"

#include "EW/LightProperties.h"
#include "EW/LightSystem.h"

using namespace std;

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

LightProperties matter;
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
const char* MEME = "Meme.jpeg";
const char* NORM = "Normu.png";
const char* METAL = "Metal.png";
const char* METALLIC = "MetalNormal.png";
const char* MOSS = "Moss.png";
const char* MOSSY = "Mossy.png";
const char* ROX = "Rocks.jpg";

int WIDTH = 8192;
int HEIGHT = 8192;
int DEPTHWIDTH = 2048;
int DEPTHHEIGHT = 2048;
int CHANNELS = 3;

unsigned int sandyTexture;
unsigned int brickTexture;
unsigned int memeTexture;
unsigned int normTexture;
unsigned int metal;
unsigned int metalNormal;
unsigned int moss;
unsigned int mossNormal;
unsigned int rocks;

unsigned int textureColorBuffer;

unsigned int dBuffer;

unsigned int fbo;
unsigned int rbo;

unsigned int dfbo;

bool active;
bool wavy;
bool grey;
bool inverse;
bool blur;

bool showDebug;
glm::vec2 position;
float scale;

float frequency;
float amplitude;
float directs;
float strength;
float blurSize;
float minBias;
float maxBias;

GLuint fboVAO;

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
ShadowCamera sCamera((float)DEPTHWIDTH / (float)DEPTHHEIGHT);
ShadowToggle sToggle;

glm::vec3 bgColor = glm::vec3(0);
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);

bool wireFrame = false;

std::string curnt;

std::vector<const char*> items;
std::vector<ew::Material> mat;

ifstream stre;
string getAs;
const char* name;
float r;
float g;
float b;
glm::vec3 colo;
float amb;
float dif;
float spec;
float shin;
float norm;
string text1;
unsigned int tx1;
string text2;
unsigned int tx2;
string norTex;
unsigned int normap;

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

GLuint createBufferTexture()
{
	GLuint text;
	glGenTextures(1, &text);
	glBindTexture(GL_TEXTURE_2D, text);

	stbi_set_flip_vertically_on_load(true);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return text;
}

GLuint createDepthTexture()
{
	unsigned int texto;
	glGenTextures(1, &texto);
	glBindTexture(GL_TEXTURE_2D, texto);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTHWIDTH, DEPTHHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dfbo, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	return texto;
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

/*
GLuint createDepthBuffer()
{
	
}
*/

void loadMaterial(std::string filePath, Shader* single, Shader* doubl)
{
	stre.open(filePath);
	stre >> getAs >> r >> g >> b;
	colo = glm::vec3(r, g, b);
	stre >> amb >> dif >> spec >> shin >> norm >> text1 >> text2 >> norTex;
	stre.close();
	name = getAs.c_str();

	tx1 = createTexture(text1.c_str());

	normap = createTexture(norTex.c_str());

	if (text2 != "na")
	{
		tx2 = createTexture(text2.c_str());
		mat.push_back(ew::Material(doubl, colo, amb, dif, spec, shin, norm, tx1, tx2, normap));
	}
	else
	{
		mat.push_back(ew::Material(doubl, colo, amb, dif, spec, shin, norm, tx1, normap));
	}

	items.push_back(name);
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

	Shader monoLitShader("shaders/defaultLit.vert", "shaders/uni.frag");

	//Used for post processing
	Shader postProcessing("shaders/depth.vert", "shaders/depth.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	Shader shadowShader("shaders/shadow.vert", "shaders/shadow.frag");

	ew::MeshData cubeMeshData;
	ew::createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	ew::MeshData sphereMeshData;
	ew::createSphere(0.5f, 64, sphereMeshData);
	ew::MeshData cylinderMeshData;
	ew::createCylinder(1.0f, 0.5f, 64, cylinderMeshData);
	ew::MeshData planeMeshData;
	ew::createPlane(1.0f, 1.0f, planeMeshData);
	ew::MeshData quadMeshData;
	ew::createQuad(2.0f, 2.0f, quadMeshData);

	ew::Mesh cubeMesh(&cubeMeshData);
	ew::Mesh sphereMesh(&sphereMeshData);
	ew::Mesh planeMesh(&planeMeshData);
	ew::Mesh cylinderMesh(&cylinderMeshData);
	ew::Mesh quadMesh(&quadMeshData);

	//loadMaterial("Meme.txt", &monoLitShader, &litShader);
	//loadMaterial("Moss.txt", &monoLitShader, &litShader);

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
	ew::Transform quadTransform;
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
	minBias = 0.005;
	maxBias = 0.015;

	position = glm::vec2(0.8, 0.8);
	scale = 0.2;

	sandyTexture = createTexture(SAND);
	brickTexture = createTexture(STONES);
	memeTexture = createTexture(MEME);
	normTexture = createTexture(NORM);
	metal = createTexture(METAL);
	metalNormal = createTexture(METALLIC);
	moss = createTexture(MOSS);
	mossNormal = createTexture(MOSSY);
	rocks = createTexture(ROX);
	textureColorBuffer = createBufferTexture();

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, DEPTHWIDTH, DEPTHHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dBuffer, 0);

	/*glGenBuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer Error: " << fboStatus << std::endl;*/

	//ew::Material()

	unsigned int text = createDepthTexture();
	glGenFramebuffers(1, &dfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, text, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer Error: " << fboStatus << std::endl;
	
	for (int i = 0; i < 1; i++)
	{
		syst.dLights[i].color = glm::vec3(1, 1, 1);
		syst.dLights[i].direction = glm::normalize(glm::vec3(1, 0, 0));
		syst.dLights[i].intensity = 1;
		syst.pLights[i].color = glm::vec3(glRand(), glRand(), glRand());
		syst.pLights[i].intensity = glRand();
		syst.pLights[i].position = glm::vec3(randPos(12), randPos(12), randPos(12));
		syst.pLights[i].radius = 20;
		syst.cLights[i].color = glm::vec3(0, 0, 0);
		syst.cLights[i].direction = glm::normalize(glm::vec3(0, 0, 0));
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

	frequency = 8;
	amplitude = 0.05f;
	directs = 16;
	strength = 3;
	blurSize = 8;

	sToggle.backP = 20;
	sToggle.distance = 10;
	sToggle.frontP = 0.001f;
	sToggle.height = 10;
	sToggle.width = 10;

	mat.push_back(ew::Material(&monoLitShader, glm::vec3(1.0), 0.5f, 0.5f, 0.5f, 0, 0, memeTexture, metalNormal));
	items.push_back("Meme");
	mat.push_back(ew::Material(&monoLitShader, glm::vec3(1.0), 0.5f, 0.5f, 0.5f, 0, 1, moss, mossNormal));
	items.push_back("Moss");
	mat.push_back(ew::Material(&litShader, glm::vec3(1.0), 0.5f, 0.5f, 0.5f, 0, 1, moss, rocks, mossNormal));
	items.push_back("MossyRocks");
	mat.push_back(ew::Material(&monoLitShader, glm::vec3(1.0), 0.5f, 0.5f, 0.5f, 0, 1, sandyTexture, normTexture));
	items.push_back("Stones");
	mat.push_back(ew::Material(&litShader, glm::vec3(1.0), 0.5f, 0.5f, 0.5f, 0, 1, moss, sandyTexture, normTexture));
	items.push_back("MossyRocks");

	cubeMesh.matter = mat[0];
	cubeMesh.current = items[0];
	sphereMesh.matter = mat[0];
	sphereMesh.current = items[0];
	cylinderMesh.matter = mat[0];
	cylinderMesh.current = items[0];
	planeMesh.matter = mat[0];
	planeMesh.current = items[0];

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;
		
		glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
		glViewport(0, 0, DEPTHWIDTH, DEPTHHEIGHT);
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		sCamera.setForward(syst.dLights[0].safeDir());
		sCamera.setAspectRatio(sToggle.width / sToggle.height);
		sCamera.setOrthoSize(sToggle.height);
		sCamera.setDistance(sToggle.distance);
		sCamera.setNearPlane(sToggle.frontP);
		sCamera.setFarPlane(sToggle.backP);

		glCullFace(GL_FRONT);
		shadowShader.use();
		shadowShader.setMat4("_Projection", sCamera.getProjectionMatrix());
		shadowShader.setMat4("_View", sCamera.getViewMatrix());

		shadowShader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		//Draw sphere
		shadowShader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		//Draw cylinder
		shadowShader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Draw plane
		shadowShader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sandyTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normTexture);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, text);

		glCullFace(GL_BACK);

		

		//Draw
		litShader.use();
		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setMat4("_LightViewProjection", sCamera.getProjectionMatrix() * sCamera.getViewMatrix());
		litShader.setFloat("minBias", minBias);
		litShader.setFloat("maxBias", maxBias);

		litShader.setInt("_SandTexture", 0);
		litShader.setInt("_BrickTexture", 1);
		litShader.setInt("_MesoTexture", 2);
		litShader.setInt("_ShadowTexture", 3);

		monoLitShader.use();
		monoLitShader.setMat4("_Projection", camera.getProjectionMatrix());
		monoLitShader.setMat4("_View", camera.getViewMatrix());
		monoLitShader.setMat4("_LightViewProjection", sCamera.getProjectionMatrix() * sCamera.getViewMatrix());
		monoLitShader.setFloat("minBias", minBias);
		monoLitShader.setFloat("maxBias", maxBias);

		monoLitShader.setInt("_SandTexture", 0);
		monoLitShader.setInt("_BrickTexture", 1);
		monoLitShader.setInt("_MesoTexture", 2);
		monoLitShader.setInt("_ShadowTexture", 3);

		litShader.use();

		//litShader.setVec3("_LightPos", lightTransform.position);
		//litShader.setVec3("_Light", lightColor);
		
		for (int i = 0; i < syst.NUM; i++)
		{
			curnt = std::to_string(i);
			litShader.setVec3("dLights[" + curnt + "].color", syst.dLights[i].color);
			litShader.setVec3("dLights[" + curnt + "].direction", syst.dLights[i].safeDir());
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

			monoLitShader.setVec3("dLights[" + curnt + "].color", syst.dLights[i].color);
			monoLitShader.setVec3("dLights[" + curnt + "].direction", syst.dLights[i].safeDir());
			monoLitShader.setFloat("dLights[" + curnt + "].intensity", syst.dLights[i].intensity);
			monoLitShader.setVec3("pLights[" + curnt + "].color", syst.pLights[i].color);
			monoLitShader.setFloat("pLights[" + curnt + "].intensity", syst.pLights[i].intensity);
			monoLitShader.setVec3("pLights[" + curnt + "].position", syst.pLights[i].position);
			monoLitShader.setFloat("pLights[" + curnt + "].radius", syst.pLights[i].radius);
			monoLitShader.setVec3("cLights[" + curnt + "].color", syst.cLights[i].color);
			monoLitShader.setVec3("cLights[" + curnt + "].direction", syst.cLights[i].direction);
			monoLitShader.setFloat("cLights[" + curnt + "].fallOff", syst.cLights[i].fallOff);
			monoLitShader.setFloat("cLights[" + curnt + "].intensity", syst.cLights[i].intensity);
			monoLitShader.setFloat("cLights[" + curnt + "].maxAngle", syst.cLights[i].maxAngle);
			monoLitShader.setFloat("cLights[" + curnt + "].minAngle", syst.cLights[i].minAngle);
			monoLitShader.setVec3("cLights[" + curnt + "].position", syst.cLights[i].position);
			monoLitShader.setFloat("cLights[" + curnt + "].range", syst.cLights[i].range);
			lightTransform[i].scale = glm::vec3(0.5f);
			lightTransform[i].position = syst.pLights[i].position;
			cLightTransform[i].scale = glm::vec3(0.5f);
			cLightTransform[i].position = syst.cLights[i].position;
			cLightTransform[i].rotation = syst.cLights[i].direction;
		}


		
		//Draw cube
		//litShader.setMat4("_Model", cubeTransform.getModelMatrix());
		//cubeMesh.draw();
		cubeMesh.materialDraw(cubeTransform, text);

		//Draw sphere
		//litShader.setMat4("_Model", sphereTransform.getModelMatrix());
		//sphereMesh.draw();
		sphereMesh.materialDraw(sphereTransform, text);

		//Draw cylinder
		//litShader.setMat4("_Model", cylinderTransform.getModelMatrix());
		//cylinderMesh.draw();
		cylinderMesh.materialDraw(cylinderTransform, text);

		//monoLitShader.use();
		planeMesh.materialDraw(planeTransform, text);
		//Draw plane
		//litShader.setMat4("_Model", planeTransform.getModelMatrix());
		//planeMesh.draw();

		//postProcessing.use();
		//quadMesh.draw();

		//Draw light as a small sphere using unlit shader, ironically.
		unlitShader.use();
		

		

		for (int i = 0; i < syst.NUM; i++)
		{
			//lightTransform[i].scale = glm::vec3(0.5f);
			//lightTransform[i].position = syst.pLights[i].position;
			//cLightTransform[i].scale = glm::vec3(0.5f);
			//cLightTransform[i].position = syst.cLights[i].position;
			//cLightTransform[i].rotation = syst.cLights[i].direction;

			//unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
			//unlitShader.setMat4("_View", camera.getViewMatrix());
			//unlitShader.setMat4("_Model", lightTransform[i].getModelMatrix());
			//unlitShader.setVec3("_Color", syst.pLights[i].color * syst.pLights[i].intensity);
			//sphereMesh.draw();

			//unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
			//unlitShader.setMat4("_View", camera.getViewMatrix());
			//unlitShader.setMat4("_Model", cLightTransform[i].getModelMatrix());
			//unlitShader.setVec3("_Color", syst.cLights[i].color * syst.pLights[i].intensity);
			//cylinderMesh.draw();
		}

		/*
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		postProcessing.use();
		postProcessing.setFloat("time", time);
		postProcessing.setInt("processing", active);
		postProcessing.setInt("inverted", inverse);
		postProcessing.setInt("greyed", grey);
		postProcessing.setInt("waving", wavy);
		postProcessing.setInt("blurry", blur);
		postProcessing.setFloat("frequency", frequency);
		postProcessing.setFloat("amplitude", amplitude);
		postProcessing.setFloat("directs", directs);
		postProcessing.setFloat("strength", strength);
		postProcessing.setFloat("blurSize", blurSize);
		postProcessing.setFloat("width", SCREEN_WIDTH);
		postProcessing.setFloat("height", SCREEN_HEIGHT);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		postProcessing.setInt("screen", 5);
		*/
		
		if (showDebug)
		{
			postProcessing.use();
			postProcessing.setInt("_ShadowTexture", 3);
			postProcessing.setVec2("positionAt", position);
			postProcessing.setFloat("widthAt", scale);
			postProcessing.setFloat("heightAt", scale);
			quadMesh.draw();
		}

		//Draw UI
		/*
		ImGui::Begin("Material Settings");
		ImGui::ColorEdit3("Material Color", &matter.color.r);
		ImGui::SliderFloat("Ambient Coefficient", &matter.ambientK, 0.0, 1.0);
		ImGui::SliderFloat("Diffuse Coefficient", &matter.diffuseK, 0.0, 1.0);
		ImGui::SliderFloat("Specular Coefficient", &matter.specularK, 0.0, 1.0);
		ImGui::SliderFloat("Shininess", &matter.shininess, 1.0, 512.0);
		ImGui::SliderFloat("Normal Map Intensity", &matter.normalMapIntensity, 0.0, 1.0);
		
		ImGui::End();
		*/

		
		ImGui::Begin("Directional Lights");
		for (int i = 0; i < 1; i++)
		{
			curnt = std::to_string(i + 1);
			ImGui::ColorEdit3(("Directional Light Color" + curnt).c_str(), &syst.dLights[i].color.r);
			ImGui::SliderFloat3(("Directional Light Direction" + curnt).c_str(), &syst.dLights[i].direction.r, -1, 1);
			ImGui::SliderFloat(("Directional Light Intensity" + curnt).c_str(), &syst.dLights[i].intensity, 0, 1);
		}
		ImGui::End();
		
		/*
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
		*/

		/*
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
		*/

		/*ImGui::Begin("Post Processing");
		ImGui::Checkbox("Use Postprocessing", &active);
		ImGui::Checkbox("Inverted", &inverse);
		ImGui::Checkbox("Wavy", &wavy);
		ImGui::Checkbox("GreyScale", &grey);
		ImGui::Checkbox("Blurry", &blur);
		ImGui::SliderFloat("Amplitude", &amplitude, 1.0f, 16.0f);
		ImGui::SliderFloat("Frequency", &frequency, 0.05f, 1.0f);
		ImGui::SliderFloat("Directions", &directs, 1.0f, 20.0f);
		ImGui::SliderFloat("Quality", &strength, 1.0f, 20.0f);
		ImGui::SliderFloat("Blur Size", &blurSize, 1.0f, 16.0f);
		ImGui::End();*/
		
		ImGui::Begin("Shadow Rendering");
		ImGui::SliderFloat("Distance", &sToggle.distance, 1.0f, 16.0f);
		ImGui::SliderFloat("Front Plane", &sToggle.frontP, 0.001f, 1000.0f);
		ImGui::SliderFloat("Back Plane", &sToggle.backP, 0.001f, 1000.0f);
		ImGui::SliderFloat("Width", &sToggle.width, 1.0f, 50.0f);
		ImGui::SliderFloat("Height", &sToggle.height, 1.0f, 50.0f);
		ImGui::SliderFloat("Minimum Bias", &minBias, 0.005, 0.05);
		ImGui::SliderFloat("Maximum Bias", &maxBias, 0.005, 0.05);
		ImGui::Checkbox("Show Debug Map", &showDebug);
		ImGui::SliderFloat2("Debug Position", &position.x, -1, 1);
		ImGui::SliderFloat("Debug Scale", &scale, 0, 1);
		ImGui::End();

		ImGui::Begin("Cube Material Settings");
		if (ImGui::BeginCombo("click here", cubeMesh.current))
		{
			for (int i = 0; i < items.size(); i++)
			{
				bool selected = (cubeMesh.current == items[i]);
				if (ImGui::Selectable(items[i], selected))
				{
					cubeMesh.current = items[i];
					cubeMesh.matter = mat[i];
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::End();

		ImGui::Begin("Sphere Material Settings");
		if (ImGui::BeginCombo("click here", sphereMesh.current))
		{
			for (int i = 0; i < items.size(); i++)
			{
				bool selected = (sphereMesh.current == items[i]);
				if (ImGui::Selectable(items[i], selected))
				{
					sphereMesh.current = items[i];
					sphereMesh.matter = mat[i];
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::End();

		ImGui::Begin("Cylinder Material Settings");
		if (ImGui::BeginCombo("click here", cylinderMesh.current))
		{
			for (int i = 0; i < items.size(); i++)
			{
				bool selected = (cylinderMesh.current == items[i]);
				if (ImGui::Selectable(items[i], selected))
				{
					cylinderMesh.current = items[i];
					cylinderMesh.matter = mat[i];
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::End();

		ImGui::Begin("Plane Material Settings");
		if (ImGui::BeginCombo("click here", planeMesh.current)) 
		{
			for (int i = 0; i < items.size(); i++)
			{
				bool selected = (planeMesh.current == items[i]); 
				if (ImGui::Selectable(items[i], selected))
				{
					planeMesh.current = items[i];
					planeMesh.matter = mat[i];
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
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
