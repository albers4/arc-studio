#include "TextInput.h"
#include "UIManager.h"
#include "Theme.h"
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
        cursorVisible = true;
        lastBlinkTime = glfwGetTime();
    } else {
        commitValue();
        clearSelection();
        isSelecting = false;
    }
}

void TextInput::update(float mouseX, float mouseY, bool mousePressed) {
    Widget::update(mouseX, mouseY, mousePressed);

    bool justPressed = mousePressed && !wasPressed;
    wasPressed = mousePressed;

    double currentTime = glfwGetTime();
    if (isFocused) {
        if (currentTime - lastBlinkTime > 0.5) {
            cursorVisible = !cursorVisible;
            lastBlinkTime = currentTime;
        }

        int newPos = cursorPos;
        //if ((hovered || isSelecting) && (mousePressed || justPressed)) {
        if (hovered || isSelecting) {
            float padding = 6.0f;
            float localMouseX = mouseX - position.x - padding + scrollOffset;
            float currentX = 0;
            newPos = 0;

            for (char c : editBuffer) {
                if (c < 32 || c > 126) continue;
                float charW = font.charData.data[c - 32].xadvance;
                if (localMouseX > currentX + charW / 2.0f) {
                    newPos++;
                    currentX += charW;
                } else {
                    break;
                }
            }
        }

        if (justPressed && hovered) {
            // start of a new click/drag
            cursorPos = newPos;
            selectionStart = newPos;
            isSelecting = true;
        } else if (isSelecting && mousePressed) {
            // continuing a drag
            cursorPos = newPos;
        } else if (!mousePressed && isSelecting) {
            // drag finished
            isSelecting = false;
            if (selectionStart == cursorPos) {
                selectionStart = -1;
            }
        }

        if (!mousePressed && !isSelecting && selectionStart == cursorPos) {
            selectionStart = -1;
        }

    } else {
        cursorVisible = false;
        isSelecting = false;
    }
}

void TextInput::onChar(unsigned int codepoint) {
    if (!isFocused) return;
    if (codepoint >= 32 && codepoint <= 126) {
        deleteSelection();
        editBuffer.insert(cursorPos, 1, static_cast<char>(codepoint));
        cursorPos++;
    }
}

void TextInput::onKey(int key, int action, int mods, UIManager& ui) {
    if (!isFocused || (action != GLFW_PRESS && action != GLFW_REPEAT)) return;

    bool ctrl = (mods & GLFW_MOD_CONTROL) || (mods & GLFW_MOD_SUPER);
    bool shift = (mods & GLFW_MOD_SHIFT);

    // --- shortcuts ---
    if (ctrl) {
        if (key == GLFW_KEY_A) { selectAll(); return; }
        if (key == GLFW_KEY_C) { ui.setClipboard(getSelectedText()); return; }
        if (key == GLFW_KEY_X) { ui.setClipboard(getSelectedText()); deleteSelection(); return; }
        if (key == GLFW_KEY_V) {
            std::string clip = ui.getClipboard();
            deleteSelection();
            editBuffer.insert(cursorPos, clip);
            cursorPos += clip.length();
            return;
        }
    }

    // --- navigation ---
    if (key == GLFW_KEY_LEFT) {
        if (cursorPos > 0) cursorPos--;
        if (!shift) clearSelection();
        else if (selectionStart == -1) selectionStart = cursorPos + 1;
        return;
    }
    if (key == GLFW_KEY_RIGHT) {
        if (cursorPos < (int)editBuffer.length()) cursorPos++;
        if (!shift) clearSelection();
        else if (selectionStart == -1) selectionStart = cursorPos - 1;
        return;
    }
    if (key == GLFW_KEY_HOME) {
        if (shift && selectionStart == -1) selectionStart = cursorPos;
        cursorPos = 0; 
        if (!shift) clearSelection();
        return;
    }
    if (key == GLFW_KEY_END) {
        if (shift && selectionStart == -1) selectionStart = cursorPos;
        cursorPos = editBuffer.length(); 
        if (!shift) clearSelection();
        return;
    }

    // --- deletion ---
    if (key == GLFW_KEY_BACKSPACE) {
        if (selectionStart != -1 && selectionStart != cursorPos) {
            deleteSelection();
        } else if (ctrl) {
            editBuffer.erase(0, cursorPos);
            cursorPos = 0;
        } else if (cursorPos > 0) {
            editBuffer.erase(cursorPos - 1, 1) ;
            cursorPos--;
        }
        return;
    }
    if (key == GLFW_KEY_DELETE) {
        if (selectionStart != -1 && selectionStart != cursorPos) {
            deleteSelection();
        } else if (ctrl) {
            editBuffer.erase(cursorPos);
        } else if (cursorPos > 0) {
            editBuffer.erase(cursorPos, 1) ;
        }
        return;
    }

    // --- commit/cancel ---
    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        commitValue();
        isFocused = false;
        ui.clearFocus();
    } else if (key == GLFW_KEY_ESCAPE) {
        editBuffer = originalBuffer;
        isFocused = false;
        ui.clearFocus();
    }
}

std::string TextInput::getSelectedText() {
    if (selectionStart == -1 || selectionStart == cursorPos) return "";
    int start = std::min(selectionStart, cursorPos);
    int end = std::max(selectionStart, cursorPos);
    return editBuffer.substr(start, end - start);
}

void TextInput::deleteSelection() {
    if (selectionStart == -1 || selectionStart == cursorPos) return;
    int start = std::min(selectionStart, cursorPos);
    int end = std::max(selectionStart, cursorPos);
    editBuffer.erase(start, end - start);
    cursorPos = start;
    selectionStart = -1;
}

void TextInput::draw(UIManager& ui) {
    const auto& theme = Theme::get();
    const auto& m = theme.metrics;
    const auto& c = theme.colors;

    // background
    glm::vec4 bgColor = isFocused ? c.surface : c.background;
    ui.rect(position.x, position.y, size.x, size.y, bgColor);

    // border
    glm::vec4 borderColor = isFocused ? c.accent : c.border;
    ui.rect(position.x, position.y, size.x, m.borderWidth, borderColor); // top
    ui.rect(position.x, position.y + size.y - m.borderWidth, size.x, m.borderWidth, borderColor); // bottom
    ui.rect(position.x, position.y, m.borderWidth, size.y, borderColor); // left
    ui.rect(position.x + size.x - m.borderWidth, position.y, m.borderWidth, size.y, borderColor); // right

    float padding = m.padding;
    float textY = position.y + (size.y / 2.0f) + font.baselineOffset;
    float visibleWidth = size.x - (padding * 2);

    float cursorLocalX = 0;
    for (int i = 0; i < cursorPos; i++) {
        if (editBuffer[i] >= 32 && editBuffer[i] <= 126)
            cursorLocalX += font.charData.data[editBuffer[i] - 32].xadvance;
    }

    if (cursorLocalX - scrollOffset > visible) {
        scrollOffset = cursorLocalX - visibleWidth;
    }
    if (cursorLocalX - scrollOffset < 0) {
        scrollOffset = cursorLocalX;
    }

    float totalTextWidth = 0;
    for (char c : editBuffer) if (c >= 32 && c <= 126) totalTextWidth += font.charData.data[c - 32].xadvance;
    float maxScroll = std::max(0.0f, totalTextWidth - visibleWidth);
    scrollOffset = std::clamp(scrollOffset, 0.0f, maxScroll);

    if (selectionStart != -1 && selectionStart != cursorPos) {
        int startIndex = std::min(selectionStart, cursorPos);
        int endIndex = std::max(selectionStart, cursorPos);

        float startX = padding - scrollOffset;
        for (int i = 0; i < startIndex; i++) startX += font.charData.data[editBuffer[i] - 32].xadvance;

        float endX = startX;
        for (int i = startIndex; i < endIndex; i++) endX += font.charData.data[editBuffer[i] - 32].xadvance;

        float screenStartX = position.x + startX;
        float screenEndX = position.x + endX;

        float visibleLeft = position.x + padding;
        float visibleRight = position.x + size.x - padding;

        screenStartX = std::max(screenStartX, visibleLeft);
        screenEndX = std::min(screenEndX, visibleRight);

        if (screenEndX > screenStartX) {
            ui.rect(screenStartX, position.y + 4.0f, screenEndX - screenStartX, size.y - 8.0f, glm::vec4(0.3f, 0.5f, 0.8f, 0.8f));
        }
    }

    // text drawing
    float currentX = padding - scrollOffset;
    for (char c : editBuffer) {
        if (c < 32 || c > 126) continue;
        float charW = font.charData.data[c - 32].xadvance;

        if (currentX + charW > padding && currentX < size.x - padding) {
            std::string s(1, c);
            ui.drawString(position.x + currentX, textY, s, font, glm::vec4(1.0f));
        }
        currentX += charW;
    }

    // cursor
    if (isFocused && cursorVisible) {
        float cursorScreenX = position.x + padding + (cursorLocalX - scrollOffset);
        cursorScreenX = std::clamp(cursorScreenX, position.x + padding, position.x + size.x - padding);
        ui.rect(cursorScreenX, position.y + 4.0f, 1.0f, size.y - 8.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

}