#include "TextAtlas.hpp"

namespace SPong
{
	void CharRepresentationInfo::initialize(const std::string& tag, AtlasDetailFormat format)
	{
		ch = tag.at(0);

		std::array<glm::vec2, 3> vectors{};

		vectors[0] = glm::vec2(std::stof(tag.substr(2, 8)), std::stof(tag.substr(11, 8)));
		vectors[1] = glm::vec2(std::stof(tag.substr(20, 8)), std::stof(tag.substr(29, 8)));
		vectors[2] = glm::vec2(std::stof(tag.substr(38, 8)), std::stof(tag.substr(47, 8)));

		switch (format)
		{
		case AtlasDetailFormat::ADVANCE_BEARING_DIMENSION:
			advance = vectors[0];
			bearing = vectors[1];
			dimension = vectors[2];
			break;
		case AtlasDetailFormat::ADVANCE_DIMENSION_BEARING:
			advance = vectors[0];
			dimension = vectors[1];
			bearing = vectors[2];
			break;
		case AtlasDetailFormat::BEARING_ADVANCE_DIMENSION:
			bearing = vectors[0];
			advance = vectors[1];
			dimension = vectors[2];
			break;
		case AtlasDetailFormat::BEARING_DIMENSION_ADVANCE:
			bearing = vectors[0];
			dimension = vectors[1];
			advance = vectors[2];
			break;
		case AtlasDetailFormat::DIMENSION_ADVANCE_BEARING:
			dimension = vectors[0];
			advance = vectors[1];
			bearing = vectors[2];
			break;
		case AtlasDetailFormat::DIMENSION_BEARING_ADVANCE:
			dimension = vectors[0];
			bearing = vectors[1];
			advance = vectors[2];
			break;
		}
	}
	void CharRepresentationInfo::initializeUV(glm::ivec2 atlasDimension, unsigned int xInAtlas)
	{
		uvBL = glm::vec2(xInAtlas / (float)atlasDimension.x, 0.0F);
		uvTR = uvBL + glm::vec2(dimension.x / (float)atlasDimension.x, dimension.y / (float)atlasDimension.y);
	}

	TextAtlas::TextAtlas(std::string atlasDetails, OGL::TextureFilter filter, AtlasDetailFormat format)
		: m_Vao(), m_Vbo(), m_Ebo(), m_Texture(atlasDetails.substr(0, atlasDetails.find_first_of('\n')), 4, filter), m_Chars()
	{
		int xInAtlas = 0;
		for (unsigned int i = 0; i < 95; i++)
		{
			atlasDetails = atlasDetails.substr(atlasDetails.find_first_of('\n') + 1, atlasDetails.size() - atlasDetails.find_first_of('\n') + 1);
			std::string tag = atlasDetails.substr(0, atlasDetails.find_first_of('\n'));
			
			m_Chars[i].initialize(tag, format);
			m_Chars[i].initializeUV(m_Texture.getTextureDimension(), xInAtlas);
			xInAtlas += m_Chars[i].dimension.x;
		}

		glCreateVertexArrays(1, &m_Vao);

		unsigned int eb[6]
		{
			0, 1, 2, 1, 2, 3
		};
		glCreateBuffers(1, &m_Ebo);
		glNamedBufferData(m_Ebo, sizeof(unsigned int) * 6, eb, GL_STATIC_DRAW);
		glVertexArrayElementBuffer(m_Vao, m_Ebo);

		void* buff = _alloca(sizeof(float) * 16);
		glCreateBuffers(1, &m_Vbo);
		glNamedBufferData(m_Vbo, sizeof(float) * 16, buff, GL_DYNAMIC_DRAW);
		glVertexArrayVertexBuffer(m_Vao, 0, m_Vbo, 0, sizeof(float) * 4);
		glVertexArrayAttribFormat(m_Vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(m_Vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
		glVertexArrayAttribBinding(m_Vao, 0, 0);
		glVertexArrayAttribBinding(m_Vao, 1, 0);
		glEnableVertexArrayAttrib(m_Vao, 0);
		glEnableVertexArrayAttrib(m_Vao, 1);
	}
	TextAtlas::~TextAtlas()
	{
		glDeleteVertexArrays(1, &m_Vao);
		glDeleteBuffers(1, &m_Vbo);
		glDeleteBuffers(1, &m_Ebo);
	}
	void TextAtlas::reinitBuffers()
	{
		unsigned int* eb = (unsigned int*) _alloca(m_CharacterCapacity * 6 * sizeof(unsigned int));
		for (unsigned int i = 0; i < m_CharacterCapacity; i++)
		{
			unsigned int i4 = i * 4, i6 = i * 6;
			eb[i6] = i4;
			eb[i6 + 1] = i4 + 1;
			eb[i6 + 2] = i4 + 2;
			eb[i6 + 3] = i4 + 1;
			eb[i6 + 4] = i4 + 2;
			eb[i6 + 5] = i4 + 3;
		}
		glDeleteBuffers(1, &m_Ebo);
		glCreateBuffers(1, &m_Ebo);
		glNamedBufferData(m_Ebo, sizeof(unsigned int) * 6 * m_CharacterCapacity, eb, GL_STATIC_DRAW);
		glVertexArrayElementBuffer(m_Vao, m_Ebo);

		glDeleteBuffers(1, &m_Vbo);
		glCreateBuffers(1, &m_Vbo);
		glNamedBufferData(m_Vbo, sizeof(float) * 16 * m_CharacterCapacity, _alloca(sizeof(float) * 16 * m_CharacterCapacity), GL_STATIC_DRAW);
		glVertexArrayVertexBuffer(m_Vao, 0, m_Vbo, 0, sizeof(float) * 4);
		glVertexArrayAttribFormat(m_Vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(m_Vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);
		glVertexArrayAttribBinding(m_Vao, 0, 0);
		glVertexArrayAttribBinding(m_Vao, 1, 0);
		glEnableVertexArrayAttrib(m_Vao, 0);
		glEnableVertexArrayAttrib(m_Vao, 1);
	}

	void TextAtlas::renderText(const std::string& text, glm::vec2 pos, float scale, const OGL::Shader& shader)
	{
		if (text.length() > m_CharacterCapacity)
		{
			m_CharacterCapacity = text.length();
			reinitBuffers();
		}

		m_Texture.bind(2);
		glBindVertexArray(m_Vao);
		shader.use();
		shader.uni1i("tex", 2);

		float* vb = (float*)_alloca(sizeof(float) * text.length() * 16);

		for (unsigned int i = 0; i < text.length(); i++)
		{
			CharRepresentationInfo info = m_Chars[text.at(i) - ' '];

			glm::vec2 tl = glm::vec2(pos.x + info.bearing.x * scale, pos.y + info.bearing.y * scale);
			glm::vec2 br = tl + glm::vec2(info.dimension.x, info.dimension.y) * scale;

			glm::vec2 crntMin(0, 0), crntMax(1600, 900), trgtMin(-1, -1), trgtMax(1, 1);
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

		glNamedBufferSubData(m_Vbo, 0, sizeof(float) * text.length() * 16, vb);
		glDrawElements(GL_TRIANGLES, text.length() * 6, GL_UNSIGNED_INT, nullptr);
	}
	float TextAtlas::getTextWidth(const std::string& text, float scale)
	{
		float width = 0.0F;
		for (char c : text)
		{
			width += m_Chars[c - ' '].advance.x;
		}
		return width * scale;
	}
}