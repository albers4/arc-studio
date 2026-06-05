#pragma once
#include "Widget.h"
#include "FontAtlas.h"
#include <glm/glm.hpp>

class Button : public Widget {
private:
    const FontAtlas& font;
    float textWidth;

    glm::vec4 color;
    glm::vec4 hoverColor;

    bool isPressed = false;
    bool wasPressed = false;

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
    void update(float mouseX, float mouseY, bool mousePressed) override;
    void draw(UIManager& ui) override;
};