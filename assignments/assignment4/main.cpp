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
#include <hfLib/scene.h>

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
ew::Camera shadowMapCamera;
ew::CameraController cameraController;
float lightCamDist = 4.0f;

//Textures
GLuint shadowMapTex;
GLuint monkeyTexture;
GLuint monkeyNormal;
GLuint concreteTexture;
GLuint concreteNormal;
std::vector<GLuint> monkeyTextures;
std::vector<GLuint> concreteTextures;

//Light uniforms
glm::vec3 lightDirection = glm::vec3(0.0, -1.0f, 0.0);
glm::vec3 lightColor = glm::vec3(1.0);

//Framebuffers
hfLib::Framebuffer framebuffer;
hfLib::Framebuffer shadowMapFramebuffer;
float minSlopeBias = 0.005f;
float maxSlopeBias = 0.01f;
hfLib::Framebuffer gBuffer;

//Create Scene
hfLib::Scene scene;

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

//Point Light
struct PointLight
{
	glm::vec3 position;
	glm::vec3 color;
	float radius;
	float intensity;
};

//Create array of Point Light colors
glm::vec3 red = { 1.0f, 0.0f, 0.0f };
glm::vec3 green = { 0.0f, 1.0f, 0.0f };
glm::vec3 blue = { 0.0f, 0.0f, 1.0f };
glm::vec3 yellow = { 1.0f, 1.0f, 0.0f };
glm::vec3 cyan = { 0.0f, 1.0f, 1.0f };
glm::vec3 magenta = { 1.0f, 0.0f, 1.0f };

glm::vec3 colors[6] = { red, green, blue, yellow, cyan, magenta };

const int MAX_POINT_LIGHTS = 1;
PointLight pointLights[MAX_POINT_LIGHTS];
float pointLightIntensity = 0.5f;

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);

	//Global OpenGL Variables
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	//glfwWindowHint(GLFW_SAMPLES, 4); //MSAA
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_DEPTH_TEST); //Depth testing
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//Draw as Wireframe

	//Lit Scene Shader
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	//Geometry Shader
	ew::Shader geometryShader = ew::Shader("assets/geometryPass.vert", "assets/geometryPass.frag");
	//Deferred Shader
	ew::Shader deferredShader = ew::Shader("assets/deferredLit.vert", "assets/deferredLit.frag");
	//Post Process Shader
	ew::Shader screenShader = ew::Shader("assets/postProcess.vert", "assets/postProcess.frag");
	//Shadow Map Shader
	ew::Shader shadowMapShader = ew::Shader("assets/depthOnly.vert", "assets/depthOnly.frag");
	//Light Orb Shader
	ew::Shader lightOrbShader = ew::Shader("assets/lightOrb.vert", "assets/lightOrb.frag");

	//Textures

	//Monkey Textures
	monkeyTexture = ew::loadTexture("assets/monkey_color.jpg");
	monkeyNormal = ew::loadTexture("assets/monkey_normal.jpg");
	monkeyTextures.push_back(monkeyTexture);
	monkeyTextures.push_back(monkeyNormal);

	//Concrete Textures
	concreteTexture = ew::loadTexture("assets/concrete_color.jpg");
	concreteNormal = ew::loadTexture("assets/concrete_normal.jpg");
	concreteTextures.push_back(concreteTexture);
	concreteTextures.push_back(concreteNormal);

	//Monkey Shader, Model, and Transform
	ew::Model monkeyModel = ew::Model("assets/Suzanne.fbx");
	ew::Transform monkeyTransform; //Array of Transforms

	monkeyTransform.position = glm::vec3(0, 0, 0);
	monkeyTransform.scale = glm::vec3(0.5f);
	scene.addAsset(hfLib::SceneAsset(monkeyModel, monkeyTransform, monkeyTextures));

	//Ground Plane Shader, Model, and Transform
	ew::Model planeModel = ew::Model(ew::Mesh(ew::createPlane(10, 10, 1)));
	ew::Transform planeTransform;
	planeTransform.position = glm::vec3(0, -1, 0);
	hfLib::SceneAsset planeAsset = hfLib::SceneAsset(planeModel, planeTransform, concreteTextures);
	scene.addAsset(planeAsset);

	//Light Orb Model
	ew::Mesh sphereMesh = ew::Mesh(ew::createSphere(1.0f, 8));
	ew::Transform lightSphereTransform[MAX_POINT_LIGHTS];

	//Set Point Light Variables
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		//Set point light positions over a grid
		pointLights[i].position = glm::vec3(i % 8 - 3.5, 0, i / 8 - 3.5);

		pointLights[i].intensity = pointLightIntensity;
		pointLights[i].color = colors[i % 6];
		pointLights[i].radius = 5.0f;

		lightSphereTransform[i].position = pointLights[i].position;
		lightSphereTransform[i].scale = glm::vec3(0.2f);
	}

	//Set Camera variables
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	//Shadow Map Cameras
	shadowMapCamera.aspectRatio = 1.0f;
	shadowMapCamera.fov = 60.0f;
	shadowMapCamera.nearPlane = 0.1f;
	shadowMapCamera.farPlane = 12.0f;
	shadowMapCamera.orthographic = true;
	shadowMapCamera.orthoHeight = 8.0f;

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
	screenShader.use();
	screenShader.setInt("_ScreenTexture", 0);
	framebuffer = hfLib::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);

	//Shadowmap Configuration
	shadowMapFramebuffer = hfLib::createFramebuffer(1024, 1024);

	//Geometry Buffer Configuration
	gBuffer = hfLib::createGBuffer(screenWidth, screenHeight);
	
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;
	
		//Update Camera
		cameraController.move(window, &camera, deltaTime);
		
		shadowMapCamera.position = scene.getAsset(0).getTransform().position - lightDirection * lightCamDist;
		glm::mat4 lightSpaceMatrix = shadowMapCamera.projectionMatrix() * shadowMapCamera.viewMatrix();

		//Enable Depth Testing
		glEnable(GL_DEPTH_TEST);

		//Shadowmap draw
        // Set the viewport settings
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFramebuffer.fbo);
		glViewport(0, 0, 1024, 1024);
		glClear(GL_DEPTH_BUFFER_BIT);
		//lCullFace(GL_FRONT);
		shadowMapShader.use();
		scene.draw(shadowMapShader, shadowMapCamera);
		shadowMapTex = shadowMapFramebuffer.depthBuffer;
		
		//Geometry Pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.fbo);
		glViewport(0, 0, gBuffer.width, gBuffer.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		geometryShader.use();
		scene.draw(geometryShader, camera);

		//Lighting Pass
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		glViewport(0, 0, framebuffer.width, framebuffer.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		deferredShader.use();

		deferredShader.setMat4("_LightViewProjection", lightSpaceMatrix);
		deferredShader.setVec3("_LightDirection", lightDirection);
		deferredShader.setVec3("_LightColor", lightColor);

		glBindTextureUnit(0, gBuffer.colorBuffers[0]);
		glBindTextureUnit(1, gBuffer.colorBuffers[1]);
		glBindTextureUnit(2, gBuffer.colorBuffers[2]);
		glBindTextureUnit(3, shadowMapTex);
		deferredShader.setInt("_ShadowMap", 3);

		// Custom Shader Uniforms
		deferredShader.setFloat("_MinSlopeBias", minSlopeBias);
		deferredShader.setFloat("_MaxSlopeBias", maxSlopeBias);
		deferredShader.setFloat("_Material.Ka", material.Ka);
		deferredShader.setFloat("_Material.Kd", material.Kd);
		deferredShader.setFloat("_Material.Ks", material.Ks);
		deferredShader.setFloat("_Material.Shininess", material.Shininess);
		deferredShader.setFloat("_Material.Shininess", material.Shininess);
		deferredShader.setVec3("_EyePos", camera.position);
		deferredShader.setMat4("_LightViewProjection", lightSpaceMatrix);
		deferredShader.setVec3("_LightDirection", lightDirection);
		deferredShader.setVec3("_LightColor", lightColor);
		
		for(int i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			deferredShader.setVec3("_PointLights[" + std::to_string(i) + "].position", pointLights[i].position);
			deferredShader.setVec3("_PointLights[" + std::to_string(i) + "].color", pointLights[i].color);
			deferredShader.setFloat("_PointLights[" + std::to_string(i) + "].radius", pointLights[i].radius);
			deferredShader.setFloat("_PointLights[" + std::to_string(i) + "].intensity", pointLights[i].intensity);
		}

		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));

		//update scene asset transforms
		scene.setAssetRot(0, monkeyTransform.rotation);

		//Not Working ATM
		//scene.getAsset(0).getTransform().rotation = glm::rotate(scene.getAsset(0).getTransform().rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		//monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		//scene.getAsset(0).addTransform(monkeyTransform);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.fbo); //Read from gBuffer 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo); //Write to current fbo
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST	);


		//Light Orb Draw
		lightOrbShader.use();
		lightOrbShader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		//make point lights revolve around world origin over time
		for (int i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			lightOrbShader.setMat4("_Model", lightSphereTransform[i].modelMatrix());
			lightOrbShader.setVec3("_Color", pointLights[i].color);
			sphereMesh.draw();
		}

		//Post Processing Pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindTextureUnit(0, (GLuint)framebuffer.colorBuffer);
		screenShader.setInt("_InverseOn",		postProcess.inverse);
		screenShader.setInt("_EffectNumber",	postProcess.effectNumber);
		screenShader.setFloat("_BlurAmount",	postProcess.blurAmount);
		screenShader.setVec3("_KernelTop",		postProcess.kernelTop);
		screenShader.setVec3("_KernelCenter",	postProcess.kernelCenter);
		screenShader.setVec3("_KernelBottom",	postProcess.kernelBottom);
		screenShader.setInt("_ColorBuffer",		0);
		glBindTextureUnit(1, (GLuint)framebuffer.depthBuffer);
		screenShader.setInt("_DepthBuffer",		1);

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

	//Primary Controls
	ImGui::Begin("Settings");
	if (ImGui::CollapsingHeader("Camera Controls")) {
		if (ImGui::Button("Reset Camera")) {
			resetCamera(&camera, &cameraController);
		}
		ImGui::Checkbox("Orthographic", &camera.orthographic);
		ImGui::SliderFloat("CameraFOV", &camera.fov, 60.0f, 120.0f);
		ImGui::SliderFloat("Near Plane", &camera.nearPlane, 0.1f, 10.0f);
		ImGui::SliderFloat("Far Plane", &camera.farPlane, 0.1f, 1000.0f);
	}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Lights")) {
		if (ImGui::CollapsingHeader("Directional Light")) {
			ImGui::ColorEdit3("LightColor", &lightColor.x);

			if (ImGui::SliderFloat3("Light Direction", &lightDirection.x, -1, 1));
			if (glm::length(lightDirection) != 0)
			{
				lightDirection = glm::normalize(lightDirection);
			}
		}
		if (ImGui::CollapsingHeader("Point Lights")) {
			ImGui::SliderFloat("Intensity", &pointLightIntensity, 0.1f, 2.0f);
			for (int i = 0; i < MAX_POINT_LIGHTS; i++)
			{
				//Set Point Light Intensity
				pointLights[i].intensity = pointLightIntensity;
			}
		}
	}
	if (ImGui::CollapsingHeader("Shadowmap")) {
		ImGui::Checkbox("Orthographic", &shadowMapCamera.orthographic);
		ImGui::SliderFloat("Ortho Height", &shadowMapCamera.orthoHeight, 1.0f, 10.0f);
		ImGui::SliderFloat("Near Plane", &shadowMapCamera.nearPlane, 0.1f, 10.0f);
		ImGui::SliderFloat("Far Plane", &shadowMapCamera.farPlane, 0.1f, 20.0f);
		ImGui::SliderFloat("Camera Distance", &lightCamDist, 3.0f, 20.0f);
		ImGui::SliderFloat("Min Slope Bias", &minSlopeBias, 0.001f, 0.01f);
		ImGui::SliderFloat("Max Slope Bias", &maxSlopeBias, 0.005f, 0.02f);
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

	//Texture Mapping Debug View
	ImGui::Begin("GBuffers"); {
		ImVec2 texSize = ImVec2(gBuffer.width/4, gBuffer.height/4);
		for (size_t i = 0; i < 3; i++)
		{
			ImGui::Image((ImTextureID)gBuffer.colorBuffers[i], texSize, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::Image((ImTextureID)shadowMapTex, texSize, ImVec2(0, 1), ImVec2(1, 0));
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

