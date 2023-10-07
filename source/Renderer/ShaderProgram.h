#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

namespace Renderer{
	class ShaderProgram{
	public:
		unsigned int ID;
		ShaderProgram(std::string& vertexCode, std::string& fragmentCode);
		~ShaderProgram();
		ShaderProgram(ShaderProgram&& shaderProgram);
		ShaderProgram& operator=(ShaderProgram&& shaderProgram);

		ShaderProgram() = delete;
		ShaderProgram(ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void use();
		bool checkCompilationSuccess();

		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec3f(const std::string& name, float v1, float v2, float v3);
		void setVec4f(const std::string& name, float v1, float v2, float v3, float v4);
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		void setMat4(const std::string &name, const glm::mat4 &mat) const;

	private:
		bool isCompiled = false;
		void checkCompileErrors(GLuint id, std::string type);
	};
}