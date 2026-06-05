#pragma once
#include <glm/glm.hpp>
#include <string>
#include <functional>

class UIManager;

class Widget {
public:
    glm::vec2 position;
    glm::vec2 size;
    std::string label;
    bool visible = true;
    bool hovered = false;

    std::function<void()> onClick;

    Widget(float x, float y, float w, float h, const std::string& label);
    virtual ~Widget() = default;

    virtual void update(float mouseX, float mouseY, bool mousePressed);
    virtual void draw(UIManager& ui) = 0; // pure virtual

    bool contains(float mx, float my) const;
};