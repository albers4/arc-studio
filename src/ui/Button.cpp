#include "Button.h"
#include "UIManager.h"
#include "FontAtlas.h"

Button::Button(
    float x, 
    float y, 
    float w, 
    float h, 
    const std::string& label, 
    const FontAtlas& font,
    glm::vec4 color,
    glm::vec4 hoverColor
)
    : Widget(x, y, w, h, label), font(font), color(color), hoverColor(hoverColor) {
        textWidth = 0;
        for (char c : label) {
            if (c >= 32 && c <= 126) {
                textWidth += font.charData.data[c - 32].xadvance;
            }
        }
    }

void Button::draw(UIManager& ui) {
    glm::vec4 finalColor = hovered ? hoverColor : color;
    ui.rect(position.x, position.y, size.x, size.y, finalColor);

    float xPos = position.x + (size.x - textWidth) / 2.0f;

    float fontHeight = font.ascent - font.descent;
    float baselineOffset = (font.ascent + font.descent) / 2.0f;
    float yPos = position.y + (size.y / 2.0f) + baselineOffset;

    ui.drawString(
        xPos,
        yPos,
        label,
        font,
        glm::vec4(1.0f));
}