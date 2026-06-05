#pragma once
#include "Widget.h"
#include "FontAtlas.h"
#include <glm/glm.hpp>
#include <functional>

class Slider : public Widget {
private:
    const FontAtlas& font;
    float value;
    float minValue;
    float maxValue;
    bool isDragging;
    bool wasPressed = false;

    glm::vec4 trackColor;
    glm::vec4 knobColor;
    glm::vec4 hoverColor;

public:
    Slider(float x, float y, float w, float h, const std::string& label, const FontAtlas& font, float min = 0.0f, float max = 1.0f, float startValue = 0.5f);

    void update(float mouseX, float mouseY, bool mousePressed) override;
    void draw(UIManager& ui) override;
    void onScroll(float delta) override;

    float getValue() const { return value; }
    void setValue(float value) { value = glm::clamp(value, 0.0f, 1.0f); }
};