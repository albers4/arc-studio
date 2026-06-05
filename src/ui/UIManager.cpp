#include "UIManager.h"

UIManager::UIManager(BatchRenderer& ren) : renderer(ren), focusedWidget(nullptr) {}

void UIManager::updateMouse(float x, float y, bool pressed, bool justPressed) {
    mouseX = x;
    mouseY = y;
    mousePressed = pressed;

    if (justPressed) {
        focusedWidget = nullptr;

        for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
            if ((*it)->contains(mouseX, mouseY)) {
                focusedWidget = it->get();
                break;
            }
        }
    }

    for (auto& widget : widgets) {
        widget->setFocused(widget.get() == focusedWidget);
        widget->update(mouseX, mouseY, mousePressed);
    }
}

void UIManager::onKey(int key, int action) {
    if (focusedWidget) {
        focusedWidget->onKey(key, action, *this);
    }
}

void UIManager::onChar(unsigned int codepoint) {
    if (focusedWidget) {
        focusedWidget->onChar(codepoint);
    }
}

void UIManager::clearFocus() {
    focusedWidget = nullptr;
    for (auto& widget : widgets) {
        widget->setFocused(false);
    }
}

void UIManager::updateScroll(float delta) {
    for (auto& widget : widgets) {
        if (widget->hovered) {
            widget->onScroll(delta);
        }
    }
}

void UIManager::rect(float x, float y, float w, float h, glm::vec4 col) {
    renderer.rect(x, y, w, h, col);
}

void UIManager::drawString(float x, float y, const std::string& text, const FontAtlas& font, glm::vec4 color) {
    float cx = x;
    for (char c : text) {
        if (c < 32 || c > 126) continue;
        const stbtt_bakedchar& ch = font.charData.data[c - 32];
        float w = ch.x1 - ch.x0;
        float h = ch.y1 - ch.y0;
        if (w <= 0 || h <= 0) continue;

        float u0 = (float)ch.x0 / (float)font.atlasW;
        float v0 = (float)ch.y0 / (float)font.atlasH;
        float u1 = (float)ch.x1 / (float)font.atlasW;
        float v1 = (float)ch.y1 / (float)font.atlasH;

        renderer.textQuad(cx + ch.xoff, y + ch.yoff, w, h, u0, v0, u1, v1, color);
        cx += ch.xadvance;
    }
}

void UIManager::flush(const glm::mat4& proj, const FontAtlas& font) {
    for (auto& widget : widgets) {
        if (widget->visible) {
            widget->draw(*this);
        }
    }

    renderer.flush(proj, font.textureID);
}