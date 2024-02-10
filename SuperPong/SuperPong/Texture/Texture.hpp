#ifndef _SUPERpong_Texture
#define _SUPERpong_Texture

#include "GLEW/glew.h"
#include "glm/glm.hpp"
#include "../STB/image.h"

#include <iostream>
#include <string>

namespace OGL
{
	struct TextureFilter
	{
		unsigned int minFilter;
		unsigned int magFilter;
		unsigned int wrapS;
		unsigned int wrapT;
		glm::vec4 color;
		TextureFilter(unsigned int minFilter, unsigned int magFilter, unsigned int wrapS, unsigned int wrapT, glm::vec4 color = glm::vec4(1.0F))
			: minFilter(minFilter), magFilter(magFilter), wrapS(wrapS), wrapT(wrapT), color(color) {}
		TextureFilter(unsigned int minMagFilter, unsigned int wrapST, glm::vec4 color = glm::vec4(1.0F))
			: minFilter(minMagFilter), magFilter(minMagFilter), wrapS(wrapST), wrapT(wrapST), color(color) {}
	};

	class Texture2D
	{
		public:
			Texture2D() = default;
			Texture2D(const Texture2D& other) = delete;
			Texture2D(const std::string& imagePath, unsigned int numOfChannels, TextureFilter filters, unsigned int levels = 1);
			Texture2D(unsigned char* img, glm::ivec2 dimensions, unsigned int numOfChannels, TextureFilter filters, unsigned int levels = 1);
			Texture2D(glm::ivec2 dimension, unsigned int numOfChannels, TextureFilter filters, unsigned int levels = 1);
			~Texture2D();

			void bind(unsigned int slot) const;
			void changeImage(unsigned char* img, glm::ivec2 offset, glm::ivec2 dimension) const;

			glm::ivec2 getTextureDimension() const { return m_TextureDimension; }

		private:
			unsigned int m_TextureId;
			glm::ivec2 m_TextureDimension;
			unsigned int m_TextureFormat;
	};
}

#endif