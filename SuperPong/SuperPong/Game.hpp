#ifndef _SUPERpong_Game
#define _SUPERpong_Game

#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "TextAtlas/TextAtlas.hpp"
#include "VAO/VAO.hpp"
#include "GLFW/glfw3.h"

namespace SPong
{
	enum GS
	{
		MAIN_MENU,
		PLAY,
		PAUSE,
		WIN
	};

	struct GameData
	{
		GLFWwindow* window;
		const glm::ivec2 windowDimension = glm::ivec2(1600, 900);
		GS gameState = GS::MAIN_MENU;
		OGL::TextureFilter textureFilters = OGL::TextureFilter(GL_NEAREST, GL_REPEAT);
		std::vector<OGL::VBOConfig> logoVBOsConfigs
		{
			OGL::VBOConfig(0, GL_STATIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(0, 2, 0)}),
			OGL::VBOConfig(1, GL_STATIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(1, 2, 0)}),
		};
		std::vector<OGL::VBOConfig> menuVBOsConfigs
		{
			OGL::VBOConfig(0, GL_STATIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(0, 2, 0)}),
			OGL::VBOConfig(1, GL_DYNAMIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(1, 2, 0)}),
		};
		std::vector<OGL::VBOConfig> textureVBOsConfigs
		{
			OGL::VBOConfig(0, GL_DYNAMIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(0, 2, 0)}),
			OGL::VBOConfig(1, GL_STATIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(1, 2, 0)}),
		};
		std::vector<OGL::VBOConfig> solidVBOsConfigs
		{
			OGL::VBOConfig(0, GL_STATIC_DRAW, sizeof(float) * 2, {OGL::AttribInfo(0, 2, 0)})
		};

		OGL::Shader textureShader;
		OGL::Shader solidShader;

		// Data for WIN, PLAY and PAUSE states;
		unsigned int leftPaddleScore = 0;
		unsigned int rightPaddleScore = 0;
		bool leftHasWon = false;

		// Data for MAIN MENU state;
		int selectedMenuItem = 0;
	};
}

typedef SPong::GS GS;

#endif