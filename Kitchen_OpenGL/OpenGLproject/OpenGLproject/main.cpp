//
//  main.cpp
//  OpenGL_Shader_Example_step1
//
//  Created by CGIS on 02/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
	#define GLFW_INCLUDE_GLCOREARB
#else
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include <windows.h>
#pragma comment(lib, "winmm.lib")
#include <string>
#include <chrono>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

/////matricele pentru transformarile de model
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

//vectori pentru lumina directionala
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;


gps::Camera myCamera(
	glm::vec3(-2.90856f, 1.33744f, -2.72827f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

float cameraSpeed = 0.03f;

float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
bool firstMouse = true;

bool pressedKeys[1024];
float yaw = 0.0f;
float pitch = 0.0f;

gps::Model3D myScene;
gps::Model3D glass;
gps::Model3D glassWine;
gps::Model3D decanter;
gps::Model3D decanterWine;
gps::Model3D screenQuad;
gps::Model3D rain;
gps::Model3D window;
gps::Model3D detalii;
gps::Model3D cuptor;
gps::Model3D usa;

gps::Shader myCustomShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;
gps::Shader transparentShader;
gps::Shader screenQuadShader;
gps::SkyBox mySkyBox;
std::vector<const GLchar*> faces;


//umbre
GLuint shadowMapFBO;
GLuint depthMapTexture;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;
bool showDepthMap;

//lumini
bool isLightEnabled = true; //activeaza direc.
GLuint isLightEnabledLoc;
bool isPointLightEnabled = false; //activeaza punctiforma
GLuint isPointLightEnabledLoc;
bool isShadowEnabled = false;//activeaza direct.+umbre
GLuint isShadowEnableLoc;

GLfloat lightAngle;


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {

	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)	{

		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	//float sensitivity = 0.1f;

	//if (firstMouse) {
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//// Calcularea diferențelor între pozițiile curente și precedente
	//float xOffset = xpos - lastX;
	//float yOffset = lastY - ypos; // invers deoarece coordonata y crește în jos

	//// Actualizarea pozițiilor anterioare ale mouse-ului
	//lastX = xpos;
	//lastY = ypos;

	//// Aplicarea sensibilității
	//xOffset *= sensitivity;
	//yOffset *= sensitivity;

	//yaw += xOffset;
	//pitch += yOffset;

	//if (pitch > 89.0f) {
	//	pitch = 89.0f;
	//}
	//if (pitch < -89.0f) {
	//	pitch = -89.0f;
	//}

	//// Aplicarea rotației camerei
	//myCamera.rotate(pitch, yaw);

}

/////////turul scenei
struct CameraPoint {
	glm::vec3 position;
	glm::vec3 target; //Camera look-at
};

std::vector<CameraPoint> cameraPath = { //camera se deplaseaza intre aceste puncte intr-un mod animat, oferind utilizatorului o impresie de plimbare in scena
	{{-1.98567f, 1.08449f, -5.96466f}, {-2.10592f, 0.527297f, -4.9032f}},  
	{{-2.10592f, 0.527297f, -4.9032f}, {-1.91592f, 0.81729f, -1.8032f}},            
	{{-1.91592f, 0.81729f, -1.8032f}, {-2.80592f, 1.1273f, -0.4532f}},
	{{-2.80592f, 1.1273f, -0.4532f}, {-2.40592f, 1.0573f, -0.4532}},
	{{-2.40592f, 1.0573f,  -0.4532 }, {-1.89592f, 1.0573f, -0.6632f}},
	{{-1.89592f, 1.0573f, -0.6632}, {-1.68592f, 1.0273f,  0.1968 }},
    {{-1.68592f, 1.0273f,  0.1968 }, {-0.92592f, 1.0273f , 0.2268}},
	{{-0.92592f, 1.0273f , 0.2268}, {-0.10592f, 1.0273f,  -0.6432f }},
	{{-0.10592f, 1.0273f,  -0.6432f }, {1.51408f, 1.0573f , -1.26466}},
	{{1.51408f, 1.0573f , -1.26466}, {0.57408f, 0.9673f,  -1.99466 }},
	{{0.57408f, 0.9673f,  -1.99466 }, {-1.47408f, 0.6773f,  -2.04466}}, //{0.79592f , 1.1373f, -2.31466 }},
	{{-1.47408f, 0.6773f,  -2.04466}, {-1.80408f, 0.3973f,  -4.14466f}},
};

bool isTourActive = false;
int currentPointIndex = 0; //intre care dintre cele 2 pct se afla camera
float tSpeed = 0.3f;
float tourTimeAccumulator = 0.0f; //cu asta verificam daca progresum camerei e intre 2 pct;

glm::vec3 getCameraPosition(glm::vec3 P0, glm::vec3 P1, float t) {
	return P1 * t + P0 * (1.0f - t);  //pozitia camerei intre 2 puncte, unde t variaza intre 0 si 1 => deplasare de la P0 la P1
}

void updateCameraMovement(float deltaTime, gps::Camera& myCamera, gps::Shader& myCustomShader) {
	if (!isTourActive) return;

	tourTimeAccumulator += deltaTime * tSpeed;

	if (tourTimeAccumulator > 1.0f) {
		tourTimeAccumulator = 0.0f;  
		currentPointIndex++;   

		//la ultima locație, oprim turul
		if (currentPointIndex >= cameraPath.size() - 1) {
			isTourActive = false;
			std::cout << "Turul a ajuns la ultima final!" << std::endl;
			return;
		}
	}

	glm::vec3 cameraPos = getCameraPosition( //calculam pozitia + target
		cameraPath[currentPointIndex].position,
		cameraPath[currentPointIndex + 1].position,
		tourTimeAccumulator
	);

	glm::vec3 cameraTarget = getCameraPosition(
		cameraPath[currentPointIndex].target,
		cameraPath[currentPointIndex + 1].target,
		tourTimeAccumulator
	);

	myCamera.setPosition(cameraPos);
	myCamera.setTarget(cameraTarget);

	//trimitem matricea cu noile cameraPos si cameraTarget la shader
	glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	GLuint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void startTour() { //apelata la apasarea tastei T
	if (!isTourActive) {
		isTourActive = true;
		currentPointIndex = 0;
		tourTimeAccumulator = 0.0f;
		std::cout << "Turul camerei a început!" << std::endl;
	}
}
//////////////////////////final tur


/////////////////////////fulger
void playBackgroundSound(const char* filepath) { 
	std::wstring wideFilePath = std::wstring(filepath, filepath + strlen(filepath));
	PlaySound(wideFilePath.c_str(), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}
void stopBackgroundSound() {
	PlaySound(NULL, NULL, 0); // Oprește orice sunet redat
}

enum FlashState { //starile efectului de fulger
	FLASH_INACTIVE,
	FLASH_BLACK_1,
	FLASH_WHITE,
	FLASH_BLACK_2
};

FlashState flashState = FLASH_INACTIVE; //starea initiala
float flashTimer = 0.0f; //timer pt a masura de cat timp suntem intr o anumita stare
const float BLACK_DURATION = 2.0f; 
const float WHITE_DURATION = 0.1f;
glm::vec3 originalLightColor = glm::vec3(1.0f);
bool isSoundPlaying = false;

void handleFlashEffect(float deltaTime) {

	switch (flashState) {
	case FLASH_INACTIVE:
		
		break;

	case FLASH_BLACK_1:
		lightColor = glm::vec3(0.0f, 0.0f, 0.0f); //lumina neagră
		flashTimer += deltaTime;

		if (!isSoundPlaying) {
			playBackgroundSound("loud-thunder-192165 (online-audio-converter.com).wav"); //sunet fulger
			isSoundPlaying = true; // Marchez că sunetul este activ
		}

		if (flashTimer >= BLACK_DURATION) { //cand ii trece timpul(2 sec) trecem la alb inapoi
			flashTimer = 0.0f;
			flashState = FLASH_WHITE; 
		}
		break;

	case FLASH_WHITE:
		lightColor = glm::vec3(100.0f, 100.0f, 100.0f); 
		flashTimer += deltaTime;
		if (flashTimer >= WHITE_DURATION) {//doar 0.1 sec
			flashTimer = 0.0f;
			flashState = FLASH_BLACK_2; 
		}
		break;

	case FLASH_BLACK_2:
		lightColor = glm::vec3(0.0f, 0.0f, 0.0f); 
		flashTimer += deltaTime;
		if (flashTimer >= 3.0) {
			flashTimer = 0.0f;
			flashState = FLASH_INACTIVE; //efectul se termina
			lightColor = originalLightColor; //+trecem la culoarea originala
			stopBackgroundSound();
			isSoundPlaying = false;
		}
		break;
	}
}

bool moveGlass = false;
float lastFrame = 0.0f;  //timpul de la cadrul anterior
float deltaTime = 0.0f;  //diferența de timp între cadre

void processMovement()
{
	//1 - NORMAL STATE
	if (glfwGetKey(glWindow, GLFW_KEY_1)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//2 - LINE STATE
	if (glfwGetKey(glWindow, GLFW_KEY_2)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//3 POINT STATE
	if (glfwGetKey(glWindow, GLFW_KEY_3)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	///////////////////////////////////////////////////////////////////////////MISCARI
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}


	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}

	if (pressedKeys[GLFW_KEY_U]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}

	if (pressedKeys[GLFW_KEY_I]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

	}

	if (pressedKeys[GLFW_KEY_UP]) { //rotatii
		pitch += 0.5f;
		myCamera.rotate(pitch, yaw);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

	}
	if (pressedKeys[GLFW_KEY_DOWN]) {
		pitch -= 0.5f;
		myCamera.rotate(pitch, yaw);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}
	if (pressedKeys[GLFW_KEY_RIGHT]) {
		yaw += 0.5f;
		myCamera.rotate(pitch, yaw);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}
	if (pressedKeys[GLFW_KEY_LEFT]) {
		yaw -= 0.5f;
		myCamera.rotate(pitch, yaw);
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
	}
	///////////////////////////////////////////////////////////////////////////MISCARI


	//LUMINA DIRECTIONALA
	if (pressedKeys[GLFW_KEY_L]) {
		isLightEnabled = !isLightEnabled; //comutam starea
		isPointLightEnabled = false; //dezactivăm punctiforma
		myCustomShader.useShaderProgram();
		glUniform1i(isLightEnabledLoc, isLightEnabled);
		glUniform1i(isPointLightEnabledLoc, isPointLightEnabled);
		pressedKeys[GLFW_KEY_L] = false; //previn repetarea
	}

	//LUMINA CU UMBRE
	if (pressedKeys[GLFW_KEY_O]) {
		isShadowEnabled = !isShadowEnabled; //comutam starea
		isPointLightEnabled = false; //dezactivăm punctiforma
		isLightEnabled = false; //dezactivam lumina directionala simpla
		myCustomShader.useShaderProgram();
		glUniform1i(isShadowEnableLoc, isShadowEnabled);
		glUniform1i(isPointLightEnabledLoc, isPointLightEnabled);
		glUniform1i(isLightEnabledLoc, isLightEnabled);
		pressedKeys[GLFW_KEY_O] = false; 
	}


	//LUMINA PUNCTIFORMA
	if (pressedKeys[GLFW_KEY_P]) {
		isPointLightEnabled = !isPointLightEnabled; 
		isLightEnabled = false;
		myCustomShader.useShaderProgram();
		glUniform1i(isPointLightEnabledLoc, isPointLightEnabled);
		glUniform1i(isLightEnabledLoc, isLightEnabled);
		pressedKeys[GLFW_KEY_P] = false; 
	}


	if (pressedKeys[GLFW_KEY_0]) { //pornire animatie
		moveGlass = true;
	}

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;  // (diferența de timp între cadre)
	lastFrame = currentFrame;  //actualizăm timpul pentru următorul cadru

	////////////////////////////////////ROTATIA LUMINII
	if (pressedKeys[GLFW_KEY_Q]) {
		lightAngle += 1.0f;
		if (lightAngle > 360.0f) lightAngle -= 360.0f;

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat3 normalMatrixForLight = glm::transpose(glm::inverse(glm::mat3(view * lightRotation)));//actulizam directia liminii cu noua modificare audsa
		glm::vec3 transformedLightDir = normalMatrixForLight * lightDir;

		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(transformedLightDir));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		lightAngle -= 1.0f;
		if (lightAngle < 0.0f) lightAngle += 360.0f;

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat3 normalMatrixForLight = glm::transpose(glm::inverse(glm::mat3(view * lightRotation)));
		glm::vec3 transformedLightDir = normalMatrixForLight * lightDir;

		lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(transformedLightDir));
	}

	//tur
	if (pressedKeys[GLFW_KEY_T]) {
		//playBackgroundSound("mixkit-cartoon-kitty-begging-meow-92.wav");
		startTour();
	}
	if (isTourActive) {
		updateCameraMovement(deltaTime, myCamera, myCustomShader);
	}

	//flash
	if (pressedKeys[GLFW_KEY_F]) {
		if (flashState == FLASH_INACTIVE) {
			flashState = FLASH_BLACK_1; //la apasarea tastei in trecem in starea de negru
			flashTimer = 0.0f; 
			originalLightColor = lightColor;
		}
	}

}

bool initOpenGLWindow() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for multisampling/antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	////////////////FEREASTRA REDIMENSIONATA PE INTREG ECRANUL
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	if (primaryMonitor) {
		glWindowWidth = mode->width;
		glWindowHeight = mode->height;

		glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Texturing", NULL, NULL);

		// Centrează fereastra pe monitor
		int xPos = (mode->width - glWindowWidth) / 2;
		int yPos = (mode->height - glWindowHeight) / 2;
		glfwSetWindowPos(glWindow, xPos, yPos);
	}
	else {
		glWindowWidth = 800;
		glWindowHeight = 600;

		glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Texturing", NULL, NULL);
	}
	///////////////////

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initObjects() {

	myScene.LoadModel("../../Scene.obj", "../../");
	glass.LoadModel("../../glass.obj");
	glassWine.LoadModel("../../glass_wine.obj", "../../");
	decanter.LoadModel("../../decanter.obj");
	decanterWine.LoadModel("../../decanter_wine.obj", "../../");
	screenQuad.LoadModel("quad/quad.obj");
	rain.LoadModel("../../rain.obj", "../../");
	window.LoadModel("../../window.obj", "../../");
	detalii.LoadModel("../../detalii.obj", "../../");
	cuptor.LoadModel("../../cuptor.obj", "../../");
	usa.LoadModel("../../usa.obj", "../../");

}

///////////////////luminile punctiforme
struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
	float constant;
	float linear;
	float quadratic;
};
std::vector<PointLight> pointLights = {
	{{-0.262264f, 1.78793f, -1.23621f}, {1.0f, 1.0f, 1.0f}, 1.0f, 0.7f, 1.8f},  //hota
	{{1.37087f, 2.02922f, 1.31587f}, {0.647f, 0.949f, 0.9529f}, 1.0f, 1.5f, 2.5f},  //frigider
	{{-3.15158f, 0.372457f, -1.3f}, {1.0f, 1.0f, 0.0f}, 1.0f, 1.5f, 2.5f},  //cuptor
	{{0.429018f, 1.40002f, 1.50442f}, {1.0f, 1.0f, 1.0f}, 1.0f, 2.0f, 2.5f}  //semineu
};
void sendPointLightsToShader(gps::Shader& shader) {
	shader.useShaderProgram();


	for (int i = 0; i < pointLights.size(); ++i) {
		std::string baseName = "pointLights[" + std::to_string(i) + "]";

		GLuint positionLoc = glGetUniformLocation(shader.shaderProgram, (baseName + ".position").c_str());
		GLuint colorLoc = glGetUniformLocation(shader.shaderProgram, (baseName + ".color").c_str());
		GLuint constantLoc = glGetUniformLocation(shader.shaderProgram, (baseName + ".constant").c_str());
		GLuint linearLoc = glGetUniformLocation(shader.shaderProgram, (baseName + ".linear").c_str());
		GLuint quadraticLoc = glGetUniformLocation(shader.shaderProgram, (baseName + ".quadratic").c_str());

		glUniform3fv(positionLoc, 1, glm::value_ptr(pointLights[i].position));
		glUniform3fv(colorLoc, 1, glm::value_ptr(pointLights[i].color));
		glUniform1f(constantLoc, pointLights[i].constant);
		glUniform1f(linearLoc, pointLights[i].linear);
		glUniform1f(quadraticLoc, pointLights[i].quadratic);
	}
}

void initUniforms() {

	myCustomShader.useShaderProgram();

	//initialize the model matrix
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send matrix data to shader
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//initialize the normal matrix
	normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	//initialize the projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 30.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction
	lightDir = glm::vec3(20.0f, 7.5f, -1.0f);
	//lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat3 normalMatrixForLight = glm::transpose(glm::inverse(glm::mat3(view * lightRotation))); //matrice normala pentru rotatia luminii, pt ca rotatiile si scalarile pot distorsiona
	glm::vec3 transformedLightDir = normalMatrixForLight * lightDir;
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(transformedLightDir));

	//set light color - culoarea luminii directionale - 
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	isLightEnabledLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isLightEnabled");
	glUniform1i(isLightEnabledLoc, isLightEnabled);

//////luminile punctiforme
	sendPointLightsToShader(myCustomShader);

	// Starea luminii punctiforme
	isPointLightEnabledLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isPointLightEnabled");
	glUniform1i(isPointLightEnabledLoc, isPointLightEnabled);

	isShadowEnableLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isShadowEnabled");
	glUniform1i(isShadowEnableLoc, isShadowEnabled);


	///lumina shader transparent
	transparentShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(transparentShader.shaderProgram, "lightDir"), 20.0f, 7.5f, -1.0f);
	glUniform3f(glGetUniformLocation(transparentShader.shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f); 

}


void initShaders() {

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();

	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	transparentShader.loadShader("shaders/transparent.vert", "shaders/transparent.frag");
	transparentShader.useShaderProgram();

	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
}

void initOpenGLState() {

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB); //***
}

void initSkyBox() {

	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");

	mySkyBox.Load(faces);
}


void drawDecanter(glm::vec3& decanterPos, glm::mat4& view, glm::mat4& projection, gps::Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, decanterPos);

	shader.useShaderProgram();
	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	GLuint alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, 0.3f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	decanter.Draw(shader);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}


void drawDecanterWine(glm::vec3& decanterPos, glm::mat4& view, glm::mat4& projection, gps::Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, decanterPos);

	shader.useShaderProgram();
	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	GLuint alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, 0.75f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	decanterWine.Draw(shader);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void drawTransparentObject(glm::mat4& view, glm::mat4& projection, gps::Shader& shader, gps::Model3D& object, float alphaValue) {
	glm::mat4 model = glm::mat4(1.0f);  

	shader.useShaderProgram();
	
	//locatiile din shader
	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


	GLuint alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alphaValue);

	// Activăm blending-ul pentru transparență
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  

	object.Draw(shader);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}
void drawFridgeDoor(glm::vec3& doorPosition, float rotationY, glm::mat4& view, glm::mat4& projection, gps::Shader& shader) {
	glm::mat4 model = glm::mat4(1.0f);

	//rotația în jurul balamalei ușii
	glm::vec3 rotationOffset = glm::vec3(1.80366f, 2.08126f, 0.8403f); 

	// Aplică transformările
	model = glm::translate(model, rotationOffset); //Mută la pivot
	model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotește
	model = glm::translate(model, -rotationOffset); 

	shader.useShaderProgram();
	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	usa.Draw(shader);
}


void handleAnimation(bool& moveGlass, float deltaTime) {
	static float moveSpeed = 0.3;  
	static glm::vec3 decanterPos = glm::vec3(0.0f, 0.0f, 0.0f);  
	static glm::vec3 decanterStartPos = glm::vec3(0.0f, 0.0f, 0.0f); 
	static glm::vec3 decanterFinalPos = glm::vec3(0.0f, 0.2f, 0.0f); 
	static bool returnToStart = false; 
	static bool showGlassWine = false;

	//animatie usa
	static float doorRotationY = 0.0f;
	static glm::vec3 doorPosition = glm::vec3(0.0f, 0.0f, 0.0f); 
	static bool doorAnimationDone = false;

	if (moveGlass) {

		decanterPos = glm::mix(decanterPos, decanterFinalPos, moveSpeed * deltaTime);

		//Dacă decanterul ajunge la poziția finală, opresc animatia
		if (glm::length(decanterPos - decanterFinalPos) < 0.01f) {
			moveGlass = false;       
			showGlassWine = true; //desenez vinului în pahar
			returnToStart = true;
		}

		if (!doorAnimationDone) {
			doorRotationY = glm::mix(doorRotationY, 37.3f, moveSpeed * deltaTime); //rotire usa
			doorPosition.z = glm::mix(doorPosition.z, -1.5f, moveSpeed * deltaTime); //translatare usa Z

			if (fabs(doorRotationY + 45.0f) < 0.01f && fabs(doorPosition.z + 1.5f) < 0.01f) {
				doorAnimationDone = true; // Oprește animația ușii
			}
		}

		drawTransparentObject(view, projection, transparentShader, glass, 0.3f);//Pahar
		drawDecanter(decanterPos, view, projection, transparentShader);  //Decanter
		drawDecanterWine(decanterPos, view, projection, transparentShader);  //Vinul din decanter
		drawFridgeDoor(doorPosition, doorRotationY, view, projection, myCustomShader); // Ușa frigiderului

	}
	else {
		
		// scena în starea normala
		drawTransparentObject(view, projection, transparentShader, glass, 0.3f);//Pahar
		drawDecanter(decanterPos, view, projection, transparentShader);  //Decanter
		drawDecanterWine(decanterPos, view, projection, transparentShader);  //Vinul din decanter
		drawFridgeDoor(doorPosition, doorRotationY, view, projection, transparentShader); // Ușa frigiderului

		if (showGlassWine) {
			drawTransparentObject(view, projection, transparentShader, glassWine, 0.75f);//vin pahar
		}
		if (returnToStart) {
			decanterPos = glm::mix(decanterPos, decanterStartPos, moveSpeed * deltaTime);
		}
	}

}

////////////ploaia
struct RainParticle {
	glm::vec3 position; //pozitia si viteza pt fiecare particula
	glm::vec3 velocity;
};
std::vector<RainParticle> rainParticles;
int maxRainParticles = 500; 

float rainAreaMinX = 2.9f; //zona in care ploua
float rainAreaMaxX = 5.0f;
float rainAreaMinY = -10.0f;
float rainAreaMaxY = 10.0f;
float rainAreaMinZ = -10.0f;
float rainAreaMaxZ = 15.0f;

void initRainParticles() {
	
	int numParticlesToGenerate = rand() % 10;  //generam un nr. random de particule la fiecare apel

	for (int i = 0; i < numParticlesToGenerate && rainParticles.size() < maxRainParticles; i++) {
		
		float x = static_cast<float>(rand() % 100) / 10.0f + rainAreaMinX; //pt. fiecare cartila generam coordonate random, in intervalul in care ploua totusi
		float y = static_cast<float>(rand() % 100) / 10.0f + rainAreaMinY; 
		float z = static_cast<float>(rand() % 100) / 10.0f + rainAreaMinZ; 

		if (x >= rainAreaMinX && x <= rainAreaMaxX &&
			y >= rainAreaMinY && y <= rainAreaMaxY &&
			z >= rainAreaMinZ && z <= rainAreaMaxZ) {
			RainParticle particle;
			particle.position = glm::vec3(x, y, z); 
			particle.velocity = glm::vec3(0.0f, -3.0f, 0.0f);
			rainParticles.push_back(particle);
		}
	}
}
void updateRainParticles(float deltaTime) {
	for (auto& particle : rainParticles) {
	
		particle.position += particle.velocity * deltaTime; //miscarea

		if (particle.position.y <= -5.0f) {
			particle.position = glm::vec3(
				static_cast<float>(rand() % 20 + rainAreaMinX),
				static_cast<float>(rand() % 20 + rainAreaMinY),
				static_cast<float>(rand() % 20 + rainAreaMinZ)
			);
		}
	}
}

void drawRainParticles(glm::mat4& view, glm::mat4& projection, gps::Shader& shader) {
	shader.useShaderProgram();

	GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shader.shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shader.shaderProgram, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	GLuint scaleLoc = glGetUniformLocation(shader.shaderProgram, "scale");
	float particleScale = 0.03f;  
	glUniform1f(scaleLoc, particleScale); 

	for (const auto& particle : rainParticles) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, particle.position);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		rain.Draw(shader); 
	}
}
////////////gata ploaia


void initFBO() { // creaza un FBO si o textura de adancime 2D pe care o ataseaza la FBO

	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);//texrura de adancime unde stocam daca un fragment e sau nu in umbra 
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //textura goala in care o sa stocam doar valori legate de adancime
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE); //stocam doar valori de adangime, astfel putem dezactiva culoarea
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
glm::mat4 computeLightSpaceTrMatrix() { //folosita pt a transforma toate fragmentele din depthMapShader in spatiul luminii, iar apoi valoare este stocata in textura de adancime

	//matricea de vizualizare care simuleaza o camera situata la pozitia sursei de lumina ~ cum vede sursa de lumina scena
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//cu glm::mat3(lightRotation) - rotim sursa de lumina + indepartam lumina de scena


	// Matricea de proiecție (lightProjection)
	GLfloat near_plane = 0.1f, far_plane = 200.0f;//planurile cu care delimitam volumul frustum-ului
	glm::mat4 lightProjection = glm::ortho(-2.5f, 2.5f, -2.5f, 2.5f, near_plane, far_plane); //volumul vizibil al scenei din perpectiva luminii

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView; //transforma coordonatele din spatiul lumii in spatiul luminii
	return lightSpaceTrMatrix; //+folosim pt a calcula coordonatele fragmentului in textura de adancime
}
void drawObjects(gps::Shader shader, bool depthPass) { //deseneaza obiectele in scena

	shader.useShaderProgram();

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		//calculam matricea normala doar daca suntem in modul de generare a hartii de adancime
		//pt. ca shader-ul sa calculeze corect luminile
		normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	myScene.Draw(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {

		normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	transparentShader.useShaderProgram();
	handleAnimation(moveGlass, deltaTime);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {

		normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	transparentShader.useShaderProgram();
	drawTransparentObject(view, projection, transparentShader, window, 0.1f);
	drawTransparentObject(view, projection, transparentShader, detalii, 0.2f);
	drawTransparentObject(view, projection, transparentShader, cuptor, 0.3f);


	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {

		normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	myCustomShader.useShaderProgram();
	initRainParticles();
	updateRainParticles(deltaTime);
	drawRainParticles(view, projection, myCustomShader);

	handleFlashEffect(deltaTime); // Gestionează efectul de fulger
	GLuint lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	if (!depthPass)
	{
		skyboxShader.useShaderProgram();
		mySkyBox.Draw(skyboxShader, view, projection);
	}
}

void renderScene() { //coordoneaza procesul de randare

	//GENERAREA HARTII DE ADANCIME + RANDAREA EFECTIVE A SCENEI

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	//matricea de transformare pentru lumina calculata si trimisa la shader
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true); //cu true pt a desena doar harta de adancime
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (showDepthMap) {

		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT);
		screenQuadShader.useShaderProgram(); //afisam harta de adancime cu un dreptunghi pe care se marcheaza intensitatea de umbra(de gri)

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat3 normalMatrixForLight = glm::transpose(glm::inverse(glm::mat3(view * lightRotation))); //matrice normala pentru rotatia luminii, pt ca rotatiile si scalarile pot distorsiona
		glm::vec3 transformedLightDir = normalMatrixForLight * lightDir;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(transformedLightDir));


		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

	}

}

void cleanup() {

	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
	    glfwTerminate();
	    return 1;
	}


	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initSkyBox();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
