#ifndef _SUPERpong_VAO
#define _SUPERpong_VAO

#include "GLEW/glew.h"

#include <vector>

namespace OGL
{
	struct AttribInfo
	{
		unsigned int attribIdx;
		unsigned int count;
		unsigned int offset;
		AttribInfo(unsigned int attribIdx, unsigned int count, unsigned int offset)
			: attribIdx(attribIdx), count(count), offset(offset) {}
	};

	struct VBOConfig
	{
		unsigned int bindingIdx;
		unsigned int usage;
		unsigned int stride;
		std::vector<AttribInfo> attributes;
		VBOConfig(unsigned int bindingIdx, unsigned int usage, unsigned int stride, std::vector<AttribInfo> attrbs)
			: bindingIdx(bindingIdx), usage(usage), stride(stride), attributes(attrbs) {}
	};

	class VAO
	{
		public:
			VAO() = delete;
			VAO(const VAO& other) = delete;
			VAO(std::vector<VBOConfig> vboConfigs, std::vector<unsigned int> vboCounts, unsigned int eboCount);
			~VAO();

			void operator=(const VAO& other) = delete;

		public:
			void bind() const;
			void updateVB(std::vector<float> vb, unsigned int bindingIdx, unsigned int offset);
			void updateEB(std::vector<unsigned int> eb, unsigned int offset);

			void reCreateVB(unsigned int vbCount, unsigned int bindingIdx);
			void reCreateEB(unsigned int ebCount);

		private:
			void initialize(std::vector<unsigned int> vbCounts, unsigned int ebCount);

		private:
			unsigned int m_VAO, m_EBO;
			std::vector<VBOConfig> m_VBOConfigs;
			std::vector<unsigned int> m_VBOs;
	};
}

#endif