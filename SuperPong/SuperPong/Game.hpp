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
		std::vector<OGL::VAOAttrib> textureAttribList
		{
			OGL::VAOAttrib(0, 2, GL_FLOAT, 0, sizeof(float) * 4),
			OGL::VAOAttrib(1, 2, GL_FLOAT, sizeof(float) * 2, sizeof(float) * 4),
		};
		std::vector<OGL::VAOAttrib> solidAttribList
		{
			OGL::VAOAttrib(0, 2, GL_FLOAT, 0, sizeof(float) * 2)
		};

		OGL::Shader textureShader;
		OGL::Shader solidShader;

		// Data for WIN, PLAY and PAUSE states;
		unsigned int leftPaddleScore = 0;
		unsigned int rightPaddleScore = 0;
		bool leftHasWon = false;

		// Data for MAIN MENU state;
		unsigned int selectedMenuItem = 0;
	};
}

typedef SPong::GS GS;

#endif