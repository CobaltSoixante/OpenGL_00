#pragma once

#include <vector>
using namespace std;

#include <GL/glew.h>

#include "Renderer.h"

/*
This is the LAYOUT for our INDEX-BUFFER:
defines what EACH ELEMENT in our indes-buffer REALL IS!
*/

/* This is sorta modelled after what we used initially in "Application.cpp":
GLCall(glVertexAttribPointer( // BIND this IndexArray to the GLArrayBuffer (==links "buffer" with "vao")
0,	// Index 0 of the VERTEX array: 1st attribute
2,	// # of compoenents per attribute
GL_FLOAT,	// type of data
GL_FALSE,	// FALSE: we don't want these to be normalized, thay are ALREADY floats...
sizeof(float) * 2,	// STRIDE: number of vytes between each vertex. (= how many bytes to go to get to the next vertex).
0			// Point/index to First attribute
));
*/
struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch(type)
		{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();
	virtual ~VertexBufferLayout();

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>  // We can push a number of "floats" in...
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		this->m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		this->m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		this->m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const vector<VertexBufferElement> GetElements(void) const { return(m_Elements); }
	inline unsigned int GetStride() const { return (m_Stride); }
private:
	vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
};

