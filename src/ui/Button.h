#pragma once
#include "Widget.h"
#include "FontAtlas.h"
#include <glm/glm.hpp>

class Button : public Widget {
private:
    glm::vec4 color;
    glm::vec4 hoverColor;
    const FontAtlas& font;
    float textWidth;

public:
    Button(
        float x,
        float y, 
        float w, 
        float h, 
        const std::string& label, 
        const FontAtlas& font,
        glm::vec4 color = glm::vec4(0.5f), 
        glm::vec4 hoverColor = glm::vec4(0.7f)
    );
    void draw(UIManager& ui) override;
};