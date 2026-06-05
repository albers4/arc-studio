#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;
    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();
    
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
};