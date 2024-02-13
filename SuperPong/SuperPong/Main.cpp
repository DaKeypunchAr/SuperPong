#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "STB/image.h"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "TextAtlas/TextAtlas.hpp"
#include "VAO/VAO.hpp"
#include "Game.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <array>

SPong::GameData game;

void debugCallback(int errorCode, const char* description)
{
	std::cerr << "GLFW Error Description:\n" << description << '\n';
	__debugbreak();
}
void __stdcall debugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam)
{
	std::string sourceStr;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		sourceStr = "Source API";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		sourceStr = "Source Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		sourceStr = "Source Other";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		sourceStr = "Source Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		sourceStr = "Source Third Party";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		sourceStr = "Source Window System";
		break;
	default:
		sourceStr = "Unknown Source";
	}

	std::string typeStr;
	switch (type)
	{
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeStr = "Type Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_ERROR:
		typeStr = "Type Error";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeStr = "Type Marker";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeStr = "Type Other";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeStr = "Type Performance";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeStr = "Type Pop Group";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeStr = "Type Portability";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeStr = "Type Push Group";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeStr = "Type Undefined Behaviour";
		break;
	default:
		typeStr = "Unknown Type";
	}

	std::string idStr = "Id " + std::to_string(id);

	std::string severityStr;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityStr = "Severity High";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severityStr = "Severity Low";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityStr = "Severity Medium";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityStr = "Severity Notification";
		break;
	default:
		severityStr = "Unknown Severity";
	}

	std::cerr << "DEBUG CALLBACK:\n\t" << sourceStr << "\n\t" << typeStr << "\n\t" << idStr << "\n\t" << severityStr << '\n' << "MESSAGE:\n\t" << message << '\n';
	if (type != GL_DEBUG_TYPE_PERFORMANCE)
	{
		__debugbreak();
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		game.selectedMenuItem--;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		game.selectedMenuItem++;
	}
	if (game.selectedMenuItem < 0) game.selectedMenuItem = 1;
	if (game.selectedMenuItem > 1) game.selectedMenuItem = 0;

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (game.gameState == GS::MAIN_MENU)
		{
			if (game.selectedMenuItem == 1) glfwSetWindowShouldClose(window, true);
			if (game.selectedMenuItem == 0) game.gameState = GS::PLAY;
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (game.gameState == GS::MAIN_MENU)
			glfwSetWindowShouldClose(window, true);
		else if (game.gameState == GS::PLAY)
			game.gameState = GS::PAUSE;
		else if (game.gameState == GS::PAUSE)
			game.gameState = GS::PLAY;
	}
}

void initializeOpenGL()
{
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

	game.window = glfwCreateWindow(game.windowDimension.x, game.windowDimension.y, "SuperPong", nullptr, nullptr);

	if (!game.window)
	{
		glfwTerminate();
		__debugbreak();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(game.window);
	glfwSetKeyCallback(game.window, keyCallback);

	if (glewInit())
	{
		glfwDestroyWindow(game.window);
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

	game.textureShader.initialize("shaders/texture");
	game.solidShader.initialize("shaders/solid");

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

static std::string readFile(const std::string& fileLocation)
{
	std::ifstream fileReader(fileLocation);
	std::string fileContents;
	for (std::string line; std::getline(fileReader, line); fileContents += '\n') fileContents += line;
	return fileContents;
}

static constexpr glm::vec2 map(glm::vec2 crntMin, glm::vec2 crntMax, glm::vec2 trgtMin, glm::vec2 trgtMax, glm::vec2 point)
{
	glm::vec2 slope = (trgtMax - trgtMin) / (crntMax - crntMin);
	glm::vec2 mapped = (point - crntMin) * slope + trgtMin;
	return mapped;
}

int main()
{
	initializeOpenGL();

	SPong::TextAtlas textAtlas(readFile("AtlasDetails.txt"), game);

	OGL::Texture2D logoTexture("textures/gameLogo.png", 4, game.textureFilters);
	OGL::Texture2D menuTexture("textures/menu-list.png", 4, game.textureFilters);
	OGL::Texture2D paddleTexture("textures/paddle.png", 4, game.textureFilters);
	OGL::Texture2D ballTexture("textures/ball.png", 4, game.textureFilters);

	int logoScale = 8;

	glm::vec2 logoTL = (glm::ivec2(game.windowDimension.x, game.windowDimension.y * 1.5F) - logoTexture.getTextureDimension() * logoScale) / 2;
	glm::vec2 logoBR = (glm::ivec2(game.windowDimension.x, game.windowDimension.y * 1.5F) + logoTexture.getTextureDimension() * logoScale) / 2;

	logoTL = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), logoTL);
	logoBR = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), logoBR);

	std::vector<float> logoPosVB
	{
		logoTL.x, logoTL.y,
		logoBR.x, logoTL.y,
		logoTL.x, logoBR.y,
		logoBR.x, logoBR.y,
	};
	std::vector<float> logoTexVB
	{
		0.0F, 0.0F,
		1.0F, 0.0F,
		0.0F, 1.0F,
		1.0F, 1.0F,
	};

	std::vector<unsigned int> logoEB
	{
		0, 1, 2,
		1, 2, 3
	};

	OGL::VAO logoVAO(game.logoVBOsConfigs, { 8, 8 }, 6);
	logoVAO.updateEB(logoEB, 0);
	logoVAO.updateVB(logoPosVB, 0, 0);
	logoVAO.updateVB(logoTexVB, 1, 0);

	int menuScale = 8;

	glm::ivec2 sixthSec = glm::vec2(game.windowDimension.x, game.windowDimension.y / 6.0F);

	glm::vec2 menu1TLPos = (sixthSec - glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu1BRPos = (sixthSec + glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu2TLPos = (sixthSec - glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);
	glm::vec2 menu2BRPos = (sixthSec + glm::ivec2(menuTexture.getTextureDimension().x / 2, menuTexture.getTextureDimension().y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);

	glm::vec2 menu1TL = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), menu1TLPos);
	glm::vec2 menu1BR = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), menu1BRPos);
	glm::vec2 menu2TL = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), menu2TLPos);
	glm::vec2 menu2BR = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), menu2BRPos);

	std::vector<float> menuPosVB
	{
		menu1TL.x, menu1TL.y,
		menu1BR.x, menu1TL.y,
		menu1TL.x, menu1BR.y,
		menu1BR.x, menu1BR.y,

		menu2TL.x, menu2TL.y,
		menu2BR.x, menu2TL.y,
		menu2TL.x, menu2BR.y,
		menu2BR.x, menu2BR.y,
	};
	std::vector<float> menuTexVB
	{
		0.0F + (game.selectedMenuItem == 0) ? 0.5F : 0.0F, 1.0F,
		0.5F + (game.selectedMenuItem == 0) ? 0.5F : 0.0F, 1.0F,
		0.0F + (game.selectedMenuItem == 0) ? 0.5F : 0.0F, 0.0F,
		0.5F + (game.selectedMenuItem == 0) ? 0.5F : 0.0F, 0.0F,

		0.0F + (game.selectedMenuItem == 1) ? 0.5F : 0.0F, 1.0F,
		0.5F + (game.selectedMenuItem == 1) ? 0.5F : 0.0F, 1.0F,
		0.0F + (game.selectedMenuItem == 1) ? 0.5F : 0.0F, 0.0F,
		0.5F + (game.selectedMenuItem == 1) ? 0.5F : 0.0F, 0.0F,
	};

	std::vector<unsigned int> menuEB
	{
		0, 1, 2, 1, 2, 3,
		4, 5, 6, 5, 6, 7,
	};

	OGL::VAO menuVAO(game.menuVBOsConfigs, { 16, 16 }, 12);
	menuVAO.updateVB(menuPosVB, 0, 0);
	menuVAO.updateVB(menuTexVB, 1, 0);
	menuVAO.updateEB(menuEB, 0);

	constexpr glm::vec2 paddleDimension(20, -100);
	glm::vec2 leftPaddlePos(game.windowDimension.x * 3 / 160, (game.windowDimension.y - paddleDimension.y) / 2);
	glm::vec2 rightPaddlePos(game.windowDimension.x * 157 / 160 - paddleDimension.x, (game.windowDimension.y - paddleDimension.y) / 2);
	float paddleSpeed = 0.8F;

	std::vector<unsigned int> paddleEB
	{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
	};

	std::vector<float> paddleTexVB
	{
		0, 0,
		1, 0,
		0, 1,
		1, 1,

		1, 0,
		0, 0,
		1, 1,
		0, 1,
	};

	OGL::VAO paddleVAO(game.textureVBOsConfigs, { 16, 16 }, 12);
	paddleVAO.updateVB(paddleTexVB, 1, 0);
	paddleVAO.updateEB(paddleEB, 0);

	float ballRadius = 10.0F;
	glm::vec2 ballPos(game.windowDimension / 2);
	glm::vec2 oldBallPos(ballPos);
	glm::vec2 ballAcc(0.7f, -1.0f);
	float maxVelocity = 24.0F;
	float initialialVel = 1.0F;
	float accMul = 5.0F;
	ballAcc = glm::normalize(ballAcc) * accMul;

	std::vector<unsigned int> ballEB
	{
		0, 1, 2,  1, 2, 3
	};

	std::vector<float> ballTexVB
	{
		0, 1,
		1, 1,
		0, 0,
		1, 0,
	};

	OGL::VAO ballVAO(game.textureVBOsConfigs, { 8, 8 }, 6);
	ballVAO.updateVB(ballTexVB, 1, 0);
	ballVAO.updateEB(ballEB, 0);

	glm::vec4 separationLineColor(0.4F, 0.4F, 0.4F, 1.0F);

	constexpr int separationLineWidth = 15;

	glm::vec2 sTL = glm::vec2((game.windowDimension.x - separationLineWidth) / 2.0f, game.windowDimension.y);
	glm::vec2 sBR = glm::vec2((game.windowDimension.x + separationLineWidth) / 2.0f, 0.0F);

	sTL = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), sTL);
	sBR = map(glm::vec2(0), game.windowDimension, glm::vec2(-1), glm::vec2(1), sBR);

	std::vector<float> separationVB
	{
		sTL.x, sTL.y,
		sBR.x, sTL.y,
		sTL.x, sBR.y,
		sBR.x, sBR.y,
	};

	std::vector<unsigned int> separationEB
	{
		0, 1, 2, 1, 2, 3
	};

	OGL::VAO separationVAO(game.solidVBOsConfigs, { 8 }, 6);
	separationVAO.updateEB(separationEB, 0);
	separationVAO.updateVB(separationVB, 0, 0);

	auto renderMenuItems = [&]()
		{
			std::vector<float> texVB
			{
				0.0F + ((game.selectedMenuItem == 0) ? 0.5F : 0.0F), 1.0F,
				0.5F + ((game.selectedMenuItem == 0) ? 0.5F : 0.0F), 1.0F,
				0.0F + ((game.selectedMenuItem == 0) ? 0.5F : 0.0F), 0.0F,
				0.5F + ((game.selectedMenuItem == 0) ? 0.5F : 0.0F), 0.0F,

				0.0F + ((game.selectedMenuItem == 1) ? 0.5F : 0.0F), 1.0F,
				0.5F + ((game.selectedMenuItem == 1) ? 0.5F : 0.0F), 1.0F,
				0.0F + ((game.selectedMenuItem == 1) ? 0.5F : 0.0F), 0.0F,
				0.5F + ((game.selectedMenuItem == 1) ? 0.5F : 0.0F), 0.0F,
			};

			menuVAO.updateVB(texVB, 1, 0);

			game.textureShader.use();
			menuVAO.bind();
			menuTexture.bind(0);
			game.textureShader.uni1i("tex", 0);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

			glm::vec2 topTextPos = (menu2TLPos + menu2BRPos) / 2.0F - glm::vec2(textAtlas.getTextWidth("PLAY", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);
			glm::vec2 bottomTextPos = (menu1TLPos + menu1BRPos) / 2.0F - glm::vec2(textAtlas.getTextWidth("EXIT", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);

			textAtlas.renderText("PLAY", topTextPos, menuScale / 3.5F, game.textureShader);
			textAtlas.renderText("EXIT", bottomTextPos, menuScale / 3.5F, game.textureShader);
		};
	auto updatePaddleVbo = [&]()
		{
			glm::vec2 leftTL = leftPaddlePos;
			glm::vec2 leftBR = leftPaddlePos + paddleDimension;
			glm::vec2 rightTL = rightPaddlePos;
			glm::vec2 rightBR = rightPaddlePos + paddleDimension;
			leftTL = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), leftTL);
			leftBR = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), leftBR);
			rightTL = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), rightTL);
			rightBR = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), rightBR);
			std::vector<float> posVB
			{
				leftTL.x, leftTL.y,
				leftBR.x, leftTL.y,
				leftTL.x, leftBR.y,
				leftBR.x, leftBR.y,

				rightTL.x, rightTL.y,
				rightBR.x, rightTL.y,
				rightTL.x, rightBR.y,
				rightBR.x, rightBR.y,
			};

			paddleVAO.updateVB(posVB, 0, 0);
		};
	auto updateBallVbo = [&]()
		{
			glm::vec2 ballTL = ballPos - glm::vec2(ballRadius, -ballRadius);
			glm::vec2 ballBR = ballPos + glm::vec2(ballRadius, -ballRadius);
			ballTL = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), ballTL);
			ballBR = map(glm::vec2(0, 0), game.windowDimension, glm::vec2(-1, -1), glm::vec2(1, 1), ballBR);
			std::vector<float> posVB
			{
				ballTL.x, ballTL.y,
				ballBR.x, ballTL.y,
				ballTL.x, ballBR.y,
				ballBR.x, ballBR.y,
			};
			ballVAO.updateVB(posVB, 0, 0);
		};
	auto resetBall = [&]()
		{
			std::srand(static_cast<unsigned int>(std::time(0)));
			bool dir = std::rand() % 2;

			int random = std::rand();

			float crntOutMin = 0.0F, crntOutMax = (float)RAND_MAX, trgtAngMin = glm::radians(-70.0F), trgtAngMax = glm::radians(70.0F);

			float slope = (trgtAngMax - trgtAngMin) / (float)(crntOutMax - crntOutMin);
			float mapped = (random - crntOutMin) * slope + trgtAngMin;

			ballPos = game.windowDimension / 2;
			oldBallPos = ballPos - glm::vec2(glm::cos(mapped) * (dir ? 1 : -1), glm::sin(mapped)) * initialialVel;
			ballAcc = glm::vec2(glm::cos(mapped) * (dir ? 1 : -1), glm::sin(mapped)) * accMul;
		};

	resetBall();
	constexpr float scoreFontSize = 32.0F / 16.0F;

	glm::vec2 leftScorePos(32, game.windowDimension.y - 64);

	glm::vec2 rightScorePos(game.windowDimension.x / 2.0F + 32, game.windowDimension.y - 64);

	glm::vec4 scoreColor(0.65F, 0.65F, 0.65F, 1.0F);
	auto renderScore = [&]()
	{
		std::string leftScoreStr = "Score: ";
		std::string rightScoreStr = "Score: ";
		if (game.leftPaddleScore < 10) leftScoreStr = leftScoreStr + std::string("0") + std::to_string(game.leftPaddleScore);
		else leftScoreStr += std::to_string(game.leftPaddleScore);
		if (game.rightPaddleScore < 10) rightScoreStr = rightScoreStr + std::string("0") + std::to_string(game.rightPaddleScore);
		else rightScoreStr += std::to_string(game.rightPaddleScore);

		textAtlas.renderText(leftScoreStr, leftScorePos, scoreFontSize, game.textureShader);
		textAtlas.renderText(rightScoreStr, rightScorePos, scoreFontSize, game.textureShader);
	};

	auto resetGame = [&]()
	{
		game.gameState = GS::MAIN_MENU;
		resetBall();
		game.leftPaddleScore = 0;
		game.rightPaddleScore = 0;
		leftPaddlePos = glm::vec2(game.windowDimension.x * 3 / 160, (game.windowDimension.y - paddleDimension.y) / 2);
		rightPaddlePos = glm::vec2(game.windowDimension.x * 157 / 160 - paddleDimension.x, (game.windowDimension.y - paddleDimension.y) / 2);
	};
	double oldTime = glfwGetTime();

	while (!glfwWindowShouldClose(game.window))
	{
		oldTime = glfwGetTime();
		
		if (game.gameState == GS::PLAY)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			game.solidShader.use();
			game.solidShader.uni4f("color", separationLineColor);
			separationVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			game.textureShader.use();
			game.textureShader.uni1i("tex", 0);
			paddleTexture.bind(0);
			paddleVAO.bind();
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			game.textureShader.use();
			game.textureShader.uni1i("tex", 1);
			ballTexture.bind(1);
			ballVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			glfwSwapBuffers(game.window);
			glfwPollEvents();
			double dt = glfwGetTime() - oldTime;

			glm::vec2 newBallPos = ballPos * 2.0f - oldBallPos + ballAcc * (float)(dt * dt);
			oldBallPos = ballPos; ballPos = newBallPos;

			glm::vec2 deltaP = ballPos - oldBallPos;
			if (glm::length(deltaP) > maxVelocity)
			{
				float angle = glm::atan(deltaP.y, deltaP.x);
				oldBallPos = ballPos - glm::vec2(glm::cos(angle), glm::sin(angle)) * maxVelocity;
			}

			leftPaddlePos.y += (glfwGetKey(game.window, GLFW_KEY_W) - glfwGetKey(game.window, GLFW_KEY_S)) * paddleSpeed;
			rightPaddlePos.y += (glfwGetKey(game.window, GLFW_KEY_UP) - glfwGetKey(game.window, GLFW_KEY_DOWN)) * paddleSpeed;

			if (leftPaddlePos.y > game.windowDimension.y) leftPaddlePos.y = game.windowDimension.y;
			if (rightPaddlePos.y > game.windowDimension.y) rightPaddlePos.y = game.windowDimension.y;

			if (leftPaddlePos.y + paddleDimension.y < 0) leftPaddlePos.y = -paddleDimension.y;
			if (rightPaddlePos.y + paddleDimension.y < 0) rightPaddlePos.y = -paddleDimension.y;

			if (ballPos.y - ballRadius < 0)
			{
				glm::vec2 delta = ballPos - oldBallPos;

				glm::vec2 offsetNeeded = glm::vec2(0, -(ballPos.y - ballRadius));
				offsetNeeded.x = (delta.y / offsetNeeded.y) * delta.x;

				ballPos += offsetNeeded; oldBallPos = ballPos - glm::vec2(delta.x, -delta.y);
				ballAcc.y *= -1;
			}

			else if (ballPos.y + ballRadius > game.windowDimension.y)
			{
				glm::vec2 delta = ballPos - oldBallPos;

				glm::vec2 offsetNeeded = glm::vec2(0, game.windowDimension.y - (ballPos.y + ballRadius));
				offsetNeeded.x = (delta.y / offsetNeeded.y) * delta.x;

				ballPos += offsetNeeded; oldBallPos = ballPos - glm::vec2(delta.x, -delta.y);
				ballAcc.y *= -1;
			}

			glm::vec2 nearestPoint = glm::clamp(ballPos, glm::vec2(rightPaddlePos.x, rightPaddlePos.y + paddleDimension.y),
				glm::vec2(rightPaddlePos.x + paddleDimension.x, rightPaddlePos.y));
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
			else
			{
				nearestPoint = glm::clamp(ballPos, glm::vec2(leftPaddlePos.x, leftPaddlePos.y + paddleDimension.y),
					glm::vec2(leftPaddlePos.x + paddleDimension.x, leftPaddlePos.y));
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

			if (ballPos.x - ballRadius < 0)
			{
				game.rightPaddleScore++;
				resetBall();
			}
			else if (ballPos.x + ballRadius > game.windowDimension.x)
			{
				game.leftPaddleScore++;
				resetBall();
			}

			if (glfwGetKey(game.window, GLFW_KEY_R)) resetGame();

			if (game.leftPaddleScore == 8)
			{
				game.leftHasWon = true;
				game.gameState = GS::WIN;
			}
			else if (game.rightPaddleScore == 8)
			{
				game.leftHasWon = false;
				game.gameState = GS::WIN;
			}
		}
		else if (game.gameState == GS::MAIN_MENU)
		{
			glClear(GL_COLOR_BUFFER_BIT);

			game.textureShader.use();
			game.textureShader.uni1i("tex", 0);
			logoVAO.bind();
			logoTexture.bind(0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderMenuItems();

			glfwSwapBuffers(game.window);
			resetGame();
			glfwPollEvents();
		}
		else if (game.gameState == GS::PAUSE)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			game.solidShader.use();
			game.solidShader.uni4f("color", separationLineColor);
			separationVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			game.textureShader.use();
			game.textureShader.uni1i("tex", 0);
			paddleTexture.bind(0);
			paddleVAO.bind();
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			game.textureShader.use();
			game.textureShader.uni1i("tex", 1);
			ballTexture.bind(1);
			ballVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			textAtlas.renderText("PAUSED!", glm::vec2((game.windowDimension.x - textAtlas.getTextWidth("PAUSED!", 6.0F)) / 2.0F, (game.windowDimension.y - 6.0F) / 2.0F), 6.0F, game.textureShader);

			glfwSwapBuffers(game.window);

			if (glfwGetKey(game.window, GLFW_KEY_R)) resetGame();

			glfwPollEvents();
		}
		else if (game.gameState == GS::WIN)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			game.solidShader.use();
			game.solidShader.uni4f("color", separationLineColor);
			separationVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			game.textureShader.use();
			game.textureShader.uni1i("tex", 0);
			paddleTexture.bind(0);
			paddleVAO.bind();
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			game.textureShader.use();
			game.textureShader.uni1i("tex", 1);
			ballTexture.bind(1);
			ballVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			if (game.leftHasWon)
				textAtlas.renderText("Left Won!", glm::vec2((game.windowDimension.x - textAtlas.getTextWidth("Left Won!", 6.0F)) / 2.0F, (game.windowDimension.y - 6.0F) / 2.0F), 6.0F, game.textureShader);
			else
				textAtlas.renderText("Right Won!", glm::vec2((game.windowDimension.x - textAtlas.getTextWidth("Right Won!", 6.0F)) / 2.0F, (game.windowDimension.y - 6.0F) / 2.0F), 6.0F, game.textureShader);

			glfwSwapBuffers(game.window);

			if (glfwGetKey(game.window, GLFW_KEY_R)) resetGame();

			glfwPollEvents();
		}
	}
	glfwDestroyWindow(game.window);
	glfwTerminate();
}