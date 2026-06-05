#include "BatchRenderer.h"

BatchRenderer::BatchRenderer(Shader& sh)
    : shader(sh)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

BatchRenderer::~BatchRenderer() {
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}
    
void BatchRenderer::rect(float x, float y, float w, float h, glm::vec4 col) {
    solidVerts.push_back({{x, y}, {0, 0}, col});
    solidVerts.push_back({{x+w, y}, {0, 0}, col});
    solidVerts.push_back({{x+w, y+h}, {0, 0}, col});
    solidVerts.push_back({{x, y}, {0, 0}, col});
    solidVerts.push_back({{x+w, y+h}, {0, 0}, col});
    solidVerts.push_back({{x, y+h}, {0, 0}, col});
}

void BatchRenderer::textQuad(float x, float y, float w, float h, float u0, float v0, float u1, float v1, glm::vec4 col) {
    textVerts.push_back({{x, y}, {u0, v0}, col});
    textVerts.push_back({{x+w, y}, {u1, v0}, col});
    textVerts.push_back({{x+w, y+h}, {u1, v1}, col});
    textVerts.push_back({{x, y}, {u0, v0}, col});
    textVerts.push_back({{x+w, y+h}, {u1, v1}, col});
    textVerts.push_back({{x, y+h}, {u0, v1}, col});
}


void BatchRenderer::flush(const glm::mat4& proj, GLuint textureID) {
    shader.use();
    shader.setMat4("uProjection", proj);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // --- DRAW SOLIDS ---
    if (!solidVerts.empty()) {
        shader.setBool("uUseTexture", false);
        glBufferSubData(GL_ARRAY_BUFFER, 0, solidVerts.size() * sizeof(Vertex), solidVerts.data());
        glDrawArrays(GL_TRIANGLES, 0, solidVerts.size());
    }
    
    // --- DRAW TEXT ---
    if (!textVerts.empty()) {
        shader.setBool("uUseTexture", true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setInt("uTexture", 0);
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, textVerts.size() * sizeof(Vertex), textVerts.data());
        glDrawArrays(GL_TRIANGLES, 0, textVerts.size());
    }
    
    solidVerts.clear();
    textVerts.clear();
}