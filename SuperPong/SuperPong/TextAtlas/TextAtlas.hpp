#ifndef _SUPERpong_TextAtlas
#define _SUPERpong_TextAtlas

#include "../Texture/Texture.hpp"
#include "../Shader/Shader.hpp"
#include "../VAO/VAO.hpp"
#include "../Game.hpp"

#include <array>

namespace SPong
{
	enum AtlasDetailFormat
	{
		DIMENSION_BEARING_ADVANCE,
		DIMENSION_ADVANCE_BEARING,
		BEARING_DIMENSION_ADVANCE,
		ADVANCE_DIMENSION_BEARING,
		ADVANCE_BEARING_DIMENSION,
		BEARING_ADVANCE_DIMENSION,
	};

	struct CharRepresentationInfo
	{
		char ch;
		glm::ivec2 dimension;
		glm::vec2 bearing, advance;
		glm::vec2 uvBL, uvTR;

		CharRepresentationInfo()
			: ch(0), dimension(), uvBL(), uvTR(), bearing(), advance() {}

		void initialize(const std::string& tag, AtlasDetailFormat format);
		void initializeUV(glm::ivec2 atlasDimension, unsigned int xInAtlas);
	};

	class TextAtlas
	{
		public:
			TextAtlas(std::string atlasDetails, const SPong::GameData& game, AtlasDetailFormat format = AtlasDetailFormat::DIMENSION_BEARING_ADVANCE);
			~TextAtlas() = default;

			void renderText(const std::string& text, glm::vec2 position, float scale, const OGL::Shader& shader);
			float getTextWidth(const std::string& text, float scale);

		private:
			void reinitBuffers();

		private:
			OGL::VAO m_VAO;
			OGL::Texture2D m_Texture;
			std::array<CharRepresentationInfo, 95> m_Chars;
			unsigned int m_CharacterCapacity = 1;
	};
}

#endif