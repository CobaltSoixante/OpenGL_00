#shader vertex
#version 330 core // GLSL (OpenGL shading language), v330, core=we can't use deprecated functions (so we are essentially using latest&greates).
 // newline for aestetics
layout(location = 0) in vec4 position; // index of 1st attribute in glVertexAttribPointer(...)... HAS to be vec4 eventually, evenn though our glVertexAttribPointer(...) indicates '2' as # of components to attribute. 

void main()
{
	gl_Position = position; // position defaults to 1(?)
}
;

#shader fragment
#version 330 core // GLSL (OpenGL shading language), v330, core=we can't use deprecated functions (so we are essentially using latest&greates).
 // newline for aestetics
layout(location = 0) out vec4 color; // index of 1st attribute in glVertexAttribPointer(...)... HAS to be vec4 eventually, evenn though our glVertexAttribPointer(...) indicates '2' as # of components to attribute. 

uniform vec4 u_Color; // color variable for the GPU that we can modify from the CPU in our application.cpp code. The writer's "stryle" is to prefix the variable name with "u_".  

void main()
{
	//color = vec4(1.0, 0.0, 0.0, 1.0); // color=RED, 4th parameter is ALPHA value (we set to 1.0). - params here are RGBA . 'vec4' means we're passing 4 floats.
	//color = vec4(0.2, 0.3, 0.8, 1.0); // Changing this to BLUE...
	color = u_Color; // Use the new shader UNIFORM variable so th CPP code running on the CPU can set it independantly.
}
;
