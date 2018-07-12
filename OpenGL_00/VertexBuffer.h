#pragma once
class VertexBuffer
{
private:
	unsigned int m_RendererID; // HECK, everything in OpenGL as an "ID", so we give this object an ID as well.
public:
	VertexBuffer(const void* data, unsigned int size);
	virtual ~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};

