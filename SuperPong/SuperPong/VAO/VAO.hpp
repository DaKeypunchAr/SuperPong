#ifndef _SUPERpong_VAO
#define _SUPERpong_VAO

#include "GLEW/glew.h"

#include <vector>

namespace OGL
{
	struct VAOAttrib
	{
		unsigned int attrib;
		unsigned int count;
		unsigned int type;
		unsigned int offset;
		unsigned int stride;
		VAOAttrib(unsigned int attrib, unsigned int count, unsigned int type, unsigned int offset, unsigned int stride)
			: attrib(attrib), count(count), type(type), offset(offset), stride(stride) {}
	};

	class VAO
	{
		public:
			VAO() = delete;
			VAO(const VAO& other) = delete;
			VAO(std::vector<float> vb, std::vector<unsigned int> eb, std::vector<VAOAttrib> attribList);
			VAO(unsigned int vbCount, unsigned int ebCount, std::vector<VAOAttrib> attribList);
			~VAO();

			void operator=(const VAO& other) = delete;

		public:
			void bind() const;
			void updateVB(std::vector<float> vb, unsigned int offset);
			void updateEB(std::vector<unsigned int> eb, unsigned int offset);

			void reCreateVB(unsigned int vbCount);
			void reCreateEB(unsigned int ebCount);

		private:
			void initialize(unsigned int vbCount, unsigned int ebCount);

		private:
			unsigned int m_VAO, m_VBO, m_EBO;
			std::vector<VAOAttrib> m_AttribList;
	};
}

#endif