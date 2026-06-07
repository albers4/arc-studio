#include "Area.h"
#include "Theme.h"
#include "UIManager.h"
#include <algorithm>
#include <iostream>

Area::Area(float x, float y, float w, float h, Type type,
           const std::string &title, const FontAtlas &font)
    : Widget(x, y, w, h, title), areaType(type), font(font) {}

void Area::applyOffset() {
  float offX = position.x;
  float offY = position.y + getHeaderHeight() - scrollY;
  for (auto &child : children) {
    child->position.x += offX;
    child->position.y += offY;
  }
}

void Area::revertOffset() {
  float offX = position.x;
  float offY = position.y + getHeaderHeight() - scrollY;
  for (auto &child : children) {
    child->position.x -= offX;
    child->position.y -= offY;
  }
}

void Area::update(float mouseX, float mouseY, bool mousePressed,
                  UIManager &ui) {
  Widget::update(mouseX, mouseY, mousePressed, ui);
  calculateContentHeight();

  bool justPressed = mousePressed && !wasPressed;
  wasPressed = mousePressed;

  float headerH = getHeaderHeight();
  float visibleH = getVisibleContentHeight();
  float contentY = position.y + headerH;

  float scrollbarX = position.x + size.x - scrollbarWidth;
  bool mouseOverScrollbar =
      (mouseX >= scrollbarX && mouseX <= position.x + size.x &&
       mouseY >= contentY && mouseY <= contentY + visibleH);

  if (totalContentHeight > visibleH) {
    if (justPressed && mouseOverScrollbar) {
      isDraggingScollbar = true;
    }
    if (!mousePressed) {
      isDraggingScollbar = false;
    }

    if (isDraggingScollbar) {
      float maxScroll = totalContentHeight - visibleH;
      float handleH =
          std::max(20.f, visibleH * (visibleH / totalContentHeight));
      float scrollableTrackH = visibleH - handleH;

      float relativeMouseY = mouseY - contentY;
      float ratio = std::clamp(
          (relativeMouseY - handleH / 2.0f) / scrollableTrackH, 0.0f, 1.0f);
      scrollY = ratio * maxScroll;
    }
  } else {
    scrollY = 0.0f;
    isDraggingScollbar = false;
  }

  bool mouseInContent =
      (mouseX >= position.x && mouseX <= position.x + size.x - scrollbarWidth &&
       mouseY >= contentY && mouseY <= contentY + visibleH);

  float effMouseX = mouseInContent ? mouseX : -10000.0f;
  float effMouseY = mouseInContent ? mouseY : -10000.0f;

  applyOffset();
  for (auto &child : children) {
    child->update(effMouseX, effMouseY, mousePressed, ui);
  }
  revertOffset();
}

void Area::draw(UIManager &ui) {
  const auto &theme = Theme::get();
  const auto &m = theme.metrics;
  const auto &c = theme.colors;

  float headerHeight = getHeaderHeight();
  float visibleH = getVisibleContentHeight();
  float contentY = position.y + headerHeight;

  // header + background
  glm::vec4 headerColor = hovered ? c.surfaceHover : c.surface;
  ui.rect(position.x, position.y, size.x, headerHeight, headerColor);
  ui.rect(position.x, position.y + headerHeight - 1.0f, size.x, 1.0f, c.border);

  float textY =
      std::round(position.y + (headerHeight / 2.0f) + font.baselineOffset);
  ui.drawString(position.x + m.padding, textY, label, font, c.text);

  ui.rect(position.x, contentY, size.x, visibleH, c.background);

  // clipping
  ui.beginClip(position.x, contentY, size.x, visibleH);

  applyOffset();
  for (auto &child : children) {
    child->draw(ui);
  }
  revertOffset();

  // scrollbar
  if (totalContentHeight > visibleH) {
    float scrollbarX = position.x + size.x - scrollbarWidth;
    float maxScroll = totalContentHeight - visibleH;
    float handleH = std::max(20.0f, visibleH * (visibleH / totalContentHeight));
    float scrollableTrackH = visibleH - handleH;
    float handleY = contentY + (scrollY / maxScroll) * scrollableTrackH;

    ui.rect(scrollbarX, contentY, scrollbarWidth, visibleH, c.background);
    glm::vec4 handleColor = isDraggingScollbar ? c.accent : c.surfaceHover;
    ui.rect(scrollbarX + 2.0f, handleY + 2.0f, scrollbarWidth - 4.0f,
            handleH - 4.0f, handleColor);
  }

  ui.endClip();
}

void Area::calculateContentHeight() {
  totalContentHeight = 0.0f;
  for (auto &child : children) {
    float bottom = child->position.y + child->size.y;
    if (bottom > totalContentHeight)
      totalContentHeight = bottom;
  }
}

float Area::getVisibleContentHeight() const {
  return std::max(0.0f, size.y - getHeaderHeight());
}

void Area::cascadeScroll(float delta) {
  onScroll(delta);
  for (auto &child : children) {
    child->cascadeScroll(delta);
  }
}

void Area::onScroll(float delta) {
  bool childHovered = false;
  for (auto &child : children) {
    if (child->hovered) {
      childHovered = true;
      break;
    }
  }

  if (hovered && !childHovered) {
    float visibleH = getVisibleContentHeight();
    if (totalContentHeight > visibleH) {
      scrollY -= delta * 20.0f;
      float maxScroll = totalContentHeight - visibleH;
      scrollY = std::clamp(scrollY, 0.0f, maxScroll);
    }
  }
}