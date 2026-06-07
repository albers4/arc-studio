#include "Area.h"
#include "Theme.h"
#include "UIManager.h"
#include <iostream>

Area::Area(float x, float y, float w, float h, Type type,
           const std::string &title, const FontAtlas &font)
    : Widget(x, y, w, h, title), areaType(type), font(font) {}

void Area::applyOffset() {
  float offX = position.x;
  float offY = position.y + getHeaderHeight();
  for (auto &child : children) {
    child->position.x += offX;
    child->position.y += offY;
  }
}

void Area::revertOffset() {
  float offX = position.x;
  float offY = position.y + getHeaderHeight();
  for (auto &child : children) {
    child->position.x -= offX;
    child->position.y -= offY;
  }
}

void Area::update(float mouseX, float mouseY, bool mousePressed,
                  UIManager &ui) {
  Widget::update(mouseX, mouseY, mousePressed, ui);

  applyOffset();
  for (auto &child : children) {
    child->update(mouseX, mouseY, mousePressed, ui);
  }
  revertOffset();
}

void Area::draw(UIManager &ui) {
  const auto &theme = Theme::get();
  const auto &m = theme.metrics;
  const auto &c = theme.colors;

  float headerHeight = 25.0f;
  float contentY = position.y + headerHeight;
  float contentH = size.y - headerHeight;

  // header + background
  glm::vec4 headerColor = hovered ? c.surfaceHover : c.surface;
  ui.rect(position.x, position.y, size.x, headerHeight, headerColor);
  ui.rect(position.x, position.y + headerHeight - 1.0f, size.x, 1.0f, c.border);

  float textY = position.y + (headerHeight / 2.0f) + font.baselineOffset;
  ui.drawString(position.x + m.padding, textY, label, font, c.text);

  ui.rect(position.x, contentY, size.x, contentH, c.background);

  // clipping
  ui.beginClip(position.x, contentY, size.x, contentH);

  applyOffset();
  for (auto &child : children) {
    child->draw(ui);
  }
  revertOffset();

  ui.endClip();
}