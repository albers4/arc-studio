#pragma once
#include <glm/glm.hpp>
#include <string>

struct ThemeColors {
  glm::vec4 background;
  glm::vec4 surface;
  glm::vec4 surfaceHover;
  glm::vec4 accent;
  glm::vec4 text;
  glm::vec4 border;
  glm::vec4 selection;

  ThemeColors();
};

struct ThemeMetrics {
  float padding = 6.0f;
  float borderWidth = 2.0f;
  float cursorWidth = 1.0f;
};

class Theme {
public:
  static Theme &get() {
    static Theme instance;
    return instance;
  }

  ThemeColors colors;
  ThemeMetrics metrics;

  static glm::vec4 hexToColor(const std::string &hex, float alpha = 1.0f);
  static glm::vec4 withAlpha(const glm::vec4 &color, float alpha);

private:
  Theme() = default;
};