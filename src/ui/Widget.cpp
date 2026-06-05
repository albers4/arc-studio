#include "Widget.h"
#include "UIManager.h"

Widget::Widget(float x, float y, float w, float h, const std::string& label)
    : position(x, y), size(w, h), label(label) {}

void Widget::update(float mouseX, float mouseY, bool mousePressed) {
    hovered = contains(mouseX, mouseY);

    if (hovered && mousePressed && onClick) {
        onClick();
    }
}

bool Widget::contains(float mx, float my) const {
    return mx >= position.x && mx <= position.x + size.x && my >= position.y && my <= position.y + size.y;
}