#pragma once
#include "FontAtlas.h"
#include "../renderer/BatchRenderer.h"
#include "Widget.h"
#include <string>
#include <vector>
#include <memory>

class UIManager {
private:
    BatchRenderer& renderer;
    std::vector<std::unique_ptr<Widget>> widgets;
    float mouseX = 0, mouseY = 0;
    bool mousePressed = false;
    
public:
    UIManager(BatchRenderer& ren);

    void updateMouse(float x, float y, bool pressed);

    template<typename T, typename... Args>
    T& addWidget(Args&&... args) {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = widget.get();
        widgets.push_back(std::move(widget));
        return *ptr;
    }
    
    void drawString(float x, float y, const std::string& text, const FontAtlas& font, glm::vec4 color = glm::vec4(1.0f));
    void rect(float x, float y, float w, float h, glm::vec4 col);
    void flush(const glm::mat4& proj, const FontAtlas& font);
};