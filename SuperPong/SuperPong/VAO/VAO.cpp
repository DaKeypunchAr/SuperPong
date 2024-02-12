#include "VAO.hpp"

namespace OGL
{
	VAO::VAO(std::vector<float> vb, std::vector<unsigned int> eb, std::vector<VAOAttrib> attribList)
		: m_VAO(), m_VBO(), m_EBO(), m_AttribList(attribList)
	{
		initialize(vb.size(), eb.size());
		updateVB(vb, 0);
		updateEB(eb, 0);
	}
	VAO::VAO(unsigned int vbCount, unsigned int ebCount, std::vector<VAOAttrib> attribList)
		: m_VAO(), m_VBO(), m_EBO(), m_AttribList(attribList)
	{
		initialize(vbCount, ebCount);
	}
	VAO::~VAO()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
	}

	void VAO::initialize(unsigned int vbCount, unsigned int ebCount)
	{
		glCreateVertexArrays(1, &m_VAO);
		glCreateBuffers(1, &m_VBO);
		glCreateBuffers(1, &m_EBO);

		glNamedBufferData(m_VBO, vbCount * sizeof(float), _alloca(vbCount * sizeof(float)), GL_DYNAMIC_DRAW);
		glNamedBufferData(m_EBO, ebCount * sizeof(unsigned int), _alloca(ebCount * sizeof(unsigned int)), GL_DYNAMIC_DRAW);

		glVertexArrayElementBuffer(m_VAO, m_EBO);
		glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, m_AttribList.at(0).stride);

		for (const VAOAttrib& attrib : m_AttribList)
		{
			glVertexArrayAttribFormat(m_VAO, attrib.attrib, attrib.count, attrib.type, GL_FALSE, attrib.offset);
			glVertexArrayAttribBinding(m_VAO, attrib.attrib, 0);
			glEnableVertexArrayAttrib(m_VAO, attrib.attrib);
		}
	}

	void VAO::bind() const
	{
		glBindVertexArray(m_VAO);
	}

	void VAO::updateVB(std::vector<float> vb, unsigned int offset)
	{
		glNamedBufferSubData(m_VBO, offset, vb.size() * sizeof(float), vb.data());
	}
	void VAO::updateEB(std::vector<unsigned int> eb, unsigned int offset)
	{
		glNamedBufferSubData(m_EBO, offset, eb.size() * sizeof(unsigned int), eb.data());
	}

	void VAO::reCreateVB(unsigned int vbCount)
	{
		glDeleteBuffers(1, &m_VBO);
		glCreateBuffers(1, &m_VBO);

		glNamedBufferData(m_VBO, sizeof(float) * vbCount, _alloca(sizeof(float) * vbCount), GL_DYNAMIC_DRAW);
		glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, m_AttribList.at(0).stride);
	}
	void VAO::reCreateEB(unsigned int ebCount)
	{
		glDeleteBuffers(1, &m_EBO);
		glCreateBuffers(1, &m_EBO);

		glNamedBufferData(m_EBO, ebCount * sizeof(unsigned int), _alloca(ebCount * sizeof(unsigned int)), GL_DYNAMIC_DRAW);
		glVertexArrayElementBuffer(m_VAO, m_EBO);
	}
}