#include "Slider.h"
#include "UIManager.h"
#include <algorithm>
#include <cstdio>

Slider::Slider(float x, float y, float w, float h, const std::string &label,
               const FontAtlas &font, float min, float max, float startValue)
    : Widget(x, y, w, h, label), font(font), minValue(min), maxValue(max),
      isDragging(false), trackColor(0.3f, 0.3f, 0.35f, 1.0f),
      knobColor(0.6, 0.6, 0.7, 1.0f), hoverColor(0.8f, 0.8f, 0.9, 1.0f) {
  if (maxValue != minValue) {
    value = (startValue - minValue) / (maxValue - minValue);
  } else {
    value = 0.0f;
  }
  value = glm::clamp(value, 0.0f, 1.0f);
}

void Slider::update(float mouseX, float mouseY, bool mousePressed) {
  float knobSize = 12.0f;
  float trackWidth = size.x - knobSize;

  float knobLeft = position.x + (value * trackWidth);
  float knobRight = knobLeft + knobSize;
  float knobTop = position.y + (size.y / 2.0f) - (knobSize / 2.0f);
  float knobBottom = knobTop + knobSize;

  bool mouseOverKnob = (mouseX >= knobLeft && mouseX <= knobRight &&
                        mouseY >= knobTop && mouseY <= knobBottom);

  bool justPressed = mousePressed && !wasPressed;

  if (justPressed && mouseOverKnob) {
    isDragging = true;
  }

  if (!mousePressed) {
    isDragging = false;
  }

  if (isDragging) {
    float relativeX = mouseX - position.x;
    float newValue = relativeX / size.x;
    value = glm::clamp(newValue, 0.0f, 1.0f);

    if (onClick) {
      onClick();
    }
  }

  wasPressed = mousePressed;
  hovered = contains(mouseX, mouseY);
}

void Slider::draw(UIManager &ui) {
  ui.rect(position.x, position.y + (size.y / 2) - 2, size.x, 4, trackColor);

  float knobSize = 12.0f;
  float trackWidth = size.x - knobSize;
  float knobX = position.x + (value * trackWidth) + (knobSize / 2);
  float knobY = position.y + (size.y / 2);

  glm::vec4 finalKnobColor = (hovered || isDragging) ? hoverColor : knobColor;
  ui.rect(knobX - (knobSize / 2), knobY - (knobSize / 2), knobSize, knobSize,
          finalKnobColor);

  char valueString[16];
  float actualValue = minValue + (value * (maxValue - minValue));
  snprintf(valueString, sizeof(valueString), "%.2f", actualValue);

  std::string fullLabel = label + ": " + valueString;
  float textY = position.y + (size.y / 2) + font.baselineOffset;

  ui.drawString(position.x, textY - 10, fullLabel, font, glm::vec4(1.0f));
}

void Slider::onScroll(float delta) {
  if (hovered) {
    float sensitivity = 0.05f;
    value += delta * sensitivity;
    value = glm::clamp(value, 0.0f, 1.0f);
  }
}