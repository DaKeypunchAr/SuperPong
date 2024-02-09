#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "STB/image.h"

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

struct CharReprInfo
{
	char ch;
	glm::ivec2 dimension;
	glm::vec2 bearing, advance;
	glm::vec2 uvBL, uvTR;

	CharReprInfo()
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
		uvBL = glm::vec2(xInAtlas / (float) atlasDimension.x, 0.0F);
		uvTR = uvBL + glm::vec2(dimension.x / (float) atlasDimension.x, dimension.y / (float) atlasDimension.y);
	}
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

	auto readFile = [&](std::string file)
	{
		std::ifstream reader(file);
		std::string result;
		for (std::string line; std::getline(reader, line); result += '\n') result += line;
		return result;
	};
	auto compileShader = [&](unsigned int shaderId)
	{
		glCompileShader(shaderId);

		int compileStatus{};
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

		if (!compileStatus)
		{
			std::vector<char> message;
			int infoLogLength{};
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
			glGetShaderInfoLog(shaderId, infoLogLength, nullptr, message.data());

			std::string shaderType;
			int shdrType{};
			glGetShaderiv(shaderId, GL_SHADER_TYPE, &shdrType);
			shaderType = ((shdrType == GL_VERTEX_SHADER) ? "Vertex" : ((shdrType == GL_FRAGMENT_SHADER) ? "Fragment" : "Unknown"));

			std::cerr << "Unable to compile " << shaderType << " shader.\nMessage: " << message.data() << '\n';
			__debugbreak();
		}
	};
	auto linkProgram = [&](unsigned int program)
	{
		glLinkProgram(program);

		int linkStatus{};
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (!linkStatus)
		{
			std::vector<char> message;
			int infoLogLength{};
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
			glGetProgramInfoLog(program, infoLogLength, nullptr, message.data());

			std::cerr << "Unable to link program!\nMessage: " << message.data() << '\n';
			__debugbreak();
		}
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

#pragma region Initializing MainMenu

#pragma region Text Atlas Creation
	unsigned int textAtlasTexture{}, textAtlasVao{}, textAtlasEbo{}, textAtlasVbo{}, textAtlasProgram{}, uniTextAtlasTexture{};
	glm::ivec2 textAtlasDimension{};

	stbi_set_flip_vertically_on_load(true);
	unsigned char* textAtlas = stbi_load("textures/compressed_textatlas.png", &textAtlasDimension.x , &textAtlasDimension.y, nullptr, 4);

	glCreateTextures(GL_TEXTURE_2D, 1, &textAtlasTexture);
	glTextureStorage2D(textAtlasTexture, 4, GL_RGBA8, textAtlasDimension.x, textAtlasDimension.y);
	glTextureSubImage2D(textAtlasTexture, 0, 0, 0, textAtlasDimension.x, textAtlasDimension.y, GL_RGBA, GL_UNSIGNED_BYTE, textAtlas);

	glTextureParameteri(textAtlasTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(textAtlasTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(textAtlasTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textAtlasTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::array<CharReprInfo, 95> charInfos;

	std::string atlasInfo = readFile("AtlasDetails.txt");

	for (unsigned int i = 0, chIdx = 0; i < 95; i++)
	{
		unsigned int prevIdx = chIdx;
		for (; atlasInfo.at(chIdx) != '\n';) chIdx++;
		charInfos[i].initialize(atlasInfo.substr(prevIdx, chIdx++ - prevIdx));
	}

	unsigned int x = 0;
	for (unsigned int i = 0; i < 95; i++)
	{
		charInfos[i].initializeUV(textAtlasDimension, x);
		x += charInfos[i].dimension.x;
	}

	unsigned int vboCount = 16, eboCount = 6;

	glCreateVertexArrays(1, &textAtlasVao);

	auto initEbo = [&]()
		{
			glCreateBuffers(1, &textAtlasEbo);

			unsigned int* eb = (unsigned int*)_alloca(eboCount * sizeof(unsigned int));
			for (unsigned int i = 0; i < eboCount / 6u; i++)
			{
				unsigned int i6 = i * 6, i4 = i * 4;
				eb[i6] = i4;
				eb[i6 + 1] = i4 + 1;
				eb[i6 + 2] = i4 + 2;
				eb[i6 + 3] = i4 + 1;
				eb[i6 + 4] = i4 + 2;
				eb[i6 + 5] = i4 + 3;
			}

			glNamedBufferData(textAtlasEbo, eboCount * sizeof(unsigned int), eb, GL_STATIC_DRAW);

			glVertexArrayElementBuffer(textAtlasVao, textAtlasEbo);
		};
	initEbo();

	auto initVbo = [&]()
		{
			glCreateBuffers(1, &textAtlasVbo);

			float* vb = (float*)_alloca(sizeof(float) * vboCount);
			glNamedBufferData(textAtlasVbo, sizeof(float) * vboCount, vb, GL_DYNAMIC_DRAW);

			glVertexArrayVertexBuffer(textAtlasVao, 0, textAtlasVbo, 0, sizeof(float) * 4);
			glVertexArrayAttribFormat(textAtlasVao, 0, 2, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribFormat(textAtlasVao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
			glVertexArrayAttribBinding(textAtlasVao, 0, 0);
			glVertexArrayAttribBinding(textAtlasVao, 1, 0);
			glEnableVertexArrayAttrib(textAtlasVao, 0);
			glEnableVertexArrayAttrib(textAtlasVao, 1);
		};
	initVbo();

	textAtlasProgram = glCreateProgram();
	unsigned int textAtlasVS = glCreateShader(GL_VERTEX_SHADER);
	unsigned int textAtlasFS = glCreateShader(GL_FRAGMENT_SHADER);

	std::string textVSSource = readFile("shaders/text/vertex.glsl");
	std::string textFSSource = readFile("shaders/text/fragment.glsl");

	const char *textVSCStr = textVSSource.c_str(), *textFSCStr = textFSSource.c_str();

	glShaderSource(textAtlasVS, 1, &textVSCStr, nullptr);
	glShaderSource(textAtlasFS, 1, &textFSCStr, nullptr);

	compileShader(textAtlasVS);
	compileShader(textAtlasFS);

	glAttachShader(textAtlasProgram, textAtlasVS);
	glAttachShader(textAtlasProgram, textAtlasFS);

	linkProgram(textAtlasProgram);

	glDetachShader(textAtlasProgram, textAtlasVS);
	glDetachShader(textAtlasProgram, textAtlasFS);

	glDeleteShader(textAtlasVS);
	glDeleteShader(textAtlasFS);

	uniTextAtlasTexture = glGetUniformLocation(textAtlasProgram, "tex");

	auto renderText = [&](const std::string& text, glm::vec2 pos, float scale)
		{
			if (text.length() > eboCount / 6u)
			{
				glDeleteBuffers(1, &textAtlasEbo);
				glDeleteBuffers(1, &textAtlasVbo);
				vboCount = text.length() * 16u;
				eboCount = text.length() * 6u;
				initEbo();
				initVbo();
			}

			glBindTextureUnit(2, textAtlasTexture);
			glBindVertexArray(textAtlasVao);
			glUseProgram(textAtlasProgram);
			glUniform1i(uniTextAtlasTexture, 2);

			float* vb = (float*)_alloca(sizeof(float) * text.length() * 16);

			for (unsigned int i = 0; i < text.length(); i++)
			{
				CharReprInfo info = charInfos[text.at(i) - ' '];

				glm::vec2 tl = glm::vec2(pos.x + info.bearing.x * scale, pos.y + info.bearing.y * scale);
				glm::vec2 br = tl + glm::vec2(info.dimension.x, info.dimension.y) * scale;

				glm::vec2 crntMin(0, 0), crntMax(windowDimension.x, windowDimension.y), trgtMin(-1, -1), trgtMax(1, 1);
				glm::vec2 slope = (trgtMax - trgtMin) / (crntMax - crntMin);
				glm::vec2 mappedTL = (tl - crntMin) * slope + trgtMin;
				glm::vec2 mappedBR = (br - crntMin) * slope + trgtMin;

				float b[16]{
					mappedTL.x, mappedTL.y, info.uvBL.x, info.uvBL.y,
					mappedBR.x, mappedTL.y, info.uvTR.x, info.uvBL.y,
					mappedTL.x, mappedBR.y, info.uvBL.x, info.uvTR.y,
					mappedBR.x, mappedBR.y, info.uvTR.x, info.uvTR.y
				};

				memcpy((vb + i * 16), b, sizeof(float) * 16);

				pos.x += info.advance.x * scale;
				pos.y += info.advance.y * scale;
			}

			glNamedBufferSubData(textAtlasVbo, 0, sizeof(float) * text.length() * 16, vb);
			glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_INT, nullptr);
		};
	auto getStringWidth = [&](const std::string& text, float scale)
		{
			float width = 0.0F;
			for (unsigned int i = 0; i < text.length(); i++)
			{
				width += charInfos[text.at(i) - ' '].advance.x;
			}
			return width * scale;
		};
#pragma endregion

#pragma region Creating Logo
	unsigned int logoVao{}, logoVbo{}, logoEbo{}, logoProgram{}, logoTexture{}, uniLogoTexture{};
	glm::ivec2 logoDimension{};

	unsigned char* logoImg = stbi_load("textures/gameLogo.png", &logoDimension.x, &logoDimension.y, nullptr, 4);
	glCreateTextures(GL_TEXTURE_2D, 1, &logoTexture);
	glTextureStorage2D(logoTexture, 1, GL_RGBA8, logoDimension.x, logoDimension.y);
	glTextureSubImage2D(logoTexture, 0, 0, 0, logoDimension.x, logoDimension.y, GL_RGBA, GL_UNSIGNED_BYTE, logoImg);

	glTextureParameteri(logoTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(logoTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(logoTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(logoTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(logoImg);

	glCreateVertexArrays(1, &logoVao);

	unsigned int logoEB[6]{
		0, 1, 2,  1, 2, 3
	};

	glCreateBuffers(1, &logoEbo);
	glNamedBufferData(logoEbo, sizeof(unsigned int) * 6, logoEB, GL_STATIC_DRAW);
	glVertexArrayElementBuffer(logoVao, logoEbo);

	int logoScale = 8;

	glm::vec2 logoTL = (glm::ivec2(windowDimension.x, windowDimension.y * 1.5F) - logoDimension * logoScale) / 2;
	glm::vec2 logoBR = (glm::ivec2(windowDimension.x, windowDimension.y * 1.5F) + logoDimension * logoScale) / 2;

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

	logoProgram = glCreateProgram();
	unsigned int logoVS = glCreateShader(GL_VERTEX_SHADER);
	unsigned int logoFS = glCreateShader(GL_FRAGMENT_SHADER);

	std::string logoVSSource = readFile("shaders/texture/vertex.glsl");
	std::string logoFSSource = readFile("shaders/texture/fragment.glsl");

	const char *logoVSCStr = logoVSSource.c_str(), *logoFSCStr = logoFSSource.c_str();

	glShaderSource(logoVS, 1, &logoVSCStr, nullptr);
	glShaderSource(logoFS, 1, &logoFSCStr, nullptr);

	compileShader(logoVS);
	compileShader(logoFS);

	glAttachShader(logoProgram, logoVS);
	glAttachShader(logoProgram, logoFS);

	linkProgram(logoProgram);

	glDetachShader(logoProgram, logoVS);
	glDetachShader(logoProgram, logoFS);

	glDeleteShader(logoVS);
	glDeleteShader(logoFS);

	uniLogoTexture = glGetUniformLocation(logoProgram, "tex");

#pragma endregion

#pragma region Creating Menu

	unsigned int menuVao{}, menuVbo{}, menuEbo{}, menuProgram{}, menuTexture{}, uniMenuTexture{};
	glm::ivec2 menuDimension{};

	unsigned char* menuImg = stbi_load("textures/menu-list.png", &menuDimension.x, &menuDimension.y, nullptr, 4);
	glCreateTextures(GL_TEXTURE_2D, 1, &menuTexture);
	glTextureStorage2D(menuTexture, 1, GL_RGBA8, menuDimension.x, menuDimension.y);
	glTextureSubImage2D(menuTexture, 0, 0, 0, menuDimension.x, menuDimension.y, GL_RGBA, GL_UNSIGNED_BYTE, menuImg);

	glTextureParameteri(menuTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(menuTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(menuTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(menuTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(menuImg);

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

	glm::vec2 menu1TLPos = (sixthSec - glm::ivec2(menuDimension.x / 2, menuDimension.y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu1BRPos = (sixthSec + glm::ivec2(menuDimension.x / 2, menuDimension.y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y);
	glm::vec2 menu2TLPos = (sixthSec - glm::ivec2(menuDimension.x / 2, menuDimension.y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);
	glm::vec2 menu2BRPos = (sixthSec + glm::ivec2(menuDimension.x / 2, menuDimension.y) * menuScale) / 2 + glm::ivec2(0, sixthSec.y * 2.0F);

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

	menuProgram = glCreateProgram();
	unsigned int menuVS = glCreateShader(GL_VERTEX_SHADER);
	unsigned int menuFS = glCreateShader(GL_FRAGMENT_SHADER);

	std::string menuVSSource = readFile("shaders/texture/vertex.glsl");
	std::string menuFSSource = readFile("shaders/texture/fragment.glsl");

	const char* menuVSCStr = menuVSSource.c_str(), *menuFSCStr = menuFSSource.c_str();

	glShaderSource(menuVS, 1, &menuVSCStr, nullptr);
	glShaderSource(menuFS, 1, &menuFSCStr, nullptr);

	compileShader(menuVS);
	compileShader(menuFS);

	glAttachShader(menuProgram, menuVS);
	glAttachShader(menuProgram, menuFS);

	linkProgram(menuProgram);

	glDetachShader(menuProgram, menuVS);
	glDetachShader(menuProgram, menuFS);

	glDeleteShader(menuVS);
	glDeleteShader(menuFS);

	uniMenuTexture = glGetUniformLocation(menuProgram, "tex");

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

		glUseProgram(menuProgram);
		glBindVertexArray(menuVao);
		glBindTextureUnit(0, menuTexture);
		glUniform1i(uniMenuTexture, 0);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		glm::vec2 topTextPos = (menu2TLPos + menu2BRPos) / 2.0F - glm::vec2(getStringWidth("PLAY", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);
		glm::vec2 bottomTextPos = (menu1TLPos + menu1BRPos) / 2.0F - glm::vec2(getStringWidth("EXIT", menuScale / 3.5F) / 2.0F, menuScale * 2.0F);

		renderText("PLAY", topTextPos, menuScale / 3.5F);
		renderText("EXIT", bottomTextPos, menuScale / 3.5F);
	};

#pragma endregion

#pragma endregion

#pragma region Initializing Game Entities

#pragma region Paddle Creation
	constexpr glm::vec2 paddleDimension(20, -100);
	glm::vec2 leftPaddlePos(windowDimension.x * 3 / 160, (windowDimension.y - paddleDimension.y) / 2);
	glm::vec2 rightPaddlePos(windowDimension.x * 157 / 160 - paddleDimension.x, (windowDimension.y - paddleDimension.y) / 2);
	float paddleSpeed = 0.8F;

	unsigned int paddleVao, paddleEbo, paddleVbo, paddleProgram, paddleTexture, uniPaddleTexture;

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

	paddleProgram = glCreateProgram();
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vsSource = readFile("shaders/paddle/vertex.glsl");
	std::string fsSource = readFile("shaders/paddle/fragment.glsl");

	const char* vsCStr = vsSource.c_str(), * fsCStr = fsSource.c_str();

	glShaderSource(vs, 1, &vsCStr, nullptr);
	glShaderSource(fs, 1, &fsCStr, nullptr);

	compileShader(vs);
	compileShader(fs);

	glAttachShader(paddleProgram, vs);
	glAttachShader(paddleProgram, fs);

	linkProgram(paddleProgram);

	glDetachShader(paddleProgram, vs);
	glDetachShader(paddleProgram, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	uniPaddleTexture = glGetUniformLocation(paddleProgram, "tex");

	int width{}, height{}, channels{};

	unsigned char* paddleImg = stbi_load("textures/paddle.png", &width, &height, &channels, 4);

	glCreateTextures(GL_TEXTURE_2D, 1, &paddleTexture);
	glTextureStorage2D(paddleTexture, 3, GL_RGBA8, width, height);
	glTextureSubImage2D(paddleTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, paddleImg);

	glTextureParameteri(paddleTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(paddleTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(paddleTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(paddleTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateTextureMipmap(paddleTexture);

	stbi_image_free(paddleImg);

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
	unsigned int ballVao, ballVbo, ballEbo, ballProgram, ballTexture, uniBallTexture;

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

	ballProgram = glCreateProgram();
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	vsSource = readFile("shaders/ball/vertex.glsl");
	fsSource = readFile("shaders/ball/fragment.glsl");

	vsCStr = vsSource.c_str(); fsCStr = fsSource.c_str();

	glShaderSource(vs, 1, &vsCStr, nullptr);
	glShaderSource(fs, 1, &fsCStr, nullptr);

	compileShader(vs);
	compileShader(fs);

	glAttachShader(ballProgram, vs);
	glAttachShader(ballProgram, fs);

	linkProgram(ballProgram);

	glDetachShader(ballProgram, vs);
	glDetachShader(ballProgram, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	uniBallTexture = glGetUniformLocation(ballProgram, "tex");

	unsigned char* ballImg = stbi_load("textures/ball.png", &width, &height, &channels, 4);

	glCreateTextures(GL_TEXTURE_2D, 1, &ballTexture);
	glTextureStorage2D(ballTexture, 3, GL_RGBA8, width, height);
	glTextureSubImage2D(ballTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ballImg);

	glTextureParameteri(ballTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(ballTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(ballTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ballTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateTextureMipmap(ballTexture);

	stbi_image_free(ballImg);

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
	unsigned int separationVao, separationVbo, separationEbo, separationProgram, uniSeparationLineColor;

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

	separationProgram = glCreateProgram();
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	vsSource = readFile("shaders/separation/vertex.glsl");
	fsSource = readFile("shaders/separation/fragment.glsl");

	vsCStr = vsSource.c_str();
	fsCStr = fsSource.c_str();

	glShaderSource(vs, 1, &vsCStr, nullptr);
	glShaderSource(fs, 1, &fsCStr, nullptr);

	compileShader(vs);
	compileShader(fs);

	glAttachShader(separationProgram, vs);
	glAttachShader(separationProgram, fs);

	linkProgram(separationProgram);

	glDetachShader(separationProgram, vs);
	glDetachShader(separationProgram, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	uniSeparationLineColor = glGetUniformLocation(separationProgram, "color");

#pragma endregion

#pragma endregion

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

		renderText(leftScoreStr, leftScorePos, scoreFontSize);
		renderText(rightScoreStr, rightScorePos, scoreFontSize);
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

	glBindTextureUnit(0, paddleTexture);
	glBindTextureUnit(1, ballTexture);
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

			glUseProgram(separationProgram);
			glBindVertexArray(separationVao);
			glUniform4f(uniSeparationLineColor, separationLineColor.r, separationLineColor.g, separationLineColor.b, separationLineColor.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			glUseProgram(paddleProgram);
			glBindTextureUnit(0, paddleTexture);
			glUniform1i(uniPaddleTexture, 0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			glUseProgram(ballProgram);
			glBindTextureUnit(1, ballTexture);
			glUniform1i(uniBallTexture, 1);
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

			glUseProgram(logoProgram);
			glBindVertexArray(logoVao);
			glBindTextureUnit(0, logoTexture);
			glUniform1i(uniLogoTexture, 0);
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

			glUseProgram(separationProgram);
			glBindVertexArray(separationVao);
			glUniform4f(uniSeparationLineColor, separationLineColor.r, separationLineColor.g, separationLineColor.b, separationLineColor.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			glUseProgram(paddleProgram);
			glBindTextureUnit(0, paddleTexture);
			glUniform1i(uniPaddleTexture, 0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			glUseProgram(ballProgram);
			glBindTextureUnit(1, ballTexture);
			glUniform1i(uniBallTexture, 1);
			glBindVertexArray(ballVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderText("PAUSED!", glm::vec2((windowDimension.x - getStringWidth("PAUSED!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F);

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
		else if (gameState == GS::WIN)
		{
			updatePaddleVbo();
			updateBallVbo();

			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(separationProgram);
			glBindVertexArray(separationVao);
			glUniform4f(uniSeparationLineColor, separationLineColor.r, separationLineColor.g, separationLineColor.b, separationLineColor.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			renderScore();

			glUseProgram(paddleProgram);
			glBindTextureUnit(0, paddleTexture);
			glUniform1i(uniPaddleTexture, 0);
			glBindVertexArray(paddleVao);
			glDrawElements(GL_TRIANGLES, 6 * 2, GL_UNSIGNED_INT, nullptr);

			glUseProgram(ballProgram);
			glBindTextureUnit(1, ballTexture);
			glUniform1i(uniBallTexture, 1);
			glBindVertexArray(ballVao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			if (leftWin)
				renderText("Left Won!", glm::vec2((windowDimension.x - getStringWidth("Left Won!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F);
			else
				renderText("Right Won!", glm::vec2((windowDimension.x - getStringWidth("Right Won!", 6.0F)) / 2.0F, (windowDimension.y - 6.0F) / 2.0F), 6.0F);

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
	}

#pragma region Releasing some memory
	glDeleteTextures(1, &paddleTexture);
	glDeleteVertexArrays(1, &paddleVao);
	glDeleteBuffers(1, &paddleVbo);
	glDeleteBuffers(1, &paddleEbo);
	glDeleteProgram(paddleProgram);

	glDeleteTextures(1, &ballTexture);
	glDeleteVertexArrays(1, &ballVao);
	glDeleteBuffers(1, &ballVbo);
	glDeleteBuffers(1, &ballEbo);
	glDeleteProgram(ballProgram);

	glDeleteTextures(1, &textAtlasTexture);
	glDeleteVertexArrays(1, &textAtlasVao);
	glDeleteBuffers(1, &textAtlasVbo);
	glDeleteBuffers(1, &textAtlasEbo);
	glDeleteProgram(textAtlasProgram);

	glDeleteVertexArrays(1, &separationVao);
	glDeleteBuffers(1, &separationVbo);
	glDeleteBuffers(1, &separationEbo);
	glDeleteProgram(separationProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
#pragma endregion
}