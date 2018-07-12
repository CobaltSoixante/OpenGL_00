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


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}
