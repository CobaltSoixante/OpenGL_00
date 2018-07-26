#include "Renderer.h"

#include <iostream>
using namespace std;

void GLClearError(void)
{
	while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, const int line)
{
	while (GLenum error = glGetError() != GL_NO_ERROR)
	{
		cout << "[OpenGL Error] " << "(" << error << ')' << "=(0x" << hex << error << ")"
			<< function << " " << file << ":" << line << endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::Draw(const VertexArray & va, const IndexBuffer & ib, const Shader & shader) const
{
	shader.Bind();
	va.Bind(); // BIND VERTEX ARRAY
	ib.Bind();

	// DRAW THE TRIANGLES DEFINED IN OUR INDEX=-BUFFER:
	GLCall(glDrawElements(GL_TRIANGLES, ib.getCount() /*indices*/, GL_UNSIGNED_INT /*typeOf data in the indeces buffer*/, nullptr/*we did a 'glBindBuffer()' to 'ibo' so no need to explicitly specify anything here.*/));
}
