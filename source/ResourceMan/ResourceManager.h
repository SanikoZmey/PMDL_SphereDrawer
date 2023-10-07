#pragma once

#include <string>
#include <memory>
#include <map>

#include <iostream>
#include <sstream>
#include <fstream>

namespace Renderer{
    class ShaderProgram;
}

class ResourceManager{
public:
    ResourceManager(const std::string& executable_path);
    ~ResourceManager() = default;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    std::shared_ptr<Renderer::ShaderProgram> loadShaderProgram(const std::string& shaderProgramName, const std::string& vertexPath, const std::string& FragmentPath);
    std::shared_ptr<Renderer::ShaderProgram> getShaderProgram(const std::string& shaderProgramName);
private:
    typedef std::map<const std::string, std::shared_ptr<Renderer::ShaderProgram>> ShaderProgramsMap;
    ShaderProgramsMap shader_programs;

    std::string e_path;

    std::string getFileCode(const std::string& relFilePath) const;
};