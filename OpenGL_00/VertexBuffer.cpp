#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_RendererID)); // 1 = how many buffers we want; the OpenGL ID of the buffer will be droppen into &buffer.
	// Now, BIND THE VERTEX BUFFER:
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // GL_ARRAY_BUFFER - means this is simply a buffer of memory. (We haven't even specified how LARGE this buffer will be).
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); // Specify how LARGE the buffer will be (6 floats); GL_STATIC_DRAW - means contents will be modified once & used many times.
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID)); // Delete only ONE buffer, its ID is the buffer hosted by this Object/Class.
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // GL_ARRAY_BUFFER - means this is simply a buffer of memory. (We haven't even specified how LARGE this buffer will be).
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // Making this 0 - unbinds (I guess).
}
