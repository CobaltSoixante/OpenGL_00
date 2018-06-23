/*
This source is based on stuff I garnered from watching videos of "TheChernoProject"
( https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=1 )
I will have to check with him if it is OK for me to check this material into GitHub. Otherwise - I may need to obliterate it.
(He has understandable reasons for MAYBE wanting to keep the written code to himself). 
*/
/* HISTORY:
9/06/2018 (Sat) - Beginning changes to come into line with TheChernoProct video "How I Deal with Shaders in OpenGL" ( https://www.youtube.com/watch?v=2pv0Fbo-7ms&index=8&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2 ). 
10/06/2018 (Sun) - Have modified the code to read in the configuration file I read off the Cherno project yesterday.
16/06/2018 (Sat) - Index Buffers: how every thing in OpenGL is based on TRIANGLES (even if they are SQUARES or 3D), and our "plight" to reduce our vertixes ti "index buffers".
23/06/2018 (Sat) - Dealing with errors in OpenGL.
23/06/2018 (Sat_ - USING "UNIFORM" VARIABLES IN SHADERS.
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream> // so we can read our invented 'Basic.shader' souce-file.
#include <string> // Dunno why this gut suddenly included <string>, it seems to be implicitly included in <iostream> all this time anyway...
#include <sstream> // This is so our "ParseShader" function can create our shader programs for us.
using namespace std;

// Just inseting error-checking code fro the error-checcing video...
// ... The upper-case 'GL' is an indication to US that this is our own custom code (as opposed to lower-case 'gl' which is the prefix for bona-fide OpenGL funcs).
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) /*'x' is a function call we call this macro with*/ \
	x; /*Make the function call*/ \
	ASSERT( GLLogCall(#x, __FILE__, __LINE__) )
static void GLClearError(void) {
	while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char* file, const int line) {
	while (GLenum error = glGetError() != GL_NO_ERROR)
	{
		cout << "[OpenGL Error] " << '(' << error << ')' << "=(0x" << hex << error <<')'
			<< function << ' ' << file << ':' << line << endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource {
	string VertexSource;
	string FragmentSource;
};
static ShaderProgramSource ParseShader(
	const string& filepath // Name of the shader program file we invented.
	)
{
	ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, 
	};
	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != string::npos) { // Fileter out this fake "#shader ..." syntax we "invented"... 
			if (line.find("vertex") != string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int(type))] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

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

	/* Moderate the window's refresh rate to be in sync with our VSYNC -
	this is for when we do the UNIFORM variable tutorial, so our color-change animation changes at a reasonable/aesthetic rate. */
	glfwSwapInterval(1);

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
	float positions[] = {
		/*TRIANGLE indexes*/ /*
		+0.0f,  +0.5f,
		- 0.5f, -0.5f,
		+ 0.5f, -0.5f,
		*/
		/*SQUARE indexes*/
		-0.5f, -0.5f, // 0 index
		+0.5f, -0.5f, // 1 index
		+0.5f,  +0.5f, // 2 index
		-0.5f, +0.5f, // 3 index
	};

	unsigned int indeces[] = {
		0, 1, 2, // 1st triangle (on right)
		2, 3, 0, // 2nd triangle (on left)
	};

	// GLEW: see documentation on docs.gl
	unsigned int buffer;
	glGenBuffers(1, &buffer); // 1 = how many buffers we want; the OpenGL ID of the buffer will be droppen into &buffer.
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // GL_ARRAY_BUFFER - means this is simply a buffer of memory. (We haven't even specified how LARGE this buffer will be).
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); // Specify how LARGE the buffer will be (6 floats); GL_STATIC_DRAW - means contents will be modified once & used many times.

	glEnableVertexAttribArray(0); // Enable vertex #0.
	glVertexAttribPointer(
		0,	// Index 0: 1st attribute
		2,	// # of compoenents per attribute
		GL_FLOAT,	// type of data
		GL_FALSE,	// FALSE: we don't want these to be normalized, thay are ALREADY floats...
		sizeof(float) * 2,	// STRIDE: number of vytes between each vertex. (= how many bytes to go to get to the next vertex).
		0			// Point/index to First attribute
	);

	/* SQUARE */
	unsigned int ibo; // IBO = Index Buffer Object
	GLCall(glGenBuffers(1, &ibo)); // 1 = how many buffers we want; the OpenGL ID of the buffer will be droppen into &buffer.
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // GL_ARRAY_BUFFER - means this is simply a buffer of memory. (We haven't even specified how LARGE this buffer will be).
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW)); // SEND DATA TO GPU.
																				 /* CREATE OUR 2 SHADERS (VERTEX & FRAGMENT) */
	/* 10/06/2018 - comment out strings: we now use "Basic.shader" file that we invented.
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
	*/
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	cout << "VERTEX" << endl;
	cout << source.VertexSource << endl;
	cout << "FRAGMENT" << endl;
	cout << source.FragmentSource << endl;
	//unsigned int shader = CreateShader(vertexShader, fragmentShader);
	//glUseProgram(shader); // Bind our shader.
	//unsigned int shader = CreateShader(vertexShader, fragmentShader);
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader)); // Bind our shader.

	// USING "UNIFORM" VARIABLES IN SHADERS:
	GLCall(int location = glGetUniformLocation(shader, "u_Color")); // retrieve the location/reference of the custom "u_Color" variable I created.
	ASSERT(-1 != location); // Ensure the Uniform "u_Color" variable actually exists (it doesn't always have to, if we didn't explicitly USE it in our shader code).
	//GLCall(glUniform4f(location, 0.2, 0.3, 0.8, 1.0));  // set my uniform variable with the sam BLUE color my "FRAGMENT" shader had originally hard-coded.
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));  // Let's make it pink...

#endif // GLEW

	// A couple of variables to "animate" the color in our "for-loop" and make it more exciting.
	float r = 0.0f;
	float increment = 0.05f;
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
		//glDrawArrays(GL_TRIANGLES, 0, 6); // Drawing a GL_TRIANGLES, beginning at vertex#0 (the 1st one), and 3 vertexes ('cuz this is a triangle).
			// This will KNOW to draw our PARTICULAR triangle due to the "glBindBuffer(GL_ARRAY_BUFFER, buffer);" statement prior this loop.
		// AM NOW USING THE PRESENTER'S ERROR CHECKING MECHANISMS...
		//GLClearError(); // Just inseting error-checking code fro the error-checcing video...
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));  // Let's make it pink...
		GLCall( glDrawElements(GL_TRIANGLES, 6/*indices*/, GL_UNSIGNED_INT /*typeOf data in the indeces buffer*/, nullptr/*we did a 'glBindBuffer()' to 'ibo' so no need to explicitly specify anything here.*/) );

		if (r > 1.0f)
			increment = -0.05f;
		if (r < 0.0f)
			increment = +0.05f;
		r += increment;

		//ASSERT(GLLogCall()); // Just inseting error-checking code fro the error-checcing video...
#endif // GLEW

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

#ifdef GLEW // For good measure:
	//glDeleteShader(shader); // this should actually be glDeleteProgram(shader);
	glDeleteProgram(shader);
#endif GLEW

	glfwTerminate();
	return 0;
}