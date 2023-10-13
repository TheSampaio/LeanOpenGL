#ifndef LEARNOPENGL_SHADER_H
#define LEARNOPENGL_SHADER_H

class Shader
{
public:
	Shader(const char* VertexShaderFile, const char* FragmentShaderFile);
	~Shader();

	// Binds the shader program
	inline void Bind() { glUseProgram(m_Id); }

	// Unbinds the shader program
	inline void Unbind() { glUseProgram(0); }

	// Returns the shader program's ID
	inline GLuint& GetId() { return m_Id; }

private:
	// Attributes
	GLuint m_Id,
		m_VertexShader,
		m_FragmentShader;

	std::string LoadShader(const char* FilePath);
	void CompileShader(const char* FilePath, GLuint& ShaderId, GLenum ShaderType);
	void CreateShaderProgram(GLuint VextexShader, GLuint FragmentShader);
};

#endif // !LEARNOPENGL_SHADER_H
