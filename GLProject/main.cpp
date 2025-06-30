#define GLEW_STATIC
#include <glew.h>
#include <GLFW\glfw3.h>
#include <glm/ext.hpp>
#include <vector>
#include <gl/GL.h>
#include <iostream>
#include "Model.h"
#include "Camera.h"
#include "LoadShaders.h"
#include "Luzes.h"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#define HEIGHT 600
#define WIDTH 800

using namespace biblioteca;


void Init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	
	// Descomentar para ativar o Face Culling
	glEnable(GL_CULL_FACE);
}

//permite a chamada do script para movimentar a camera pelo rato
void HandleMouse(GLFWwindow* window, double xpos, double ypos) {
	Camera::GetInstance()->mouseCallback(window, xpos, ypos);
}

//permite a chamada do script para efetuar zoom pelo scroll
void HandleScroll(GLFWwindow* window, double xoffset, double yoffset) {
	Camera::GetInstance()->scrollCallback(window, xoffset, yoffset);
}



int main(void) {
	GLFWwindow* window;
	Camera* camera;
	camera = camera->GetInstance();
	camera->InicializeCamera(45.0f, WIDTH, HEIGHT, glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 2.0f, 0.0));

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(800, 600, "Iron_Man", NULL, NULL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);




	if (!window) {
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	Init();
	glewExperimental = GL_TRUE;
	glfwSetCursorPosCallback(window, HandleMouse);
	glfwSetScrollCallback(window, HandleScroll);
	glfwSetKeyCallback(window, lights::OnkeyPress);
	glewInit(); 



	//Precisa ser depois do glewInit para funcionar 
	
	
	
	Model model = Model("Iron_Man.obj");
	//GLuint shaderProgram = model.sendModelData();
	
	
	
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lights::Lights(&model);

		model.Draw(glm::vec3(0,0,0),glm::vec3(0,0,0));
		model.Draw(glm::vec3(-2,-1,-5),glm::vec3(0,0,0));
		model.Draw(glm::vec3(2,-1, -5), glm::vec3(0,0,0));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	

	glfwTerminate();
	return 0;
}
