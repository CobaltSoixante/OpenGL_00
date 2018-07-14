#pragma once

/*
The point of the VERTEX-ARRAY is to tie a...
VERTEX-BUFFER - which is just a jumble of incoherent data... WITH:
a LAYOUT - which defines/explains what the data in the VERTEX-BUFFER actually IS!

NOW, JUST REMEMBER: VERTEX-ARRAYS are actually A NATIVE construct of OpenGL: they are not just an abstract class that we are inventing here "on the fly".
(Also - bear in mind - vertex-arrays are a OpenGL "invention" that does not exists in more serious graphics/gaming engines such as DirectX or Direct3D).
*/

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;

public:
	VertexArray();
	virtual ~VertexArray();

	/* BRING TOGETHER A -
	VERTEX-BUFFER
	and a VERTEX-LAYOUT
	in our VERTEX-ARRAY: */
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};

