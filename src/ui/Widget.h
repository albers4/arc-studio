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
    bool isFocused = false;

    std::function<void()> onClick;

    Widget(float x, float y, float w, float h, const std::string& label);
    virtual ~Widget() = default;

    virtual void update(float mouseX, float mouseY, bool mousePressed);
    virtual void draw(UIManager& ui) = 0; // pure virtual
    virtual void setFocused(bool focused);

    virtual void onScroll(float delta);
    virtual void onKey(int key, int action, int mods, UIManager& ui) {}
    virtual void onChar(unsigned int codepoint) {}

    bool contains(float mx, float my) const;
};