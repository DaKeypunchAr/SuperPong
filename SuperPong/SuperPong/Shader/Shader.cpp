#include "Shader.hpp"

namespace OGL
{
	Shader::Shader(const std::string& folderPath)
		: m_ProgramId(glCreateProgram()) { initialize(folderPath + "/vertex.glsl", folderPath + "/fragment.glsl"); }
	Shader::Shader(const std::string& vsFilePath, const std::string& fsFilePath)
		: m_ProgramId(glCreateProgram()) { initialize(vsFilePath, fsFilePath); }
	Shader::~Shader()
	{
		glDeleteProgram(m_ProgramId);
	}

	void Shader::use() const
	{
		glUseProgram(m_ProgramId);
	}

	unsigned int Shader::getUniformLocation(const std::string& uniformName) const
	{
		unsigned int location = glGetUniformLocation(m_ProgramId, uniformName.c_str());
		if (location == -1)
		{
			std::cerr << "Uniform does not exist!\n";
			__debugbreak();
		}
		return location;
	}
	void Shader::uni1i(const std::string& uniformName, int i) const
	{
		glUniform1i(getUniformLocation(uniformName), i);
	}
	void Shader::uni1f(const std::string& uniformName, float f) const
	{
		glUniform1f(getUniformLocation(uniformName), f);
	}
	void Shader::uni2f(const std::string& uniformName, glm::vec2 f2) const
	{
		glUniform2f(getUniformLocation(uniformName), f2.x, f2.y);
	}
	void Shader::uni3f(const std::string& uniformName, glm::vec3 f3) const
	{
		glUniform3f(getUniformLocation(uniformName), f3.x, f3.y, f3.z);
	}
	void Shader::uni4f(const std::string& uniformName, glm::vec4 f4) const
	{
		glUniform4f(getUniformLocation(uniformName), f4.x, f4.y, f4.z, f4.w);
	}

	void Shader::initialize(const std::string& vsFilePath, const std::string& fsFilePath) const
	{
		unsigned int vs = glCreateShader(GL_VERTEX_SHADER), fs = glCreateShader(GL_FRAGMENT_SHADER);

		std::string vsSource = readFile(vsFilePath), fsSource = readFile(fsFilePath);
		const char* vsCStr = vsSource.c_str(), * fsCStr = fsSource.c_str();

		glShaderSource(vs, 1, &vsCStr, nullptr);
		glShaderSource(fs, 1, &fsCStr, nullptr);

		compileShader(vs);
		compileShader(fs);

		glAttachShader(m_ProgramId, vs);
		glAttachShader(m_ProgramId, fs);

		linkProgram();

		glDetachShader(m_ProgramId, vs);
		glDetachShader(m_ProgramId, fs);

		glDeleteShader(vs);
		glDeleteShader(fs);
	}
	std::string Shader::readFile(const std::string& fileLocation) const
	{
		std::ifstream fileReader(fileLocation);
		std::string fileContents;
		for (std::string line; std::getline(fileReader, line); fileContents += '\n') fileContents += line;
		return fileContents;
	}
	bool Shader::compileShader(unsigned int shaderId) const
	{
		glCompileShader(shaderId);

		int compileStatus{};
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

		if (!compileStatus)
		{
			int infoLogLength{};
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			// sizeof(char) * infoLogLength = 1 * infoLogLength = infoLogLength
			char* infoLog = (char*)_alloca(infoLogLength);
			glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLog);

			int shdrType{};
			glGetShaderiv(shaderId, GL_SHADER_TYPE, &shdrType);

			std::string shaderType = ((shdrType == GL_VERTEX_SHADER) ? "Shader Type: Vertex"
				: ((shdrType == GL_FRAGMENT_SHADER) ? "Shader Type: Fragment"
					: "Shader Type: Unknown"));

			std::cerr << "Failed to compile shader:\n\t" << shaderType << "\n\tInfo Log: " << infoLog << '\n';
			__debugbreak();
			return false;
		}
		return true;
	}
	bool Shader::linkProgram() const
	{
		glLinkProgram(m_ProgramId);

		int linkStatus{};
		glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &linkStatus);

		if (!linkStatus)
		{
			int infoLogLength{};
			glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			// sizeof(char) * infoLogLength = 1 * infoLogLength = infoLogLength
			char* infoLog = (char*) _alloca(infoLogLength);
			glGetProgramInfoLog(m_ProgramId, infoLogLength, nullptr, infoLog);

			std::cerr << "Failed to Link Program:\n\tInfo Log: " << infoLog << '\n';
			__debugbreak();
			return false;
		}
		return true;
	}
}