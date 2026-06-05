#pragma once
#include <glm/glm.hpp>

struct ThemeColors {
  glm::vec4 background = glm::vec4(0.20f, 0.20f, 0.25f, 1.0f);
  glm::vec4 surface = glm::vec4(0.25f, 0.25f, 0.30f, 1.0f);
  glm::vec4 surfaceHover = glm::vec4(0.30f, 0.30f, 0.35f, 1.0f);
  glm::vec4 accent = glm::vec4(0.35f, 0.60f, 0.90f, 1.0f);
  glm::vec4 text = glm::vec4(0.90f, 0.90f, 0.90f, 1.0f);
  glm::vec4 border = glm::vec4(0.15f, 0.15f, 0.20f, 1.0f);
  glm::vec4 selection = glm::vec4(0.30f, 0.50f, 0.80f, 0.80f);
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

private:
  Theme() = default;
};