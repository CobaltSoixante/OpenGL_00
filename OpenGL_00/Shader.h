#pragma once

#include <string>
#include <unordered_map>
using namespace std;

struct ShaderProgramSource
{
	string VertexSource;
	string FragmentSource;
};


class Shader
{
private:
	const string m_FilePath; // Just for debug purposes: save the filepath of our original shader source text material.
	int m_RendererID;
	//Caching for uniforms:
	unordered_map<string, unsigned int> m_UniformLocationCache;

public:
	Shader(const string& filepath);
	virtual ~Shader();

	void Bind() const;
	void Unbind() const;

	// Set unifor,s:
	void Setuniform4f(const string& name, float v0, float v1, float v2, float v3);

private:
	ShaderProgramSource ParseShader(
		const string& filepath // Name of the shader program file we invented.
	);
	unsigned int CompileShader(unsigned int type, const string& source);
	unsigned int CreateShader(const string& vertexShader, const string& fragmentShader);
	unsigned int GetUniformLocation(const string& name);
};

