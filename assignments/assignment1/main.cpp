#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/transform.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <hfLib/framebuffer.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;

//Create Camera and Camera Transform
ew::Camera camera;
ew::CameraController cameraController;

//Material
struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

//Post Process Parameters
struct PostProcess
{
	bool inverse = 0;										//Invert Colors
	int effectNumber = 0;									//Changing this number changes the current Post Processing Effect
	float blurAmount = 1.0;									//Change Power of Box Blur
	glm::vec3 kernelTop = glm::vec3(0);						//Top Row of Kernel Matrix
	glm::vec3 kernelCenter = glm::vec3(0.0, 1.0, 0.0);		//Center Row of Kernel Matrix
	glm::vec3 kernelBottom = glm::vec3(0);					//Bottom Row of Kernel Matrix

}postProcess;

//Creating Frame Buffer Quad
float quadVertices[] =
{
	// Coords	// texCoords
	-1.0f,  1.0f,	0.0f, 1.0f,
	-1.0f, -1.0f,	0.0f, 0.0f,
	 1.0f, -1.0f,	1.0f, 0.0f,

	-1.0f,	1.0f,	0.0f, 1.0f,
	 1.0f, -1.0f,	1.0f, 0.0f,
	 1.0f,	1.0f,	1.0f, 1.0f
};


int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);

	//Set Camera variables
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//Shader, Model, and Transform
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Model monkeyModel = ew::Model("assets/Suzanne.fbx");
	ew::Transform monkeyTransform;

	//Set Shader
	shader.use();

	//Textures
	GLuint monkeyTexture = ew::loadTexture("assets/monkey_color.jpg");
	GLuint monkeyNormal = ew::loadTexture("assets/monkey_normal.jpg");

	//Create Framebuffer Screen Quad
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//Framebuffer shader configuration
	GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
	ew::Shader screenShader = ew::Shader("assets/postProcess.vert", "assets/postProcess.frag");
	screenShader.use();
	screenShader.setInt("_ScreenTexture", 0);
	hfLib::Framebuffer framebuffer = hfLib::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);

	//Global OpenGL Variables
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Draw as Wireframe
	
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//Render First Pass
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//Use Model Shader
		shader.use();

		//Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, monkeyTexture);
		shader.setInt("_MainTex", 0);
		glActiveTexture(GL_TEXTURE1);		//todo fix!
		glBindTexture(GL_TEXTURE_2D, monkeyNormal);
		shader.setInt("_NormalTex", 1);
		shader.setMat4("_Model", glm::mat4(1.0f));
		shader.setVec3("_EyePos", camera.position);

		//Draw Scene
		//Camera Controller
		cameraController.move(window, &camera, deltaTime);
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		//Rotate model around Y axis
		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		//transform.modelMatrix() combines translation, rotation, and scale into a 4x4 model matrix
		shader.setMat4("_Model", monkeyTransform.modelMatrix());

		//Set Material Properties
		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);
		shader.setFloat("_Material.Shininess", material.Shininess);

		//Draw Mesh to Screen
		monkeyModel.draw();

		//Second Pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Post Processing Effects
		screenShader.use();
		glBindTextureUnit(0, (GLuint)framebuffer.colorBuffer);
		screenShader.setInt("_InverseOn",		postProcess.inverse);
		screenShader.setInt("_EffectNumber",	postProcess.effectNumber);
		screenShader.setFloat("_BlurAmount",	postProcess.blurAmount);
		screenShader.setVec3("_KernelTop",		postProcess.kernelTop);
		screenShader.setVec3("_KernelCenter",	postProcess.kernelCenter);
		screenShader.setVec3("_KernelBottom",	postProcess.kernelBottom);
		screenShader.setInt("_ColorBuffer",		0);	// Not working properly
		glBindTextureUnit(1, (GLuint)framebuffer.depthBuffer);
		screenShader.setInt("_DepthBuffer",		framebuffer.depthBuffer);	// Not working properly

		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
		camera->position = glm::vec3(0, 0, 5.0f);
		camera->target = glm::vec3(0);
		controller->yaw = controller->pitch = 0;
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Post Process")) {
		ImGui::Checkbox("Inverse Colors", &postProcess.inverse);
		static int e = 0;
		ImGui::RadioButton("Kernels", &e, 0);
		ImGui::RadioButton("Box Blur", &e, 1);
		ImGui::RadioButton("Gamma Correction", &e, 2);
		ImGui::RadioButton("sRGB Correction", &e, 3);

		if (e == 0) {
			postProcess.effectNumber = 0;
			if (ImGui::CollapsingHeader("Kernel Effects")) {
				static int k = 0;
				ImGui::RadioButton("None", &k, 0);
				ImGui::RadioButton("Sharpen", &k, 1);
				ImGui::RadioButton("Blur", &k, 2);
				ImGui::RadioButton("Emboss", &k, 3);
				ImGui::RadioButton("Outline", &k, 4);
				ImGui::RadioButton("Edge Detect", &k, 5);

				if (k == 0) {
					//Identity Kernel
					postProcess.kernelTop =		glm::vec3(0, 0, 0);
					postProcess.kernelCenter =	glm::vec3(0, 1, 0);
					postProcess.kernelBottom =	glm::vec3(0, 0, 0);
				}
				if (k == 1) {
					//Sharpen Kernel
					postProcess.kernelTop =		glm::vec3(0, -1, 0);
					postProcess.kernelCenter =	glm::vec3(-1, 5, -1);
					postProcess.kernelBottom =	glm::vec3(0, -1, 0);
				}
				if (k == 2) {
					//Blur Kernel
					postProcess.kernelTop =		glm::vec3(0.0625, 0.125, 0.0625);
					postProcess.kernelCenter =	glm::vec3(0.125,  0.25,  0.125);
					postProcess.kernelBottom =	glm::vec3(0.0625, 0.125, 0.0625);
				}
				if (k == 3) {
					//Emboss Kernel
					postProcess.kernelTop =		glm::vec3(-1, 1, -1);
					postProcess.kernelCenter =	glm::vec3(1,  1, 1);
					postProcess.kernelBottom =	glm::vec3(-1,   1, -1);
				}
				if (k == 4) {
					//Outline Kernel
					postProcess.kernelTop =		glm::vec3(-1, -1, -1);
					postProcess.kernelCenter =	glm::vec3(-1,  8, -1);
					postProcess.kernelBottom =	glm::vec3(-1, -1, -1);
				}
				if (k == 5) {
					//Edge Detect Kernel
					postProcess.kernelTop =		glm::vec3(1,  1, 1);
					postProcess.kernelCenter =	glm::vec3(1, -8, 1);
					postProcess.kernelBottom =	glm::vec3(1,  1, 1);
				}
			}
		}
		else if (e == 1) {
			postProcess.effectNumber = 1;
			ImGui::SliderFloat("Blur Amount", &postProcess.blurAmount, 0.0f, 5.0f);
		}
		else if (e == 2) {
			postProcess.effectNumber = 2;
		}
		else if (e == 3) {
			postProcess.effectNumber = 3;
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

