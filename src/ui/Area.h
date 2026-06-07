#pragma once
#include "FontAtlas.h"
#include "Widget.h"
#include <functional>
#include <memory>
#include <string>

class Area : public Widget {
public:
  enum class Type { Viewport3D, Outliner, Graph, Properties, Console, Timeline };

  Type areaType;
  const FontAtlas &font;

  Area(float x, float y, float w, float h, Type type, const std::string &title,
       const FontAtlas &font);

  template <typename T, typename... Args> T &addChild(Args &&...args) {
    auto child = std::make_unique<T>(std::forward<Args>(args)...);
    T *ptr = child.get();

    children.push_back(
        std::unique_ptr<Widget>(static_cast<Widget *>(child.release())));

    return *ptr;
  }

  void update(float mouseX, float mouseY, bool mousePressed,
              UIManager &ui) override;
  void draw(UIManager &ui) override;

private:
  std::vector<std::unique_ptr<Widget>> children;

  void applyOffset();
  void revertOffset();
  float getHeaderHeight() const { return 25.0f; }
};