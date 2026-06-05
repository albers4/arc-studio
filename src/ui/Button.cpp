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
    : Widget(x, y, w, h, label), font(font), color(color), hoverColor(hoverColor)
{
    textWidth = 0;
    for (char c : label) {
        if (c >= 32 && c <= 126) {
            textWidth += font.charData.data[c - 32].xadvance;
        }
    }
}

void Button::update(float mouseX, float mouseY, bool mousePressed) {
    hovered = contains(mouseX, mouseY);

    bool justPressed = mousePressed && !wasPressed;

    if (hovered && justPressed) {
        isPressed = true;
    }

    if (!mousePressed) {
        if (isPressed && hovered && onClick) {
            onClick();
        }

        isPressed = false;
    }

    wasPressed = mousePressed;
}

void Button::draw(UIManager& ui) {
    glm::vec4 finalColor = color;
    if (isPressed) {
        finalColor = hoverColor - glm::vec4(0.15f, 0.15f, 0.15f, 0.0f);
    } else if (hovered) {
        finalColor = hoverColor;
    }

    ui.rect(position.x, position.y, size.x, size.y, finalColor);

    float xPos = position.x + (size.x - textWidth) / 2.0f;
    float yPos = position.y + (size.y / 2.0f) + font.baselineOffset;

    ui.drawString(
        xPos,
        yPos,
        label,
        font,
        glm::vec4(1.0f)
    );
}