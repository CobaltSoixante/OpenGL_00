#include "Shader.h"

#include <iostream>
#include <fstream> // so we can read our invented 'Basic.shader' souce-file.
#include <string> // Dunno why this gut suddenly included <string>, it seems to be implicitly included in <iostream> all this time anyway...
#include <sstream> // This is so our "ParseShader" function can create our shader programs for us.
using namespace std;

#include "Renderer.h"

Shader::Shader(const string& filepath) :
	m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	cout << "VERTEX" << endl;
	cout << source.VertexSource << endl;
	cout << "FRAGMENT" << endl;
	cout << source.FragmentSource << endl;
	this->m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(this->m_RendererID));
}

ShaderProgramSource Shader::ParseShader(
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

unsigned int Shader::CompileShader(unsigned int type, const string& source) {
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
		char* message = (char*)alloca(length * sizeof(char));
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
unsigned int Shader::CreateShader(const string& vertexShader, const string& fragmentShader) {
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

void Shader::Bind() const
{
	GLCall(glUseProgram(this->m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::Setuniform4f(const string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const string& name)
{
	if (this->m_UniformLocationCache.find(name) != this->m_UniformLocationCache.end())
		return this->m_UniformLocationCache[name];

	GLCall(unsigned int location = glGetUniformLocation( this->m_RendererID, name.c_str() ));
	if (-1 == location)
		cout << "Warning: uniform '" << name << "' doesn't exist!" << endl;
	
	m_UniformLocationCache[name] = location;
	return location;
}
