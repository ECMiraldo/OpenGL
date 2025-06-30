#pragma once
#include "Camera.h"




void Camera::InicializeCamera(float fovDegrees, float WindowWidth, float WindowHeight, glm::vec3 pos, glm::vec3 target) {

	zoom = 5.0f;
	this->position = pos;
	this->position.z = zoom;
	this->target = target;
	glm::vec3 cameraFront = position - target;
	glm::vec3 cameraRight = glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 worldUp = -glm::cross(cameraFront, cameraRight);
	projection = glm::perspective(glm::radians(fovDegrees), WindowWidth / WindowHeight, 0.1f, 100.f);
	view = glm::lookAt(this->position, target, worldUp);

}
void Camera::Update() {

	view = glm::lookAt(glm::vec3(position.x, position.y, position.z), target, glm::vec3(0.0f, 1.0f, 0.0f));

}
void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {


	// assume os primeiros valores de x y 
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// calcular o offset do x e do y e guardar os valores mais recentes de x e y
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	// multiplicar os offsets pela sensibilidade do rato
	float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// calcular o yaw e o pitch
	yaw += xoffset;
	pitch += yoffset;

	//    // definir os valores limites do pitch 
	if (pitch > 90.0f)
		pitch = 90.0f;
	if (pitch < -90.0f)
		pitch = -90.0f;

	// alterar os valores da camera
	position.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * zoom;
	position.y = -sin(glm::radians(pitch)) * zoom;
	position.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * zoom;
	//position.x = position.x * cos(glm::radians(xoffset));
   // position.z = position.z * -sin(glm::radians(xoffset));




}
void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	// Se faz zoom in
	if (yoffset == 1) {
		// Incremento no zoom, varia com a distância da câmara
		zoom -= fabs(zoom) * 0.1f;
	}
	// Senão, se faz zoom out
	else if (yoffset == -1) {
		// Incremento no zoom, varia com a distância da câmara
		zoom += fabs(zoom) * 0.1f;
	}


	position.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * zoom;
	position.y = -sin(glm::radians(pitch)) * zoom;
	position.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * zoom;



	std::cout << "ZOOM = " << zoom << std::endl;
}

//construtor para alocar memoria
Camera* Camera::instance = nullptr;

//construtor para inicializar a camera
Camera* Camera::GetInstance() {

	return (!instance) ?
		instance = new Camera :
		instance;
}