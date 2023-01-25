#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

struct Vec3
{
	float x, y, z;
};

struct Color
{
	float r, g, b, a;
};

struct Vertex
{
	Vec3 position;
	Color color;
};

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

//TODO: Vertex shader source code
const char* vertexShaderSource =
"#version 450 \n"
"layout(location = 0) in vec3 vPos; \n"
"layout(location = 1) in vec4 vColor; \n"
"out vec4 Color; \n"
"uniform float _Time; \n"
"void main(){ \n"
"Color = vColor; \n"
"gl_Position = vec4((vPos.x * cos(_Time)) - (vPos.z * sin(_Time)), .4 * sin(_Time) + vPos.y, (vPos.x * sin(_Time)) + (vPos.z * cos(_Time)), 1.0); \n"
"} \0";

//TODO: Fragment shader source code
const char* fragmentShaderSource = 
"#version 450 \n"
"out vec4 FragColor; \n"
"in vec4 Color; \n"
"uniform float _Time; \n"
"void main(){ \n"
"FragColor = vec4(abs(sin(_Time))*Color); \n"
"} \0";

//TODO: Vertex data array
const Vertex vertexData[] = {
	Vertex{Vec3{-0.3, +0.1, +0.0}, Color{1.0, 0.0, 0.0, 1.0}},
	Vertex{Vec3{+0.3, +0.1, +0.0}, Color{1.0, 0.0, 0.0, 1.0}},
	Vertex{Vec3{+0.0, +0.7, +0.0}, Color{1.0, 0.0, 0.0, 1.0}},
	Vertex{Vec3{+0.0, -0.5, +0.0}, Color{0.0, 0.0, 1.0, 1.0}},
	Vertex{Vec3{-0.3, +0.1, +0.0}, Color{0.0, 0.0, 1.0, 1.0}},
	Vertex{Vec3{-0.6, -0.5, +0.0}, Color{0.0, 0.0, 1.0, 1.0}},
	Vertex{Vec3{+0.0, -0.5, +0.0}, Color{0.0, 1.0, 0.0, 1.0}},
	Vertex{Vec3{+0.3, +0.1, +0.0}, Color{0.0, 1.0, 0.0, 1.0}},
	Vertex{Vec3{+0.6, -0.5, +0.0}, Color{0.0, 1.0, 0.0, 1.0}},
};

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLExample", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);


	//TODO: Create and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//TODO: Get vertex shader compilation status and output info log
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader : %s", infoLog);
	}
	
	//TODO: Create and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile fragment shader : %s", infoLog);
	}

	//TODO: Create shader program
	GLuint shaderProgram = glCreateProgram();

	//TODO: Attach vertex and fragment shaders to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//TODO: Link shader program
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to compile program : %s", infoLog);
	}

	//TODO: Delete vertex + fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//TODO: Create and bind Vertex Array Object (VAO)
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//TODO: Create and bind Vertex Buffer Object (VBO), fill with vertexData
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//TODO: Define vertex attribute layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float time = (float)glfwGetTime();

		//TODO:Use shader program
		glUseProgram(shaderProgram);
		GLint timeLocation = glGetUniformLocation(shaderProgram, "_Time");
		glUniform1f(timeLocation, time);
		
		//TODO: Draw triangle (3 indices!)
		glDrawArrays(GL_TRIANGLES, 0, 9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

