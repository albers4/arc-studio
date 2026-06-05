#pragma once
#include "FontAtlas.h"
#include "../renderer/BatchRenderer.h"
#include <string>

class UIManager {
private:
    BatchRenderer& renderer;
    
public:
    UIManager(BatchRenderer& ren);
    
    void rect(float x, float y, float w, float h, glm::vec4 col);
    void drawString(float x, float y, const std::string& text, const FontAtlas& font, glm::vec4 color = glm::vec4(1.0f));
    void flush(const glm::mat4& proj, const FontAtlas& font);
};