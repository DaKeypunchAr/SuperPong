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

	TextAtlas::TextAtlas(std::string atlasDetails, const SPong::GameData& game, AtlasDetailFormat format)
		: m_VAO(game.textureVBOsConfigs, {8, 8}, 6), m_Texture(atlasDetails.substr(0, atlasDetails.find_first_of('\n')), 4, game.textureFilters), m_Chars()
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

		std::vector<unsigned int> eb
		{
			0, 1, 2, 1, 2, 3
		};
		m_VAO.updateEB(eb, 0);
	}
	void TextAtlas::reinitBuffers()
	{
		m_VAO.reCreateEB(m_CharacterCapacity * 6);
		m_VAO.reCreateVB(m_CharacterCapacity * 8, 0);
		m_VAO.reCreateVB(m_CharacterCapacity * 8, 1);

		std::vector<unsigned int> eb;
		eb.reserve(m_CharacterCapacity * 6);

		for (unsigned int i = 0; i < m_CharacterCapacity; i++)
		{
			unsigned int i4 = i * 4;
			eb.push_back(i4);
			eb.push_back(i4 + 1);
			eb.push_back(i4 + 2);
			eb.push_back(i4 + 1);
			eb.push_back(i4 + 2);
			eb.push_back(i4 + 3);
		}

		m_VAO.updateEB(eb, 0);
	}

	void TextAtlas::renderText(const std::string& text, glm::vec2 pos, float scale, const OGL::Shader& shader)
	{
		if (text.length() > m_CharacterCapacity)
		{
			m_CharacterCapacity = text.length();
			reinitBuffers();
		}

		m_Texture.bind(2);
		m_VAO.bind();
		shader.use();
		shader.uni1i("tex", 2);

		std::vector<float> posVB, texVB;
		posVB.reserve(sizeof(float) * text.length() * 8);
		texVB.reserve(sizeof(float) * text.length() * 8);

		for (unsigned int i = 0; i < text.length(); i++)
		{
			CharRepresentationInfo info = m_Chars[text.at(i) - ' '];

			glm::vec2 tl = glm::vec2(pos.x + info.bearing.x * scale, pos.y + info.bearing.y * scale);
			glm::vec2 br = tl + glm::vec2(info.dimension.x, info.dimension.y) * scale;

			glm::vec2 crntMin(0, 0), crntMax(1600, 900), trgtMin(-1, -1), trgtMax(1, 1);
			glm::vec2 slope = (trgtMax - trgtMin) / (crntMax - crntMin);
			glm::vec2 mappedTL = (tl - crntMin) * slope + trgtMin;
			glm::vec2 mappedBR = (br - crntMin) * slope + trgtMin;

			std::vector<float> p{
				mappedTL.x, mappedTL.y,
				mappedBR.x, mappedTL.y,
				mappedTL.x, mappedBR.y,
				mappedBR.x, mappedBR.y,
			};
			std::vector<float> t{
				info.uvBL.x, info.uvBL.y,
				info.uvTR.x, info.uvBL.y,
				info.uvBL.x, info.uvTR.y,
				info.uvTR.x, info.uvTR.y
			};

			posVB.insert(posVB.end(), p.begin(), p.end());
			texVB.insert(texVB.end(), t.begin(), t.end());

			pos.x += info.advance.x * scale;
			pos.y += info.advance.y * scale;
		}

		m_VAO.updateVB(posVB, 0, 0);
		m_VAO.updateVB(texVB, 1, 0);
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