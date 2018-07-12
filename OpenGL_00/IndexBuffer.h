#pragma once
class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count; // number of indeces in or buffer (REMEMBER: 3 indeces per each Traiangle in our shape).

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	virtual ~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int getCount(void) const { return m_Count; };
};

