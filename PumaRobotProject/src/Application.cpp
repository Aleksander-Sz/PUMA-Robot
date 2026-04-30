#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Paths.h"
#include "../Shader.h"
#include "../Camera.h"
#include "../Light.h"
#include "../Model.h"
#include "../Robot.h"
#include "../Particles.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <stb_image.h>
#include <vector>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMovement = true;
Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (cam)
	{
		cam->windowWidth = width;
		cam->windowHeight = height;
	}
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

static void window_focus_callback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		firstMovement = true;
	}
}

static void pumaSetupMouseCapture(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	int ww, wh;
	glfwGetWindowSize(window, &ww, &wh);
	lastX = ww / 2.0;
	lastY = wh / 2.0;
	firstMovement = true;
}

int main()
{
	pumaInitAssetRoot();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
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
	int windowWidth = WINDOW_WIDTH;
	int windowHeight = WINDOW_HEIGHT;

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
	glfwSetWindowUserPointer(window, &camera);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);
	camera = Camera(fbWidth, fbHeight);

	pumaSetupMouseCapture(window);

	// Rendering commands here

	Shader ourShader(
		pumaResolveAsset("Shaders/VertexShader.glsl"),
		pumaResolveAsset("Shaders/FragmentShader.glsl"));
	ourShader.use();
	ourShader.setInt("metalSheetTex", 0);
	ourShader.setInt("wallTex", 1);
	ourShader.setInt("floorTex", 2);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	Robot robot;
	Model room;
	room.Room(8.0f, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)));
	Model mirror;
	Model mirrorBack;
	Model cylinder;
	cylinder.Cylinder(0.3f, 3.0f, glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, -1.0f)), 3.14159f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
	ParticleSystem particles;

	Shader shadowVolShader(
		pumaResolveAsset("Shaders/ShadowVolume.vert"),
		pumaResolveAsset("Shaders/ShadowVolume.frag"));
	Shader shadowDarkenShader(
		pumaResolveAsset("Shaders/ShadowDarken.vert"),
		pumaResolveAsset("Shaders/ShadowDarken.frag"));

	unsigned int shadowVolVao = 0, shadowVolVbo = 0, fullscreenVao = 0;
	glGenVertexArrays(1, &shadowVolVao);
	glGenBuffers(1, &shadowVolVbo);
	glBindVertexArray(shadowVolVao);
	glBindBuffer(GL_ARRAY_BUFFER, shadowVolVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * (int)sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &fullscreenVao);
	glBindVertexArray(fullscreenVao);
	glBindVertexArray(0);

	GLuint sceneFbo = 0;
	GLuint sceneColorTex = 0;
	GLuint sceneDepthTex = 0;
	glGenFramebuffers(1, &sceneFbo);
	glGenTextures(1, &sceneColorTex);
	glGenTextures(1, &sceneDepthTex);

	glBindTexture(GL_TEXTURE_2D, sceneColorTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, fbWidth, fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, sceneDepthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, fbWidth, fbHeight, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, sceneDepthTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "scene FBO incomplete\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Lighting Shader
	// point light cube
	Shader lightsShader(
		pumaResolveAsset("Shaders/LightsVertexShader.glsl"),
		pumaResolveAsset("Shaders/LightsFragmentShader.glsl"));
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
	Light light = Light::PointLight(glm::vec3(1.45f, 1.45f, 1.45f), glm::vec3(0.75f), glm::vec3(2.0f, 1.0f, 0.0f));
	glm::vec3 targetPosition;
	glm::vec3 targetNormal(0.0f, 1.0f, 0.0f);
	float r = 0.5f;
	const float mirrorTiltRad = glm::radians(50.0f);
	glm::mat4 circleTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.3f, 0.0f)) * glm::rotate(glm::mat4(1.0f), -mirrorTiltRad, glm::vec3(0.0f, 0.0f, 1.0f));
	const glm::mat4 mirrorPlaneMatrix = circleTranslationMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.07f, 0.0f));
	targetNormal = glm::vec3(mirrorPlaneMatrix * glm::vec4(targetNormal, 0.0f));
	mirror.Plane(1.5f, mirrorPlaneMatrix);
	mirrorBack.Plane(1.5f, mirrorPlaneMatrix * glm::rotate(glm::mat4(1.0f), 3.14159f, glm::vec3(0.0f, 0.0f, 1.0f)));
	const glm::vec3 mirrorUAxis = glm::normalize(glm::vec3(mirror.model * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
	const glm::vec3 mirrorVAxis = glm::normalize(glm::vec3(mirror.model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	const glm::vec3 mirrorOrigin = glm::vec3(mirror.model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	const float mirrorSize = 1.5f;

	GLuint mirrorSheetTex = 0;
	glGenTextures(1, &mirrorSheetTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mirrorSheetTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	int texW = 0, texH = 0, texCh = 0;
	unsigned char* mirrorTexData = stbi_load(
		pumaResolveAsset("Textures/metalsheet.jpg").c_str(),
		&texW, &texH, &texCh, 0);
	if (mirrorTexData)
	{
		GLenum fmt = (texCh == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, fmt, texW, texH, 0, fmt, GL_UNSIGNED_BYTE, mirrorTexData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load mirror texture: Textures/metalsheet.jpg\n";
		unsigned char fallback[3] = { 170, 170, 170 };
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fallback);
	}
	stbi_image_free(mirrorTexData);
	glBindTexture(GL_TEXTURE_2D, 0);

	auto load2DTexture = [&](const std::string& relPath, GLenum wrapMode) -> GLuint
	{
		GLuint tex = 0;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int w = 0, h = 0, ch = 0;
		unsigned char* data = stbi_load(pumaResolveAsset(relPath).c_str(), &w, &h, &ch, 0);
		if (data)
		{
			GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cerr << "Failed to load texture: " << relPath << "\n";
			unsigned char fallback[3] = { 255, 255, 255 };
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fallback);
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return tex;
	};
	GLuint wallTex = load2DTexture("Textures/container-diffuse.png", GL_REPEAT);
	GLuint floorTex = load2DTexture("Textures/container-specular.png", GL_REPEAT);

	const glm::vec3 mirrorNormal = glm::normalize(glm::vec3(mirror.model * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
	const glm::vec3 mirrorPoint = glm::vec3(mirror.model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	const float mirrorD = -glm::dot(mirrorNormal, mirrorPoint);
	const float a = mirrorNormal.x;
	const float b = mirrorNormal.y;
	const float c = mirrorNormal.z;
	glm::mat4 mirroredWorldMatrix(1.0f);
	mirroredWorldMatrix[0][0] = 1.0f - 2.0f * a * a;
	mirroredWorldMatrix[1][0] = -2.0f * a * b;
	mirroredWorldMatrix[2][0] = -2.0f * a * c;
	mirroredWorldMatrix[3][0] = -2.0f * a * mirrorD;
	mirroredWorldMatrix[0][1] = -2.0f * a * b;
	mirroredWorldMatrix[1][1] = 1.0f - 2.0f * b * b;
	mirroredWorldMatrix[2][1] = -2.0f * b * c;
	mirroredWorldMatrix[3][1] = -2.0f * b * mirrorD;
	mirroredWorldMatrix[0][2] = -2.0f * a * c;
	mirroredWorldMatrix[1][2] = -2.0f * b * c;
	mirroredWorldMatrix[2][2] = 1.0f - 2.0f * c * c;
	mirroredWorldMatrix[3][2] = -2.0f * c * mirrorD;

	int frame = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
		glViewport(0, 0, fbWidth, fbHeight);
		{
			GLenum sceneDrawBuf[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, sceneDrawBuf);
		}

		//rendering commands here
		ourShader.use();
		glClearColor(0.0f, 0.1f, 0.0f, 1.0f);

		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		ourShader.setMat4("view", camera.view());
		ourShader.setMat4("projection", camera.projection());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mirrorSheetTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		glActiveTexture(GL_TEXTURE0);

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
		ourShader.setBool("useMetalSheetTex", false);
		ourShader.setBool("useRoomTex", false);
		ourShader.setFloat("wallTexScale", 1.0f);
		ourShader.setFloat("floorTexScale", 1.0f);
		ourShader.setFloat("ceilingTexScale", 1.0f);
		ourShader.setFloat("floorUvRotation", 0.0f);
		ourShader.setFloat("floorUvShear", -0.577350269f);
		//material properties
		ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setFloat("material.shininess", 32.0f);
		const glm::vec3 baseMetalColor(0.72f, 0.74f, 0.78f);
		const glm::vec3 robotOrange(1.0f, 0.9f, 0.1f);
		const glm::vec3 mirrorTintColor(0.72f, 0.75f, 0.80f);
		const float mirrorSheetAlpha = 0.35f;
		ourShader.setVec3("color", baseMetalColor);
		ourShader.setFloat("alpha", 1.0f);
		ourShader.setBool("useClipPlane", false);
		float now = glfwGetTime();
		targetPosition.x = r * cosf(now);
		targetPosition.y = 0.0f;
		targetPosition.z = r * sinf(now);
		targetPosition = glm::vec3(circleTranslationMatrix * glm::vec4(targetPosition, 1.0f));
		robot.InverseKinematics(targetPosition, targetNormal);

		// Mark mirror pixels in stencil bit 7.
		glStencilMask(0x80);
		glStencilFunc(GL_ALWAYS, 0x80, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		mirror.Draw(ourShader);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glStencilMask(0x00);
		glStencilFunc(GL_EQUAL, 0x80, 0x80);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// Draw reflected scene only inside the mirror mask.
		glCullFace(GL_FRONT);
		const glm::vec4 mirrorPlaneEq(mirrorNormal, mirrorD);
		const float cameraPlaneSide = glm::dot(mirrorNormal, camera.cameraPos) + mirrorD;
		const float clipSign = (cameraPlaneSide >= 0.0f) ? 1.0f : -1.0f;
		ourShader.setVec4("clipPlane", mirrorPlaneEq * clipSign);
		ourShader.setBool("useClipPlane", true);
		glEnable(GL_CLIP_DISTANCE0);

		ourShader.setMat4("view", camera.view() * mirroredWorldMatrix);

		ourShader.setVec3("color", robotOrange);
		robot.Draw(ourShader);
		ourShader.setVec3("color", baseMetalColor);
		ourShader.setBool("useRoomTex", true);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		room.Draw(ourShader);
		ourShader.setBool("useRoomTex", false);
		glActiveTexture(GL_TEXTURE0);
		ourShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder.Draw(ourShader);
		ourShader.setVec3("color", baseMetalColor);
		ourShader.setBool("useClipPlane", false);
		glDisable(GL_CLIP_DISTANCE0);
		particles.Draw(camera.projection() * camera.view() * mirroredWorldMatrix);
		ourShader.use();

		glCullFace(GL_BACK);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// Draw normal scene while preserving stencil.
		glStencilMask(0x00);
		glStencilFunc(GL_ALWAYS, 0, 0x00);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		ourShader.setMat4("view", camera.view());

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		mirror.Draw(ourShader);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		ourShader.setVec3("color", robotOrange);
		robot.Draw(ourShader);
		ourShader.setVec3("color", baseMetalColor);
		ourShader.setBool("useRoomTex", true);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		room.Draw(ourShader);
		ourShader.setBool("useRoomTex", false);
		glActiveTexture(GL_TEXTURE0);
		mirrorBack.Draw(ourShader);
		ourShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		cylinder.Draw(ourShader);
		ourShader.setVec3("color", mirrorTintColor);
		ourShader.setFloat("alpha", mirrorSheetAlpha);
		ourShader.setBool("useMetalSheetTex", true);
		ourShader.setVec3("mirrorOrigin", mirrorOrigin);
		ourShader.setVec3("mirrorUAxis", mirrorUAxis);
		ourShader.setVec3("mirrorVAxis", mirrorVAxis);
		ourShader.setFloat("mirrorSize", mirrorSize);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mirrorSheetTex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		mirror.Draw(ourShader);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		ourShader.setVec3("color", baseMetalColor);
		ourShader.setFloat("alpha", 1.0f);
		ourShader.setBool("useMetalSheetTex", false);

		particles.Update(targetPosition, circleTranslationMatrix);

		// Clear mirror bit where the mirror is occluded by foreground geometry.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		glStencilMask(0x80);
		glStencilFunc(GL_ALWAYS, 0x80, 0xFF);
		glStencilOp(GL_KEEP, GL_ZERO, GL_KEEP);
		mirror.Draw(ourShader);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		static std::vector<glm::vec4> shadowGeom;
		shadowGeom.clear();
		robot.AppendShadowVolumes(light.position, shadowGeom);

		// Z-fail shadow volume pass in stencil bits 0..6.
		glStencilMask(0x7F);
		glEnable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		shadowVolShader.use();
		shadowVolShader.setMat4("viewProj", camera.projection() * camera.view());
		glBindVertexArray(shadowVolVao);
		if (!shadowGeom.empty())
		{
			glBindBuffer(GL_ARRAY_BUFFER, shadowVolVbo);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(shadowGeom.size() * sizeof(glm::vec4)), shadowGeom.data(), GL_STREAM_DRAW);
		}

		glEnable(GL_CULL_FACE);
		glStencilFunc(GL_ALWAYS, 0, 0x7F);

		if (!shadowGeom.empty())
		{
			glEnable(GL_DEPTH_CLAMP);
			glCullFace(GL_FRONT);
			glStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)shadowGeom.size());
			glCullFace(GL_BACK);
			glStencilOp(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)shadowGeom.size());
			glDisable(GL_DEPTH_CLAMP);
		}

		glCullFace(GL_BACK);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		// Multiply darkening over shadowed pixels.
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL, 0, 0x7F);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_DEPTH_TEST);

		shadowDarkenShader.use();
		shadowDarkenShader.setVec3("darken", glm::vec3(0.58f, 0.58f, 0.60f));
		glBindVertexArray(fullscreenVao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glStencilFunc(GL_GREATER, 0x80, 0xFF);
		shadowDarkenShader.setVec3("darken", glm::vec3(0.50f, 0.50f, 0.55f));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);

		particles.Draw(camera.projection() * camera.view());

		glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, fbWidth, fbHeight, 0, 0, fbWidth, fbHeight,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//lightColor = glm::vec3(fmod(lastFrame, 5.0f) / 5.0f);
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
