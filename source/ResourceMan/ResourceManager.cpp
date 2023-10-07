#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"

ResourceManager::ResourceManager(const std::string& executablePath){
    size_t found = executablePath.find_last_of("/\\");
    e_path = executablePath.substr(0, found);
}

std::string ResourceManager::getFileCode(const std::string& relFilePath) const{
    std::ifstream file;
    file.open(e_path + "/" + relFilePath.c_str(), std::ios::in | std::ios::binary);
    if(!file.is_open()){
        std::cerr << "Failed to open file: " << relFilePath << "\n";
        return std::string{};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::loadShaderProgram(const std::string& shaderProgramName, const std::string& vertexPath, const std::string& FragmentPath){
    std::string vertexCode = getFileCode(vertexPath);
    if(vertexCode.empty()){
        std::cerr << "No vertex shader code!" << "\n";
        return nullptr;
    }

    std::string fragmentCode = getFileCode(FragmentPath);
    if(fragmentCode.empty()){
        std::cerr << "No fragment shader code!" << "\n";
        return nullptr;
    }

    shader_programs.emplace(shaderProgramName, std::make_shared<Renderer::ShaderProgram>(vertexCode, fragmentCode));
    std::shared_ptr<Renderer::ShaderProgram> newShader = shader_programs.at(shaderProgramName);
    
    if (newShader->checkCompilationSuccess()){
        return newShader;
    }

    std::cerr << "Can not load shader program:\nVertex: " << vertexPath << "\n" << "Fragment: " << FragmentPath << "\n";
    return nullptr;
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderProgramName){
    ShaderProgramsMap::const_iterator it = shader_programs.find(shaderProgramName);
    if(it != shader_programs.end()){
        return it->second;
    }
    
    std::cerr << "Can not find shader program!\n";
    return nullptr;
}