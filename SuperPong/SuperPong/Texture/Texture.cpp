#include "Texture.hpp"

namespace OGL
{
	Texture2D::Texture2D(const std::string& imagePath, unsigned int numOfChannels, TextureFilter filter, unsigned int levels)
	{
		stbi_set_flip_vertically_on_load(true);
		unsigned char* img = stbi_load(imagePath.c_str(), &m_TextureDimension.x, &m_TextureDimension.y, nullptr, numOfChannels);

		if (numOfChannels > 4 || numOfChannels == 0)
		{
			std::cerr << "Invalid num of channels!\n";
			__debugbreak();
		}

		unsigned int intFormat = (numOfChannels == 1) ? GL_R8 : (numOfChannels == 2) ? GL_RG8 : (numOfChannels == 3) ? GL_RGB8 : GL_RGBA8;
		m_TextureFormat = (intFormat == GL_R8) ? GL_R : (intFormat == GL_RG8) ? GL_RG : (intFormat == GL_RGB8) ? GL_RGB : GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, numOfChannels);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
		glTextureStorage2D(m_TextureId, levels, intFormat, m_TextureDimension.x, m_TextureDimension.y);
		changeImage(img, glm::ivec2(0), m_TextureDimension);

		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, filter.minFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, filter.magFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, filter.wrapS);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, filter.wrapT);

		stbi_image_free(img);
	}
	Texture2D::Texture2D(unsigned char* img, glm::ivec2 dimension, unsigned int numOfChannels, TextureFilter filter, unsigned int levels)
		: m_TextureDimension(dimension)
	{
		if (numOfChannels > 4 || numOfChannels == 0)
		{
			std::cerr << "Invalid num of channels!\n";
			__debugbreak();
		}

		unsigned int intFormat = (numOfChannels == 1) ? GL_R8 : (numOfChannels == 2) ? GL_RG8 : (numOfChannels == 3) ? GL_RGB8 : GL_RGBA8;
		m_TextureFormat = (intFormat == GL_R8) ? GL_R : (intFormat == GL_RG8) ? GL_RG : (intFormat == GL_RGB8) ? GL_RGB : GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, numOfChannels);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
		glTextureStorage2D(m_TextureId, levels, intFormat, dimension.x, dimension.y);
		changeImage(img, glm::ivec2(0), m_TextureDimension);

		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, filter.minFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, filter.magFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, filter.wrapS);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, filter.wrapT);
	}
	Texture2D::Texture2D(glm::ivec2 dimension, unsigned int numOfChannels, TextureFilter filter, unsigned int levels)
		: m_TextureDimension(dimension)
	{
		if (numOfChannels > 4 || numOfChannels == 0)
		{
			std::cerr << "Invalid num of channels!\n";
			__debugbreak();
		}

		unsigned int intFormat = (numOfChannels == 1) ? GL_R8 : (numOfChannels == 2) ? GL_RG8 : (numOfChannels == 3) ? GL_RGB8 : GL_RGBA8;
		m_TextureFormat = (intFormat == GL_R8) ? GL_R : (intFormat == GL_RG8) ? GL_RG : (intFormat == GL_RGB8) ? GL_RGB : GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, numOfChannels);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
		glTextureStorage2D(m_TextureId, levels, intFormat, dimension.x, dimension.y);

		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, filter.minFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, filter.magFilter);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, filter.wrapS);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, filter.wrapT);
	}
	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_TextureId);
	}

	void Texture2D::bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_TextureId);
	}
	void Texture2D::changeImage(unsigned char* img, glm::ivec2 offset, glm::ivec2 dimension) const
	{
		glTextureSubImage2D(m_TextureId, 0, offset.x, offset.y, dimension.x, dimension.y, m_TextureFormat, GL_UNSIGNED_BYTE, img);
	}
}