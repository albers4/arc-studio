#include "Split.h"
#include "Theme.h"
#include "UIManager.h"
#include <algorithm>

Split::Split(float x, float y, float w, float h, Direction dir, float ratio)
    : Widget(x, y, w, h, "Split"), direction(dir),
      ratio(std::clamp(ratio, 0.1f, 0.9f)) {}

void Split::update(float mouseX, float mouseY, bool mousePressed,
                   UIManager &ui) {
  Widget::update(mouseX, mouseY, mousePressed, ui);

  bool justPressed = mousePressed && !wasPressed;
  wasPressed = mousePressed;

  float splitterSize = 6.0f;
  float splitPos = (direction == Direction::Vertical)
                       ? position.y + (size.y * ratio)
                       : position.x + (size.x * ratio);

  float handleStart =
      (direction == Direction::Vertical) ? position.x : position.y;
  float handleEnd = (direction == Direction::Vertical) ? position.x + size.x
                                                       : position.y + size.y;

  bool mouseOverSplitter = (direction == Direction::Vertical)
                               ? (mouseY >= splitPos - splitterSize / 2 &&
                                  mouseY <= splitPos + splitterSize / 2 &&
                                  mouseX >= handleStart && mouseX <= handleEnd)
                               : (mouseX >= splitPos - splitterSize / 2 &&
                                  mouseX <= splitPos + splitterSize / 2 &&
                                  mouseY >= handleStart && mouseY <= handleEnd);

  if (mouseOverSplitter) {
    ui.desiredCursor = (direction == Direction::Vertical)
                           ? CursorType::ResizeNS
                           : CursorType::ResizeEW;
  }

  if (justPressed && mouseOverSplitter) {
    isDragging = true;
  }

  if (!mousePressed) {
    isDragging = false;
  }

  if (isDragging) {
    if (direction == Direction::Vertical) {
      ratio = std::clamp((mouseY - position.y) / size.y, 0.1f, 0.9f);
    } else {
      ratio = std::clamp((mouseX - position.x) / size.x, 0.1f, 0.9f);
    }
  }

  if (children.size() >= 2) {
    if (direction == Direction::Vertical) {
      children[0]->position = {position.x, position.y};
      children[0]->size = {size.x, size.y * ratio};

      children[1]->position = {position.x, position.y + (size.y * ratio)};
      children[1]->size = {size.x, size.y * (1.0f - ratio)};
    } else {
      children[0]->position = {position.x, position.y};
      children[0]->size = {size.x * ratio, size.y};

      children[1]->position = {position.x + (size.x * ratio), position.y};
      children[1]->size = {size.x * (1.0f - ratio), size.y};
    }
  }

  for (auto &child : children) {
    child->update(mouseX, mouseY, mousePressed, ui);
  }
}

void Split::draw(UIManager &ui) {
  const auto &theme = Theme::get();
  const auto &c = theme.colors;
  const auto &m = theme.metrics;

  for (auto &child : children) {
    child->draw(ui);
  }

  if (children.size() >= 2) {
    float splitterSize = 6.0f;
    float splitPos = (direction == Direction::Vertical)
                         ? position.y + (size.y * ratio)
                         : position.x + (size.x * ratio);

    glm::vec4 handleColor = isDragging ? c.accent : c.border;
    if (!isDragging && hovered) {
    }

    if (direction == Direction::Vertical) {
      ui.rect(position.x, splitPos - splitterSize / 2, size.x, splitterSize,
              handleColor);
    } else {
      ui.rect(splitPos - splitterSize / 2, position.y, splitterSize, size.y,
              handleColor);
    }
  }
}

void Split::cascadeScroll(float delta) {
  onScroll(delta);
  for (auto &child : children) {
    child->cascadeScroll(delta);
  }
}