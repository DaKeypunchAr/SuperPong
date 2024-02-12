#ifndef _SUPERpong_Shader
#define _SUPERpong_Shader

#include "GLEW/glew.h"
#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace OGL
{
	class Shader
	{
		public:
			Shader(const std::string& folderPath);
			Shader(const std::string& vsFilePath, const std::string& fsFilePath);
			Shader();
			Shader(const Shader& shader) = delete;
			~Shader();

			void use() const;

			unsigned int getUniformLocation(const std::string& name) const;

			void uni1i(const std::string&, int i) const;
			void uni1f(const std::string&, float f) const;
			void uni2f(const std::string&, glm::vec2 f2) const;
			void uni3f(const std::string&, glm::vec3 f3) const;
			void uni4f(const std::string&, glm::vec4 f4) const;

			void operator=(const Shader & shader) = delete;

			void initialize(const std::string& folderPath);
			void initialize(const std::string& vsFilePath, const std::string& fsFilePath);

		private:
			std::string readFile(const std::string& fileLoc) const;
			bool compileShader(unsigned int shaderId) const;
			bool linkProgram() const;

		private:
			unsigned int m_ProgramId;
	};
}

#endif