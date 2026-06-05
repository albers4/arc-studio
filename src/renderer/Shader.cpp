#include "Shader.h"
#include "../utils/FileHelper.h"
#include <iostream>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    std::string vertCode = Utils::readFile(vertPath);
    std::string fragCode = Utils::readFile(fragPath);

    auto compileShader = [](GLenum type, const std::string& src) {
        unsigned int shader = glCreateShader(type);
        const char* code = src.c_str();
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);
        
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION: " << infoLog << std::endl;
        }
        return shader;
    };

    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertCode);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragCode);
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    ID = program;
}

Shader::~Shader() { if (ID) glDeleteProgram(ID); }
void Shader::use() const { glUseProgram(ID); }

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}

void Shader::setInt(const std::string &name, int value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc != -1) glUniform1i(loc, value);
}

void Shader::setBool(const std::string &name, bool value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if (loc != -1) glUniform1i(loc, (int)value);
}