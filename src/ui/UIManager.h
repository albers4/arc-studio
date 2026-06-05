#pragma once
#include "FontAtlas.h"
#include "../renderer/BatchRenderer.h"
#include "Widget.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class UIManager {
private:
    BatchRenderer& renderer;
    std::vector<std::unique_ptr<Widget>> widgets;
    float mouseX = 0, mouseY = 0;
    bool mousePressed = false;
    Widget* focusedWidget = nullptr;
    
public:
    UIManager(BatchRenderer& ren);

    std::function<std::string()> getClipboard;
    std::function<void(const std::string&)> setClipboard;

    void updateMouse(float x, float y, bool pressed, bool justPressed);
    void updateScroll(float delta);
    void onKey(int key, int action, int mods);
    void onChar(unsigned int codepoint);

    void clearFocus();

    template<typename T, typename... Args>
    T& addWidget(Args&&... args) {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = widget.get();

        widgets.push_back(
            std::unique_ptr<Widget>(static_cast<Widget*>(widget.release()))
        );

        return *ptr;
    }
    
    void drawString(float x, float y, const std::string& text, const FontAtlas& font, glm::vec4 color = glm::vec4(1.0f));
    void rect(float x, float y, float w, float h, glm::vec4 col);
    void flush(const glm::mat4& proj, const FontAtlas& font);
};