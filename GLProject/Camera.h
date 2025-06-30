#pragma once
#include <glew.h>
#include <GLFW\glfw3.h>
#include <glm/ext.hpp>
#include <vector>
#include <gl/GL.h>
#include <iostream>
#include <math.h>

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")




class Camera
{
	public: 
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 position;
		glm::vec3 target;
		double lastX, lastY, yaw, pitch;
		bool firstMouse;
	    float zoom;
		float angle;
	    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	    void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static Camera* GetInstance();
		void InicializeCamera(float fovDegrees, float WindowWidth, float WindowHeight, glm::vec3 pos, glm::vec3 target);
		Camera() { lastX = 0.0f; lastY = 0.0f; yaw = 0.0f; pitch = 0.0f; firstMouse = true; zoom = 5.0f; };
		void Update();
		
	private:
		static Camera* instance;

};







