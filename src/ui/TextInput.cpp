#include "TextInput.h"
#include "UIManager.h"
#include <algorithm>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <sstream>
#include <locale>
#include <iostream>

TextInput::TextInput(float x, float y, float w, float h, const std::string& label, const FontAtlas& font)
    : Widget(x, y, w, h, label), font(font) {}

void TextInput::bind(std::string& value) {
    valueType = Type::String;
    valuePtr = &value;
    loadValueToBuffer();
}

void TextInput::bind(int& value, int min, int max) {
    valueType = Type::Int;
    valuePtr = &value;
    minValue = min;
    maxValue = max;
    loadValueToBuffer();
}

void TextInput::bind(float& value, float min, float max) {
    valueType = Type::Float;
    valuePtr = &value;
    minValue = min;
    maxValue = max;
    loadValueToBuffer();
}

void TextInput::loadValueToBuffer() {
    try {
        if (valueType == Type::String) {
            editBuffer = *static_cast<std::string*>(valuePtr);
        } else if (valueType == Type::Int) {
            std::stringstream ss;
            ss.imbue(std::locale::classic());
            ss << *static_cast<int*>(valuePtr);
            editBuffer = ss.str();
        } else if (valueType == Type::Float) {
            std::stringstream ss;
            ss.imbue(std::locale::classic());
            ss << *static_cast<float*>(valuePtr);
            editBuffer = ss.str();
        }
        originalBuffer = editBuffer;
    } catch (...) {
        editBuffer = "Error";
        originalBuffer = editBuffer;
    }
} 

void TextInput::commitValue() {
    try {
        if (valueType == Type::String) {
            *static_cast<std::string*>(valuePtr) = editBuffer;
        } else if (valueType == Type::Int) {
            std::stringstream ss(editBuffer);
            ss.imbue(std::locale::classic());
            int value;
            if (!(ss >> value)) throw std::invalid_argument("Invalid int");

            value = std::clamp(value, static_cast<int>(minValue), static_cast<int>(maxValue));
            *static_cast<int*>(valuePtr) = value;

            std::stringstream out;
            out.imbue(std::locale::classic());
            out << value;
            editBuffer = out.str();
        } else if (valueType == Type::Float) {
            std::stringstream ss(editBuffer);
            ss.imbue(std::locale::classic());
            float value;
            if (!(ss >> value)) throw std::invalid_argument("Invalid float");

            value = std::clamp(value, static_cast<float>(minValue), static_cast<float>(maxValue));
            *static_cast<float*>(valuePtr) = value;

            std::stringstream out;
            out.imbue(std::locale::classic());
            out << value;
            editBuffer = out.str();
        }
        originalBuffer = editBuffer;
    } catch (...) {
        editBuffer = originalBuffer;
    }
}

void TextInput::setFocused(bool focused) {
    if (isFocused == focused) return;

    isFocused = focused;
    if (isFocused) {
        loadValueToBuffer();
        cursorTimer = 0.0f;
        cursorVisible = true;
    } else {
        commitValue();
    }
}

void TextInput::update(float mouseX, float mouseY, bool mousePressed) {
    Widget::update(mouseX, mouseY, mousePressed);

    if (isFocused) {
        cursorTimer += 0.016f;
        if (cursorTimer > 1.5f) {
            cursorVisible = !cursorVisible;
            cursorTimer = 0.0f;
        }
    } else {
        cursorVisible = false;
    }
}

void TextInput::onChar(unsigned int codepoint) {
    if (!isFocused) return;
    if (codepoint >= 32 && codepoint <= 126) {
        editBuffer += static_cast<char>(codepoint);
    }
}

void TextInput::onKey(int key, int action, UIManager& ui) {
    if (!isFocused || (action != GLFW_PRESS && action != GLFW_REPEAT)) return;

    if (key == GLFW_KEY_BACKSPACE && !editBuffer.empty()) {
        editBuffer.pop_back();
    } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        commitValue();
        isFocused = false;
        ui.clearFocus();
    } else if (key == GLFW_KEY_ESCAPE) {
        editBuffer = originalBuffer;
        isFocused = false;
        ui.clearFocus();
    }
}

void TextInput::draw(UIManager& ui) {
    // background
    glm::vec4 bgColor = isFocused ? glm::vec4(0.2f, 0.2f, 0.25f, 1.0f) : glm::vec4(0.15f, 0.15f, 0.18f, 1.0f);
    ui.rect(position.x, position.y, size.x, size.y, bgColor);

    // border
    glm::vec4 borderColor = isFocused ? glm::vec4(0.5f, 0.7f, 0.9f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.35f, 1.0f);
    ui.rect(position.x, position.y, size.x, 2.0f, borderColor); // top
    ui.rect(position.x, position.y + size.y - 2.0f, size.x, 2.0f, borderColor); // bottom
    ui.rect(position.x, position.y, 2.0f, size.y, borderColor); // left
    ui.rect(position.x + size.x - 2.0f, position.y, 2.0f, size.y, borderColor); // right

    float textY = position.y + (size.y / 2.0f) + font.baselineOffset;
    ui.drawString(position.x + 6.0f, textY, editBuffer, font, glm::vec4(1.0f));

    if (isFocused && cursorVisible) {
        float cursorX = position.x + 6.0f;
        for (char c : editBuffer) {
            if (c >= 32 && c <= 126) {
                cursorX += font.charData.data[c - 32].xadvance;
            }
        }
        ui.rect(cursorX, position.y + 4.0f, 1.0f, size.y - 8.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

}