#include "VertexArray.h"

#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &this->m_RendererID));
}


VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &this->m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	this->Bind(); // BIND this vertex array.
	vb.Bind(); // BIND the buffer we want to deal with.

	// SETUP THE LAYOUT:
	const auto& elements = layout.GetElements();
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		unsigned int offset = 0;
		// OUR "Vertex Attribute" STUFF, WHICH IS THE ACTUAL LAYOUT OF OUR VERTEX BUFFER:
		// IS NOW DEALT DEALT WITH IN OUR "VERTEX ARRAY" CLASS ("vao") (now that we broke this stuff DOWN TO CLASSES).
		GLCall(glEnableVertexAttribArray(i)); // Enable vertex #i.
		GLCall(glVertexAttribPointer( // BIND this IndexArray to the GLArrayBuffer (==links "buffer" with "vao")
			i,	// Index 0 of the VERTEX array: 1st attribute
			element.count,	// # of compoenents per attribute
			element.type,	// type of data
			element.normalized,	// FALSE: we don't want these to be normalized, thay are ALREADY floats...
			layout.GetStride(),	// STRIDE: number of vytes between each vertex. (= how many bytes to go to get to the next vertex).
			(const void*) offset	// Point/index to First attribute
		));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);

	}

}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(this->m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(this->m_RendererID));
}
