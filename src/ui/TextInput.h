#pragma once
#include "Widget.h"
#include "FontAtlas.h"
#include <string>
#include <functional>

class TextInput : public Widget {
private:
    const FontAtlas& font;
    std::string editBuffer;
    std::string originalBuffer;
    float cursorTimer = 0.0f;
    bool cursorVisible = true;

    enum class Type { String, Int, Float } valueType = Type::String;
    void* valuePtr = nullptr;
    double minValue = 0.0, maxValue = 100.0;

    void commitValue();
    void loadValueToBuffer();

public:
    TextInput(float x, float y, float w, float h, const std::string& label, const FontAtlas& font);

    void bind(std::string& value);
    void bind(int& value, int min, int max);
    void bind(float& value, float min, float max);

    void setFocused(bool focused) override;
    void update(float mouseX, float mouseY, bool mousePressed) override;
    void draw(UIManager& ui) override;

    void onKey(int key, int action, UIManager& ui) override;
    void onChar(unsigned int codepoint) override;
};