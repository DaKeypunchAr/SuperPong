#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "STB/image.h"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "TextAtlas/TextAtlas.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <array>

void __stdcall debugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam)
{
	std::string sourceStr = ((source == GL_DEBUG_SOURCE_API) ? "Source API" :
		((source == GL_DEBUG_SOURCE_APPLICATION) ? "Source Application" :
			((source == GL_DEBUG_SOURCE_OTHER) ? "Source Other" :
				((source == GL_DEBUG_SOURCE_SHADER_COMPILER) ? "Source Shader Compiler" : 
					((source == GL_DEBUG_SOURCE_THIRD_PARTY ? "Source Third Party" :
						((source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) ? "Source Window System" : "Unknown Source")))))));

	std::string typeStr = ((type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) ? "Type Deprecated Behaviour" :
		((type == GL_DEBUG_TYPE_ERROR) ? "Type Error" :
			((type == GL_DEBUG_TYPE_MARKER) ? "Type Marker" :
				((type == GL_DEBUG_TYPE_OTHER) ? "Type Other" :
					((type == GL_DEBUG_TYPE_PERFORMANCE) ? "Type Performance" :
						((type == GL_DEBUG_TYPE_POP_GROUP) ? "Type Pop Group" :
							((type == GL_DEBUG_TYPE_PORTABILITY) ? "Type Portability" :
								((type == GL_DEBUG_TYPE_PUSH_GROUP) ? "Type Push Group" :
									((type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) ? "Type Undefined Behaviour" : "Unknown Type")))))))));

	std::string idStr = "Id "; idStr += std::to_string(id);

	std::string severityStr = ((severity == GL_DEBUG_SEVERITY_HIGH) ? "Severity High" :
		((severity == GL_DEBUG_SEVERITY_LOW) ? "Severity Low" :
			((severity == GL_DEBUG_SEVERITY_MEDIUM) ? "Severity Medium" :
				((severity == GL_DEBUG_SEVERITY_NOTIFICATION) ? "Severity Notification" : "Unknown Severity"))));

	std::cerr << "DEBUG CALLBACK:\n\t" << sourceStr << "\n\t" << typeStr << "\n\t" << idStr << "\n\t" << severityStr << '\n' << "MESSAGE:\n\t" << message << '\n';
	if (type != GL_DEBUG_TYPE_PERFORMANCE) __debugbreak();
}
void debugCallback(int errorCode, const char* description)
{
	std::cerr << "GLFW Error Description:\n" << description << '\n';
	__debugbreak();
}

enum GS
{
	PLAY, MAIN_MENU, PAUSE, WIN
};

int selectedMenuItem = 0;
GS gameState = GS::MAIN_MENU;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		selectedMenuItem++;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		selectedMenuItem--;
	}
	if (selectedMenuItem < 0) selectedMenuItem = 1;
	if (selectedMenuItem > 1) selectedMenuItem = 0;

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (gameState == GS::MAIN_MENU)
		{
			if (selectedMenuItem == 1) glfwSetWindowShouldClose(window, true);
			if (selectedMenuItem == 0) gameState = GS::PLAY;
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (gameState == GS::MAIN_MENU)
			glfwSetWindowShouldClose(window, true);
		else if (gameState == GS::PLAY)
			gameState = GS::PAUSE;
		else if (gameState == GS::PAUSE)
			gameState = GS::PLAY;
	}
}

struct CharRepresentationInfo
{
	char ch;
	glm::ivec2 dimension;
	glm::vec2 bearing, advance;
	glm::vec2 uvBL, uvTR;

	CharRepresentationInfo()
		: ch(0), dimension(), uvBL(), uvTR(), bearing(), advance() {}

	void initialize(const std::string& tag)
	{
		ch = tag.at(0);

		unsigned int dimensionX = std::stoi(tag.substr(2, 8));
		unsigned int dimensionY = std::stoi(tag.substr(11, 8));

		dimension = glm::ivec2(dimensionX, dimensionY);

		float bearingX = std::stof(tag.substr(20, 8));
		float bearingY = std::stof(tag.substr(29, 8));

		bearing = glm::vec2(bearingX, bearingY);

		float advanceX = std::stof(tag.substr(38, 8));
		float advanceY = std::stof(tag.substr(47, 8));

		advance = glm::vec2(advanceX, advanceY);
	}
	void initializeUV(glm::ivec2 atlasDimension, unsigned int xInAtlas)
	{
		uvBL = glm::vec2(xInAtlas / (float)atlasDimension.x, 0.0F);
		uvTR = uvBL + glm::vec2(dimension.x / (float)atlasDimension.x, dimension.y / (float)atlasDimension.y);
	}
};

void main()
{
#pragma region Initializing GLFW and GLEW
	if (!glfwInit())
	{
		__debugbreak();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

	glfwSetErrorCallback(debugCallback);

	constexpr glm::ivec2 windowDimension(1600, 900);
	GLFWwindow* window = glfwCreateWindow(windowDimension.x, windowDimension.y, "SuperPong", nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		__debugbreak();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	if (glewInit())
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		__debugbreak();
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_CALLBACK_FUNCTION);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDebugMessageCallback(debugCallback, nullptr);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
#pragma endregion

#pragma region Some Useful Lambdas
	auto readFile = [&](const std::string& fileLocation)
	{
		std::ifstream fileReader(fileLocation);
		std::string fileContents;
		for (std::string line; std::getline(fileReader, line); fileContents += '\n') fileContents += line;
		return fileContents;
	};
	auto map = [&](glm::vec2 crntMin, glm::vec2 crntMax, glm::vec2 trgtMin, glm::vec2 trgtMax, glm::vec2 point)
	{
		glm::vec2 slope = (trgtMax - trgtMin) / (crntMax - crntMin);
		glm::vec2 mapped = (point - crntMin) * slope + trgtMin;
		return mapped;
	};
	auto clamp = [&](float min, float max, float i)
	{
		if (max < min) __debugbreak();
		return (i < min) ? min : (i > max) ? max : i;
	};

#pragma endregion

	OGL::TextureFilter pixelArtFilter(GL_NEAREST, GL_REPEAT);

#pragma region Initializing MainMenu

#pragma region Text Atlas Creation
	SPong::TextAtlas textAtlas(readFile("AtlasDetails.txt"), pixelArtFilter);
	OGL::Shader textAtlasProgram("shaders/text");
#pragma endregion

#pragma region Creating Logo
	unsigned int logoVao{}, logoVbo{}, logoEbo{};

	OGL::Texture2D logoTexture("textures/gameLogo.png", 4, pixelArtFilter);

	glCreateVertexArrays(1, &logoVao);

	unsigned int logoEB[6]{
		0, 1, 2,  1, 2, 3
	};

	glCreateBuffers(1, &logoEbo);
	glNamedBufferData(logoEbo, sizeof(unsigned int) * 6, logoEB, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(logoVao, logoEbo);

	int logoScale = 8;

	glm::vec2 logoTL = (glm::ivec2(windowDimension.x, windowDimension.y * 1.5F) - logoTexture.getTextureDimension() * logoScale) / 2;
	glm::vec2 logoBR = (glm::ivec2(windowDimension.x, windowDimension.y * 1.5F) + logoTexture.getTextureDimension() * logoScale) / 2;

	logoTL = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), logoTL);
	logoBR = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), logoBR);

	float logoVB[16]
	{
		logoTL.x, logoTL.y, 0.0F, 0.0F,
		logoBR.x, logoTL.y, 1.0F, 0.0F,
		logoTL.x, logoBR.y, 0.0F, 1.0F,
		logoBR.x, logoBR.y, 1.0F, 1.0F,
	};

	glCreateBuffers(1, &logoVbo);
	glNamedBufferData(logoVbo, sizeof(float) * 16, logoVB, GL_STATIC_DRAW);
	glVertexArrayVertexBuffer(logoVao, 0, logoVbo, 0, sizeof(float) * 4);

	glVertexArrayAttribFormat(logoVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(logoVao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
	glVertexArrayAttribBinding(logoVao, 0, 0);
	glVertexArrayAttribBinding(logoVao, 1, 0);
	glEnableVertexArrayAttrib(logoVao, 0);
	glEnableVertexArrayAttrib(logoVao, 1);

	OGL::Shader logoProgram("shaders/texture");

#pragma endregion

#pragma region Creating Menu

	unsigned int menuVao{}, menuVbo{}, menuEbo{};

	OGL::Texture2D menuTexture("textures/menu-list.png", 4, pixelArtFilter);

	glCreateVertexArrays(1, &menuVao);

	unsigned int menuEB[12]
	{
		0, 1, 2, 1, 2, 3,
		4, 5, 6, 5, 6, 7,
	};

	glCreateBuffers(1, &menuEbo);
	glNamedBufferData(menuEbo, sizeof(unsigned int) * 12, menuEB, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(menuVao, menuEbo);

	int menuScale = 8;

	glm::ivec2 sixthSec = glm::vec2(windowDimension.x, windowDimension.y / 6.0F);

	glm::vec2 menu1TLPos = (sixthSec - glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu1BRPos = (sixthSec + glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu2TLPos = (sixthSec - glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);
	glm::vec2 menu2BRPos = (sixthSec + glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);

	glm::vec2 menu1TL = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), menu1TLPos);
	glm::vec2 menu1BR = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), menu1BRPos);
	glm::vec2 menu2TL = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), menu2TLPos);
	glm::vec2 menu2BR = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), menu2BRPos);

	float menuVB[32]
	{
		menu1TL.x, menu1TL.y, 0.0F + (selectedMenuItem == 0) ? 0.5F : 0.0F, 1.0F,
		menu1BR.x, menu1TL.y, 0.5F + (selectedMenuItem == 0) ? 0.5F : 0.0F, 1.0F,
		menu1TL.x, menu1BR.y, 0.0F + (selectedMenuItem == 0) ? 0.5F : 0.0F, 0.0F,
		menu1BR.x, menu1BR.y, 0.5F + (selectedMenuItem == 0) ? 0.5F : 0.0F, 0.0F,

		menu2TL.x, menu2TL.y, 0.0F + (selectedMenuItem == 1) ? 0.5F : 0.0F, 1.0F,
		menu2BR.x, menu2TL.y, 0.5F + (selectedMenuItem == 1) ? 0.5F : 0.0F, 1.0F,
		menu2TL.x, menu2BR.y, 0.0F + (selectedMenuItem == 1) ? 0.5F : 0.0F, 0.0F,
		menu2BR.x, menu2BR.y, 0.5F + (selectedMenuItem == 1) ? 0.5F : 0.0F, 0.0F,
	};

	glCreateBuffers(1, &menuVbo);
	glNamedBufferData(menuVbo, sizeof(float) * 32, menuVB, GL_DYNAMIC_DRAW);
	glVertexArrayVertexBuffer(menuVao, 0, menuVbo, 0, sizeof(float) * 4);

	glVertexArrayAttribFormat(menuVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(menuVao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
	glVertexArrayAttribBinding(menuVao, 0, 0);
	glVertexArrayAttribBinding(menuVao, 1, 0);
	glEnableVertexArrayAttrib(menuVao, 0);
	glEnableVertexArrayAttrib(menuVao, 1);

	OGL::Shader menuProgram("shaders/texture");

	auto renderMenuItems = [&]()
	{
		float vb[32]
		{
			menu1TL.x, menu1TL.y, 0.0F + ((selectedMenuItem == 0) ? 0.5F : 0.0F), 1.0F,
			menu1BR.x, menu1TL.y, 0.5F + ((selectedMenuItem == 0) ? 0.5F : 0.0F), 1.0F,
			menu1TL.x, menu1BR.y, 0.0F + ((selectedMenuItem == 0) ? 0.5F : 0.0F), 0.0F,
			menu1BR.x, menu1BR.y, 0.5F + ((selectedMenuItem == 0) ? 0.5F : 0.0F), 0.0F,

			menu2TL.x, menu2TL.y, 0.0F + ((selectedMenuItem == 1) ? 0.5F : 0.0F), 1.0F,
			menu2BR.x, menu2TL.y, 0.5F + ((selectedMenuItem == 1) ? 0.5F : 0.0F), 1.0F,
			menu2TL.x, menu2BR.y, 0.0F + ((selectedMenuItem == 1) ? 0.5F : 0.0F), 0.0F,
			menu2BR.x, menu2BR.y, 0.5F + ((selectedMenuItem == 1) ? 0.5F : 0.0F), 0.0F,
		};

		glNamedBufferSubData(menuVbo, 0, sizeof(float) * 32, vb);

		menuProgram.use();
		glBindVertexArray(menuVao);
		menuTexture.bind(0);
		menuProgram.uni1i("tex", 0);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glm::vec2 topTextPos = (menu2TLPos + menu2BRPos) / 2.0F - glm::vec2(textAtlas.getTextWidth("PLAY", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);
		glm::vec2 bottomTextPos = (menu1TLPos + menu1BRPos) / 2.0F - glm::vec2(textAtlas.getTextWidth("EXIT", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);

		textAtlas.renderText("PLAY", topTextPos, menuScale / 3.5F, textAtlasProgram);
		textAtlas.renderText("EXIT", bottomTextPos, menuScale / 3.5F, textAtlasProgram);
	};

#pragma endregion

#pragma endregion

#pragma region Initializing Game Entities

#pragma region Paddle Creation
	constexpr glm::vec2 paddleDimension(20, -100);
	glm::vec2 leftPaddlePos(windowDimension.x * 3 / 160, (windowDimension.y - paddleDimension.y) / 2);
	glm::vec2 rightPaddlePos(windowDimension.x * 157 / 160 - paddleDimension.x, (windowDimension.y - paddleDimension.y) / 2);
	float paddleSpeed = 0.8F;

	unsigned int paddleVao, paddleEbo, paddleVbo;

	OGL::Texture2D paddleTexture("textures/paddle.png", 4, pixelArtFilter);

	glCreateVertexArrays(1, &paddleVao);

	unsigned int eb[6 * 2]
	{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
	};

	glCreateBuffers(1, &paddleEbo);
	glNamedBufferData(paddleEbo, sizeof(unsigned int) * 6 * 2, eb, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(paddleVao, paddleEbo);

	float vb[4 * 4 * 2];

	glCreateBuffers(1, &paddleVbo);
	glNamedBufferData(paddleVbo, sizeof(float) * 4 * 4 * 2, vb, GL_DYNAMIC_DRAW);
	glVertexArrayVertexBuffer(paddleVao, 0, paddleVbo, 0, sizeof(float) * 4);
	glVertexArrayAttribFormat(paddleVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(paddleVao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
	glVertexArrayAttribBinding(paddleVao, 0, 0);
	glVertexArrayAttribBinding(paddleVao, 1, 0);
	glEnableVertexArrayAttrib(paddleVao, 0);
	glEnableVertexArrayAttrib(paddleVao, 1);

	OGL::Shader paddleProgram("shaders/paddle");

	auto updatePaddleVbo = [&]()
	{
		glm::vec2 leftTL = leftPaddlePos;
		glm::vec2 leftBR = leftPaddlePos + paddleDimension;
		glm::vec2 rightTL = rightPaddlePos;
		glm::vec2 rightBR = rightPaddlePos + paddleDimension;
		leftTL = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), leftTL);
		leftBR = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), leftBR);
		rightTL = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), rightTL);
		rightBR = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), rightBR);
		float vb[4 * 4 * 2]
		{
			leftTL.x, leftTL.y, 0, 0,
			leftBR.x, leftTL.y, 1, 0,
			leftTL.x, leftBR.y, 0, 1,
			leftBR.x, leftBR.y, 1, 1,

			rightTL.x, rightTL.y, 1, 0,
			rightBR.x, rightTL.y, 0, 0,
			rightTL.x, rightBR.y, 1, 1,
			rightBR.x, rightBR.y, 0, 1,
		};

		glNamedBufferSubData(paddleVbo, 0, sizeof(float) * 4 * 4 * 2, vb);
	};
#pragma endregion

#pragma region Ball Creation
	float ballRadius = 10.0F;
	glm::vec2 ballPos(windowDimension / 2);
	glm::vec2 oldBallPos(ballPos);
	glm::vec2 ballAcc(0.7f, -1.0f);
	float maxVelocity = 24.0F;
	float initialialVel = 1.0F;
	float accMul = 5.0F;
	ballAcc = glm::normalize(ballAcc) * accMul;
	unsigned int ballVao, ballVbo, ballEbo;

	glCreateVertexArrays(1, &ballVao);

	unsigned int eb1[6]
	{
		0, 1, 2,  1, 2, 3
	};

	glCreateBuffers(1, &ballEbo);
	glNamedBufferData(ballEbo, sizeof(unsigned int) * 6, eb1, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(ballVao, ballEbo);

	float* vb1[4 * 4]{};
	glCreateBuffers(1, &ballVbo);
	glNamedBufferData(ballVbo, sizeof(float) * 4 * 4, vb1, GL_DYNAMIC_DRAW);
	glVertexArrayVertexBuffer(ballVao, 0, ballVbo, 0, sizeof(float) * 4);
	glVertexArrayAttribFormat(ballVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(ballVao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
	glVertexArrayAttribBinding(ballVao, 0, 0);
	glVertexArrayAttribBinding(ballVao, 1, 0);
	glEnableVertexArrayAttrib(ballVao, 0);
	glEnableVertexArrayAttrib(ballVao, 1);

	OGL::Shader ballProgram("shaders/ball");

	OGL::Texture2D ballTexture("textures/ball.png", 4, pixelArtFilter);

	auto updateBallVbo = [&]()
	{
		glm::vec2 ballTL = ballPos - glm::vec2(ballRadius, -ballRadius);
		glm::vec2 ballBR = ballPos + glm::vec2(ballRadius, -ballRadius);
		ballTL = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), ballTL);
		ballBR = map(glm::vec2(0, 0), windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), ballBR);
		float vb[4 * 4]
		{
			ballTL.x, ballTL.y, 0, 1,
			ballBR.x, ballTL.y, 1, 1,
			ballTL.x, ballBR.y, 0, 0,
			ballBR.x, ballBR.y, 1, 0,
		};

		glNamedBufferSubData(ballVbo, 0, sizeof(float) * 4 * 4, vb);
	};

	auto resetBall = [&]()
	{
		std::srand(static_cast<unsigned int>(std::time(0)));
		bool dir = std::rand() % 2;

		int random = std::rand();

		float crntOutMin = 0.0F, crntOutMax = (float)RAND_MAX, trgtAngMin = glm::radians(-70.0F), trgtAngMax = glm::radians(70.0F);

		float slope = (trgtAngMax - trgtAngMin) / (float)(crntOutMax - crntOutMin);
		float mapped = (random - crntOutMin) * slope + trgtAngMin;

		ballPos = windowDimension / 2;
		oldBallPos = ballPos - glm::vec2(glm::cos(mapped) * (dir ? 1 : -1), glm::sin(mapped)) * initialialVel;
		ballAcc = glm::vec2(glm::cos(mapped) * (dir ? 1 : -1), glm::sin(mapped)) * accMul;
	};
#pragma endregion

#pragma region Separation Line Creation

	glm::vec4 separationLineColor(0.4F, 0.4F, 0.4F, 1.0F);
	unsigned int separationVao, separationVbo, separationEbo;

	glCreateVertexArrays(1, &separationVao);

	constexpr int separationLineWidth = 15;

	glm::vec2 sTL = glm::vec2((windowDimension.x - separationLineWidth) / 2.0f, windowDimension.y);
	glm::vec2 sBR = glm::vec2((windowDimension.x + separationLineWidth) / 2.0f, 0.0F);

	sTL = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), sTL);
	sBR = map(glm::vec2(0), windowDimension, glm::vec2(-1), glm::vec2(1), sBR);

	float sVb[8]
	{
		sTL.x, sTL.y,
		sBR.x, sTL.y,
		sTL.x, sBR.y,
		sBR.x, sBR.y,
	};

	glCreateBuffers(1, &separationVbo);
	glNamedBufferData(separationVbo, sizeof(float) * 2 * 4, sVb, GL_STATIC_DRAW);
	glVertexArrayVertexBuffer(separationVao, 0, separationVbo, 0, sizeof(float) * 2);
	glVertexArrayAttribFormat(separationVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(separationVao, 0, 0);
	glEnableVertexArrayAttrib(separationVao, 0);

	unsigned int sEb[6]
	{
		0, 1, 2, 1, 2, 3
	};

	glCreateBuffers(1, &separationEbo);
	glNamedBufferData(separationEbo, sizeof(unsigned int) * 6, sEb, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(separationVao, separationEbo);

	OGL::Shader separationLineProgram("shaders/separation");

#pragma endregion

#pragma endregion

	logoProgram.use();
	logoProgram.uni1i("tex", 0);

	resetBall();
	constexpr float scoreFontSize = 32.0F / 16.0F;

	unsigned int leftScore = 0, rightScore = 0;
	glm::vec2 leftScorePos(32, windowDimension.y - 64);
	glm::vec2 rightScorePos(windowDimension.x / 2.0F + 32, windowDimension.y - 64);

	glm::vec4 scoreColor(0.65F, 0.65F, 0.65F, 1.0F);
	auto renderScore = [&]()
	{
		std::string leftScoreStr = "Score: ";
		std::string rightScoreStr = "Score: ";
		if (leftScore < 10) leftScoreStr = leftScoreStr + std::string("0") + std::to_string(leftScore);
		else leftScoreStr += std::to_string(leftScore);
		if (rightScore < 10) rightScoreStr = rightScoreStr + std::string("0") + std::to_string(rightScore);
		else rightScoreStr += std::to_string(rightScore);

		textAtlas.renderText(leftScoreStr, leftScorePos, scoreFontSize, textAtlasProgram);
		textAtlas.renderText(rightScoreStr, rightScorePos, scoreFontSize, textAtlasProgram);
	};

	auto resetGame = [&]()
	{
		gameState = GS::MAIN_MENU;
		resetBall();
		leftScore = 0;
		rightScore = 0;
		leftPaddlePos = glm::vec2(windowDimension.x * 3 / 160, (windowDimension.y - paddleDimension.y) / 2);
		rightPaddlePos = glm::vec2(windowDimension.x * 157 / 160 - paddleDimension.x, (windowDimension.y - paddleDimension.y) / 2);
	};
	double oldTime = glfwGetTime();

	bool leftWin = false;

	while (!glfwWindowShouldClose(window))
	{
		oldTime = glfwGetTime();
		
		if (gameState == GS::PLAY)
		{
#pragma region Updating Vbo and Drawing
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			separationLineProgram.use();
			separationLineProgram.uni4f("color", separationLineColor);
			glBindVertexArray(separationVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			paddleProgram.use();
			paddleProgram.uni1i("tex", 0);
			paddleTexture.bind(0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			ballProgram.use();
			ballProgram.uni1i("tex", 1);
			ballTexture.bind(1);
			glBindVertexArray(ballVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			glfwSwapBuffers(window);
#pragma endregion

#pragma region Updating Paddle and Ball Position
			glfwPollEvents();
			double dt = glfwGetTime() - oldTime;

#pragma region Updating Ball Position
			glm::vec2 newBallPos = ballPos * 2.0f - oldBallPos + ballAcc * (float)(dt * dt);
			oldBallPos = ballPos; ballPos = newBallPos;

			glm::vec2 deltaP = ballPos - oldBallPos;
			if (glm::length(deltaP) > maxVelocity)
			{
				float angle = glm::atan(deltaP.y, deltaP.x);
				oldBallPos = ballPos - glm::vec2(glm::cos(angle), glm::sin(angle)) * maxVelocity;
			}
#pragma endregion

#pragma region Updating Paddle Position
			leftPaddlePos.y += (glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S)) * paddleSpeed;
			rightPaddlePos.y += (glfwGetKey(window, GLFW_KEY_UP) - glfwGetKey(window, GLFW_KEY_DOWN)) * paddleSpeed;

			if (leftPaddlePos.y > windowDimension.y) leftPaddlePos.y = windowDimension.y;
			if (rightPaddlePos.y > windowDimension.y) rightPaddlePos.y = windowDimension.y;

			if (leftPaddlePos.y + paddleDimension.y < 0) leftPaddlePos.y = -paddleDimension.y;
			if (rightPaddlePos.y + paddleDimension.y < 0) rightPaddlePos.y = -paddleDimension.y;
#pragma endregion

#pragma region Checking Ball Collision

#pragma region Bottom Wall Collision
			if (ballPos.y - ballRadius < 0)
			{
				glm::vec2 delta = ballPos - oldBallPos;

				glm::vec2 offsetNeeded = glm::vec2(0, -(ballPos.y - ballRadius));
				offsetNeeded.x = (delta.y / offsetNeeded.y) * delta.x;

				ballPos += offsetNeeded; oldBallPos = ballPos - glm::vec2(delta.x, -delta.y);
				ballAcc.y *= -1;
			}
#pragma endregion

#pragma region Top Wall Collision
			else if (ballPos.y + ballRadius > windowDimension.y)
			{
				glm::vec2 delta = ballPos - oldBallPos;

				glm::vec2 offsetNeeded = glm::vec2(0, windowDimension.y - (ballPos.y + ballRadius));
				offsetNeeded.x = (delta.y / offsetNeeded.y) * delta.x;

				ballPos += offsetNeeded; oldBallPos = ballPos - glm::vec2(delta.x, -delta.y);
				ballAcc.y *= -1;
			}
#pragma endregion

#pragma region Right Paddle Collision
			glm::vec2 nearestPoint(
				clamp(rightPaddlePos.x, rightPaddlePos.x + paddleDimension.x, ballPos.x),
				clamp(rightPaddlePos.y + paddleDimension.y, rightPaddlePos.y, ballPos.y)
			);
			glm::vec2 d = nearestPoint - ballPos;
			if (d.x * d.x + d.y * d.y <= ballRadius * ballRadius)
			{
				glm::vec2 delta = ballPos - oldBallPos;

				glm::vec2 offsetNeeded = glm::vec2(rightPaddlePos.x - (ballPos.x + ballRadius), 0);
				offsetNeeded.y = (delta.x / offsetNeeded.x) * delta.y;

				ballPos += offsetNeeded;
				ballAcc.x *= -1;

				float crntYMin = rightPaddlePos.y + paddleDimension.y - ballRadius / 4.0f;
				float crntYMax = rightPaddlePos.y + ballRadius / 4.0f;
				constexpr float trgtYMin = glm::radians(-65.0F);
				constexpr float trgtYMax = glm::radians(+65.0F);

				float slope = (trgtYMax - trgtYMin) / (crntYMax - crntYMin);
				float mapped = (ballPos.y - crntYMin) * slope + trgtYMin; // Got the angle

				float distance = glm::length(delta);

				oldBallPos = ballPos - glm::vec2(-glm::cos(mapped), glm::sin(mapped)) * distance;
			}
#pragma endregion

#pragma region Left Paddle Collision
			else
			{
				nearestPoint = glm::vec2(
					clamp(leftPaddlePos.x, leftPaddlePos.x + paddleDimension.x, ballPos.x),
					clamp(leftPaddlePos.y + paddleDimension.y, leftPaddlePos.y, ballPos.y)
				);
				d = nearestPoint - ballPos;

				if (d.x * d.x + d.y * d.y <= ballRadius * ballRadius)
				{
					glm::vec2 delta = ballPos - oldBallPos;

					glm::vec2 offsetNeeded = glm::vec2(leftPaddlePos.x + paddleDimension.x + ballRadius - ballPos.x, 0);
					offsetNeeded.y = (delta.x / offsetNeeded.x) * delta.y;

					ballPos += offsetNeeded; oldBallPos = ballPos - glm::vec2(-delta.x, delta.y);
					ballAcc.x *= -1;

					float crntYMin = leftPaddlePos.y + paddleDimension.y - ballRadius / 4.0f;
					float crntYMax = leftPaddlePos.y + ballRadius / 4.0f;
					constexpr float trgtYMin = glm::radians(-65.0F);
					constexpr float trgtYMax = glm::radians(+65.0F);

					float slope = (trgtYMax - trgtYMin) / (crntYMax - crntYMin);
					float mapped = (ballPos.y - crntYMin) * slope + trgtYMin; // Got the angle

					float distance = glm::length(delta);

					oldBallPos = ballPos - glm::vec2(glm::cos(mapped), glm::sin(mapped)) * distance;
				}
			}
#pragma endregion

#pragma region Outer Bounds
			if (ballPos.x - ballRadius < 0)
			{
				rightScore++;
				resetBall();
			}
			else if (ballPos.x + ballRadius > windowDimension.x)
			{
				leftScore++;
				resetBall();
			}
#pragma endregion

#pragma endregion

#pragma region Reseting the Game If required
			if (glfwGetKey(window, GLFW_KEY_R)) resetGame();
#pragma endregion

#pragma endregion

			if (leftScore == 8)
			{
				leftWin = true;
				gameState = GS::WIN;
			}
			else if (rightScore == 8)
			{
				leftWin = false;
				gameState = GS::WIN;
			}
		}
		else if (gameState == GS::MAIN_MENU)
		{
			glClear(GL_COLOR_BUFFER_BIT);

			logoProgram.use();
			logoProgram.uni1i("tex", 0);
			glBindVertexArray(logoVao);
			logoTexture.bind(0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderMenuItems();

			glfwSwapBuffers(window);
			resetGame();
			glfwPollEvents();
		}
		else if (gameState == GS::PAUSE)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			separationLineProgram.use();
			separationLineProgram.uni4f("color", separationLineColor);
			glBindVertexArray(separationVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			paddleProgram.use();
			paddleProgram.uni1i("tex", 0);
			paddleTexture.bind(0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			ballProgram.use();
			ballProgram.uni1i("tex", 1);
			ballTexture.bind(1);
			glBindVertexArray(ballVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			textAtlas.renderText("PAUSED!", glm::vec2((windowDimension.x - textAtlas.getTextWidth("PAUSED!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F, textAtlasProgram);

			glfwSwapBuffers(window);

			if (glfwGetKey(window, GLFW_KEY_R)) resetGame();

			glfwPollEvents();
		}
		else if (gameState == GS::WIN)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			separationLineProgram.use();
			separationLineProgram.uni4f("color", separationLineColor);
			glBindVertexArray(separationVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			paddleProgram.use();
			paddleProgram.uni1i("tex", 0);
			paddleTexture.bind(0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			ballProgram.use();
			ballProgram.uni1i("tex", 1);
			ballTexture.bind(1);
			glBindVertexArray(ballVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			if (leftWin)
				textAtlas.renderText("Left Won!", glm::vec2((windowDimension.x - textAtlas.getTextWidth("Left Won!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F, textAtlasProgram);
			else
				textAtlas.renderText("Right Won!", glm::vec2((windowDimension.x - textAtlas.getTextWidth("Right Won!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F, textAtlasProgram);

			glfwSwapBuffers(window);

			if (glfwGetKey(window, GLFW_KEY_R)) resetGame();

			glfwPollEvents();
		}
	}

#pragma region Releasing some memory
	glDeleteVertexArrays(1, &paddleVao);
	glDeleteBuffers(1, &paddleVbo);
	glDeleteBuffers(1, &paddleEbo);

	glDeleteVertexArrays(1, &ballVao);
	glDeleteBuffers(1, &ballVbo);
	glDeleteBuffers(1, &ballEbo);

	glDeleteVertexArrays(1, &separationVao);
	glDeleteBuffers(1, &separationVbo);
	glDeleteBuffers(1, &separationEbo);

	glfwDestroyWindow(window);
	glfwTerminate();
#pragma endregion
}