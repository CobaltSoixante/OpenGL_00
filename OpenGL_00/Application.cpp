/*
This source is based on stuff I garnered from watching videos of "TheChernoProject"
( https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=1 )
I will have to check with him if it is OK for me to check this material into GitHub. Otherwise - I may need to obliterate it.
(He has understandable reasons for MAYBE wanting to keep the written code to himself). 
*/

/* HISTORY:
9/06/2018 (Sat) - Beginning changes to come into line with TheChernoProct video "How I Deal with Shaders in OpenGL" ( https://www.youtube.com/watch?v=2pv0Fbo-7ms&index=8&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2 ). 
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

static unsigned int CompileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // id of shader, only 1 shader code source, &src (for some reason), array of string lengths (nullptr assumes that each string is nul-terminated).
	glCompileShader(id); // Compile the shader code.

	// Error Handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		// Get the error message
		int length; // error message length.
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message); // GOD only knows why it wants the 'length' and the POINTER to the 'length'.
		cout
			<< "Failed to compile shader of type "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< endl;
		cout << message << endl;
		glDeleteShader(id); // Delete it: it failed anyway.
		return 0;
	}

	return id;
}

// Shader Function (the in return-code is the OpenGL ID for our compiled shader).
static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) {
	unsigned int program = glCreateProgram(); // Create the combined shader program.

	// Create the 2 shaders:
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach the 2 above shaders to our program:
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program); // LINK the program
	glValidateProgram(program); // VALIDATE the program.

	// NOW that our shaders have been "compiled"/"baked" into a "program" - we can DELETE them -
	// much as we can delete object files after they have been linked into a final executable.
	glDeleteShader(vs);
	glDeleteShader(fs);

	// TECHNICALLY, we should call glDetachShader() at this point but few developers or game engines bother -
	// the memory gain is miniscule, and the source-code lost is a valuable debug-tool if problems arise.

	return program;
}


int main(void)
{
	GLFWwindow* window;

	//#define GLFW
	#define GLEW

	/* Initialize the library (required for BOTH GLFW & GLEW) */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

#ifdef GLEW
	/*
	SETUP GLEW data.
	NOTE this is just state-machine data, the ORDER in which these statements are executed does not matter: they just APPLY DATA to our GLEW state-machine.
	*/
	if (glewInit() != GLEW_OK) // GLEW
		cout << "glewInit() ERROR!" << endl;
	
	cout << glGetString(GL_VERSION) << endl;

	// GLEW / VERTEX BUFFER EXERCISE:
	// each line is a VERTEX (= a point):
	float positions[6] = {
		+0.0f,  +0.5f,
		- 0.5f, -0.5f,
		+ 0.5f, -0.5f,
	};

	// GLEW: see documentation on docs.gl
	unsigned int buffer;
	glGenBuffers(1, &buffer); // 1 = how many buffers we want; the OpenGL ID of the buffer will be droppen into &buffer.
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // GL_ARRAY_BUFFER - means this is simply a buffer of memory. (We haven't even specified how LARGE this buffer will be).
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); // Specify how LARGE the buffer will be (6 floats); GL_STATIC_DRAW - means contents will be modified once & used many times.

	glEnableVertexAttribArray(0); // Enable vertex #0.
	glVertexAttribPointer(
		0,	// Index 0: 1st attribute
		2,	// # of compoenents per attribute
		GL_FLOAT,	// type of data
		GL_FALSE,	// FALSE: we don't want these to be normalized, thay are ALREADY floats...
		sizeof(float)*2,	// STRIDE: number of vytes between each vertex. (= how many bytes to go to get to the next vertex).
		0			// Point/index to First attribute
	);

	/* CREATE OUR 2 SHADERS (VERTEX & FRAGMENT) */
	string vertexShader =
		"#version 330 core\n" // GLSL (OpenGL shading language), v330, core=we can't use deprecated functions (so we are essentially using "latest&greates").
		"\n" // newline for aestetics
		"layout(location = 0) in vec4 position;" // index of 1st attribute in glVertexAttribPointer(...)... HAS to be vec4 eventually, evenn though our glVertexAttribPointer(...) indicates '2' as # of components to attribute. 
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n" // position defaults to 1(?)
		"}\n"
		;
	string fragmentShader =
		"#version 330 core\n" // GLSL (OpenGL shading language), v330, core=we can't use deprecated functions (so we are essentially using "latest&greates").
		"\n" // newline for aestetics
		"layout(location = 0) out vec4 color;" // index of 1st attribute in glVertexAttribPointer(...)... HAS to be vec4 eventually, evenn though our glVertexAttribPointer(...) indicates '2' as # of components to attribute. 
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);" // color=RED, 4th parameter is ALPHA value (we set to 1.0). - params here are RGBA .
		"}\n"
		;

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader); // Bind our shader.

#endif // GLEW

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

#ifdef GLFW
		/* DRAW TRIANGLE USING FGLW: AW: CUSTOM CODE FROM THE YOUTUBE */
		glBegin(GL_TRIANGLES);
		glVertex2f(+0.0f, +0.5f); // TOP point
		glVertex2f(-0.5f, -0.5f); // LEFT bottom point
		glVertex2f(+0.5f, -0.5f); // RIGHT bottom point
		glEnd();
#endif // GLFW

#ifdef GLEW /* DRAW USING GLEW */
		glDrawArrays(GL_TRIANGLES, 0, 3); // Drawing a GL_TRIANGLES, beginning at vertex#0 (the 1st one), and 3 vertexes ('cuz this is a triangle).
			// This will KNOW to draw our PARTICULAR triangle due to the "glBindBuffer(GL_ARRAY_BUFFER, buffer);" statement prior this loop.
#endif // GLEW

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

#ifdef GLEW // For good measure:
	glDeleteShader(shader); // this should actually be glDeleteProgram(shader);
#endif GLEW

	glfwTerminate();
	return 0;
}