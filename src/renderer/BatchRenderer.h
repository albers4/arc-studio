#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

struct Vertex {
    glm::vec2 pos;
    glm::vec2 tex;
    glm::vec4 col;
};

class BatchRenderer {
private:
    Shader& shader;
    GLuint VAO = 0, VBO = 0;
    std::vector<Vertex> solidVerts;
    std::vector<Vertex> textVerts;
    static constexpr size_t MAX_BUFFER_SIZE = 1024 * 1024;

public:
    BatchRenderer(Shader& sh);
    ~BatchRenderer();

    void rect(float x, float y, float w, float h, glm::vec4 col);
    void textQuad(float x, float y, float w, float h, float u0, float v0, float u1, float v1, glm::vec4 col);
    void flush(const glm::mat4& proj, GLuint textureID);
    
    BatchRenderer(const BatchRenderer&) = delete;
    BatchRenderer& operator=(const BatchRenderer&) = delete;
};