#include "VAO.hpp"

namespace OGL
{
	VAO::VAO(std::vector<VBOConfig> vboConfigs, std::vector<unsigned int> vboCounts, unsigned int ebCount)
		: m_VBOConfigs(vboConfigs), m_VAO(), m_VBOs(), m_EBO()
	{
		initialize(vboCounts, ebCount);
	}
	VAO::~VAO()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_EBO);

		for (unsigned int vbo : m_VBOs)
		{
			glDeleteBuffers(1, &vbo);
		}
	}

	void VAO::initialize(std::vector<unsigned int> vboCounts, unsigned int eboCount)
	{
		glCreateVertexArrays(1, &m_VAO);
		glCreateBuffers(1, &m_EBO);
		glNamedBufferData(m_EBO, eboCount * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glVertexArrayElementBuffer(m_VAO, m_EBO);

		for (unsigned int idx = 0; idx < vboCounts.size(); idx++)
		{
			unsigned int vbo;
			glCreateBuffers(1, &vbo);
			glNamedBufferData(vbo, vboCounts[idx] * sizeof(float), nullptr, m_VBOConfigs[idx].usage);

			glVertexArrayVertexBuffer(m_VAO, m_VBOConfigs[idx].bindingIdx, vbo, 0, m_VBOConfigs[idx].stride);

			for (const AttribInfo& info : m_VBOConfigs[idx].attributes)
			{
				glVertexArrayAttribFormat(m_VAO, info.attribIdx, info.count, GL_FLOAT, GL_FALSE, info.offset);
				glVertexArrayAttribBinding(m_VAO, info.attribIdx, m_VBOConfigs[idx].bindingIdx);
				glEnableVertexArrayAttrib(m_VAO, info.attribIdx);
			}
			m_VBOs.push_back(vbo);
		}
	}

	void VAO::bind() const
	{
		glBindVertexArray(m_VAO);
	}

	void VAO::updateVB(std::vector<float> vb, unsigned int bindingIdx, unsigned int offset)
	{
		glNamedBufferSubData(m_VBOs[bindingIdx], offset, vb.size() * sizeof(float), vb.data());
	}
	void VAO::updateEB(std::vector<unsigned int> eb, unsigned int offset)
	{
		glNamedBufferSubData(m_EBO, offset, eb.size() * sizeof(unsigned int), eb.data());
	}

	void VAO::reCreateVB(unsigned int vbCount, unsigned int bindingIdx)
	{
		glNamedBufferData(m_VBOs[bindingIdx], sizeof(float) * vbCount, nullptr, GL_DYNAMIC_DRAW);
	}
	void VAO::reCreateEB(unsigned int ebCount)
	{
		glNamedBufferData(m_EBO, ebCount * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
	}
}