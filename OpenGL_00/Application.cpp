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
30/06/2018 (Sat) - VERTEX arrays in OpenGL.
9/07/2018 (Mon) - VERTEX arrays in OpenGL (going another round).
10/07/2018 (Tue) - Abstracting into classes: add some annotations and minr code fixes.
11/07/2018 (Thu) - A bit more annotations before start to break into classes.
11/07/2018 (thu) - BREAK INTO CLASSES: we start with VERTEX-BUFFR and INDEX-BUFFER (VERTEX-ARRAY(vao) is left for another "episode, because it is complex, and involves the SHADERS).
11/07/2018 (Thu) - Add the INDEX and Array CLASSES buffers to the application.
14/07/2018 (Sat) - Add the VertexArray and VertexBufferLayout CLASS abstraction.
15/07/2018 (Sun) - Shader abstraction: the 'Shader' class.
26/07/2018 (Thus) - Add the Randerer class - renderer.Draw(va, ib, shader); - this simply incorporates our Vertex-Array, Index-Buffer (defines triangles), Shader (makes the colors chage).

*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream> // so we can read our invented 'Basic.shader' souce-file.
#include <string> // Dunno why this gut suddenly included <string>, it seems to be implicitly included in <iostream> all this time anyway...
#include <sstream> // This is so our "ParseShader" function can create our shader programs for us.
using namespace std;

#include "Renderer.h"

// 12/07/2018 (Thu) - start getting our CLASS dichotomy in place, starting with the VERTEX & INDEX buffers:
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"


int main(void)
{
	GLFWwindow* window;

//#define GLFW
//#define GLEW // I AM ONLY USING GLEW NOW - AM getting rid of extranious definitions.

	/* Initialize the library (required for BOTH GLFW & GLEW) */
	if (!glfwInit())
		return -1;

	/* VertexArrays in OpenGL: CREATE A CONTEXT: OK, here we are getting padantic... Setting to ver 3.3...*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // Yeahy, this whit doesn't work anymore with VertexArrays
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // VertexArrays in OpenGL: CREATE A CONTEXT

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

	/*
	SETUP GLEW data.
	NOTE this is just state-machine data, the ORDER in which these statements are executed does not matter: they just APPLY DATA to our GLEW state-machine.
	GLEW: see documentation on docs.gl
	*/

	if (glewInit() != GLEW_OK) // GLEW
		cout << "glewInit() ERROR!" << endl;

	cout << glGetString(GL_VERSION) << endl;

	// GLEW / VERTEX BUFFER EXERCISE:
	// each line is a VERTEX (= a point):
	// Our application MAY typically have ONE CENTRAL VERTEX BUFFER - EG, if we are defining a SPACESHIP shape, it will encompass the
	// ENTIRE spaceship: EG metal wings, metal hub, tranparent windows - etc.
	float positions[] = {
		/*TRIANGLE indexes*/
		/* +0.0f,  +0.5f, - 0.5f, -0.5f, + 0.5f, -0.5f, */
		/*SQUARE indexes*/
		-0.5f, -0.5f, // 0 index
		+0.5f, -0.5f, // 1 index
		+0.5f,  +0.5f, // 2 index
		-0.5f, +0.5f, // 3 index
	};

	// INDEX BUFFER (the triangles composing or OpenGL shape):
	// There may be MANY INDEX-BUFFERS to a VERTEX-BUFFER - each INDEX-BUFFER defining a SEGMENT of the global VERTEX-BUFFER -
	// EG if the VERTEX-BUFFER defines a SPACESHIP - each INDEX-BUFFER describes a SECTION of that SPACESHIP -
	// EG metal wings, metal hub, tranparent windows - etc.
	unsigned int indices[] = {
		0, 1, 2, // 1st triangle (on right)
		2, 3, 0, // 2nd triangle (on left)
	};

	//VERTEX ARRAY in OpenGL: Connects a VERTEX-BUFFER with "some kind of" LAYOUT:
	//THE LAYOUT is'nt terribly necessary, but is convenient for DEBUGGING stuff on the CPU (though we COULD debug this stuff on the GPU, if we wanted).
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao)); // Bind the vertex array.

	// Our VERTEX BUFFER: Takes in our FLOAT POSITIONS ("positions") array:
	// The VERTEX-BUFFER is actually just a "stupid" lump of data: it doesn't have a notion of what KIND or TYPE of data it represents:
	// We rely on the VERTEX-ARRAY to associate the VERTEX-BUFFER with some kind of LAYOUT...
	// EG
	VertexArray va; // DEFINE OUR "wrapper" VERTEX ARRAY OBJECT
	VertexBuffer vb(positions, sizeof(positions)); // 12/07/2018 (Thu): we use our new Classes for this.
		// This is actually "binded" for us in the construter. so - theoretically - nwe don't need to call "vb.bind()". BUT: we will need to if we bind additional "positions" along the way.

	// LAYOUT:
	VertexBufferLayout layout;
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 6); // OUR INDEX BUFFER (SQUARE made of 2 triangles)

	Shader shader("res/shaders/Basic.shader");
	shader.Bind();
	shader.Setuniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
																							 
	// 14/07/2018 (Sat):
	va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();

	Renderer renderer;

	// A couple of variables to "animate" the color in our "for-loop" and make it more exciting.
	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear(); // We clear the screen - if we want...

		shader.Bind();
		shader.Setuniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

		renderer.Draw(va, ib, shader); // This is where we are drawing our rectangle.

		// This is where we are adjusting the "uniform" variable in the shader that changes the colour of the rectangle.
		if (r > 1.0f)
			increment = -0.05f;
		if (r < 0.0f)
			increment = +0.05f;
		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}