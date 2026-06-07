#include "Button.h"
#include "FontAtlas.h"
#include "Theme.h"
#include "UIManager.h"
#include <iostream>

Button::Button(float x, float y, float w, float h, const std::string &label,
               const FontAtlas &font, glm::vec4 color, glm::vec4 hoverColor)
    : Widget(x, y, w, h, label), font(font), color(color),
      hoverColor(hoverColor) {
  textWidth = 0;
  for (char c : label) {
    if (c >= 32 && c <= 126) {
      textWidth += font.charData.data[c - 32].xadvance * font.scale;
    }
  }
}

void Button::update(float mouseX, float mouseY, bool mousePressed,
                    UIManager &ui) {
  Widget::update(mouseX, mouseY, mousePressed, ui);

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

void Button::draw(UIManager &ui) {
  const auto &theme = Theme::get();

  glm::vec4 finalColor =
      hovered ? theme.colors.surfaceHover : theme.colors.surface;
  if (isPressed)
    finalColor = theme.colors.accent * 0.8f;

  ui.rect(position.x, position.y, size.x, size.y, finalColor);

  float xPos = position.x + (size.x - textWidth) / 2.0f;
  float yPos = position.y + (size.y / 2.0f) + font.baselineOffset;

  ui.drawString(xPos, yPos, label, font, glm::vec4(1.0f));
}