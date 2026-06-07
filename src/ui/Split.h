#pragma once
#include "Widget.h"
#include <memory>
#include <vector>

class Split : public Widget {
public:
  enum class Direction { Horizontal, Vertical };

  Split(float x, float y, float w, float h, Direction dir, float ratio = 0.5f);

  template <typename T, typename... Args> T &addChild(Args &&...args) {
    auto child = std::make_unique<T>(std::forward<Args>(args)...);
    T *ptr = child.get();

    children.push_back(
        std::unique_ptr<Widget>(static_cast<Widget *>(child.release())));

    return *ptr;
  }

  void cascadeScroll(float delta) override;

  void update(float mouseX, float mouseY, bool mousePressed,
              UIManager &ui) override;
  void draw(UIManager &ui) override;

private:
  Direction direction;
  float ratio;
  bool isDragging = false;
  bool wasPressed = false;
  std::vector<std::unique_ptr<Widget>> children;
};