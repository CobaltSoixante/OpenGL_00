#pragma once

#include <GL/glew.h>

/*** OUR GENERIC ERROR HANDLING CODE ***/
// Just inseting error-checking code fro the error-checcing video...
// ... The upper-case 'GL' is an indication to US that this is our own custom code (as opposed to lower-case 'gl' which is the prefix for bona-fide OpenGL funcs).
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) /*'x' is a function call we call this macro with*/ \
	GLClearError();\
	x; /*Make the function call*/ \
	ASSERT( GLLogCall(#x, __FILE__, __LINE__) )
void GLClearError(void);
bool GLLogCall(const char* function, const char* file, const int line);

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();
};

