#include "ShaderProgram.h"

namespace Renderer{
	ShaderProgram::ShaderProgram(std::string& vertexCode, std::string& fragmentCode){
		const char* vShader = vertexCode.c_str();
		const char* fShader = fragmentCode.c_str();

		GLuint vertexShader, fragmentShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShader, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShader, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");

		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	ShaderProgram::~ShaderProgram(){
		glDeleteProgram(ID);
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram){
		ID = shaderProgram.ID;
		isCompiled = shaderProgram.isCompiled;
		shaderProgram.ID = 0;
		shaderProgram.isCompiled = false;
	}
	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram){
		glDeleteProgram(ID);
		ID = shaderProgram.ID;
		isCompiled = shaderProgram.isCompiled;
		shaderProgram.ID = 0;
		shaderProgram.isCompiled = false;

		return *this;
	}
	void ShaderProgram::use(){
		glUseProgram(ID);
	}
	bool ShaderProgram::checkCompilationSuccess(){
		return isCompiled;
	}
	void ShaderProgram::setBool(const std::string& name, bool value) const{ 
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void ShaderProgram::setInt(const std::string& name, int value) const{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ShaderProgram::setFloat(const std::string& name, float value) const{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void ShaderProgram::setVec3f(const std::string& name, float v1, float v2, float v3){
		glUniform3f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3);
	}
	void ShaderProgram::setVec4f(const std::string& name, float v1, float v2, float v3, float v4){
		glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
	}
	void ShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
	void ShaderProgram::checkCompileErrors(GLuint id, std::string type){
		int success;
		char infolog[1024];
		if (type != "PROGRAM") 
		{
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(id, 1024, NULL, infolog);
				std::cout << "ERROR:SHADER:COMPILATION_FAILED type: " << type << "\n" << infolog << "\n--------------------------------\n";
			}
		}
		else
		{
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, 1024, NULL, infolog);
				std::cout << "ERROR:PROGRAM:SHADER_PROGRAM:LINKING_FAILED\n" << infolog << "\n--------------------------------\n";
			}
			else{
				isCompiled = true;
			}
		}
	}
}
