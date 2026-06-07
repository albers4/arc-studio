#include "Theme.h"
#include <algorithm>
#include <stdexcept>

ThemeColors::ThemeColors() {
  background = Theme::hexToColor("#202020");
  surface = Theme::hexToColor("272727");
  surfaceHover = Theme::hexToColor("#202020");
  accent = Theme::hexToColor("FFD100");
  text = Theme::hexToColor("FFFFFF");
  border = Theme::hexToColor("454545");
  selection = Theme::hexToColor("FFD100");
}

glm::vec4 Theme::hexToColor(const std::string &hex, float alpha) {
  std::string cleanHex = hex;

  if (!cleanHex.empty() && cleanHex[0] == '#') {
    cleanHex = cleanHex.substr(1);
  }

  // 6 for RGB and 8 for RGBA
  if (cleanHex.length() != 6 && cleanHex.length() != 8) {
    // return visual indicator for invalid hex string
    return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
  }

  try {
    float r = std::stoul(cleanHex.substr(0, 2), nullptr, 16) / 255.0f;
    float g = std::stoul(cleanHex.substr(2, 2), nullptr, 16) / 255.0f;
    float b = std::stoul(cleanHex.substr(4, 2), nullptr, 16) / 255.0f;
    float a = alpha;

    if (cleanHex.length() == 8) {
      a = std::stoul(cleanHex.substr(6, 2), nullptr, 16) / 255.0f;
    }

    return glm::vec4(r, g, b, a);
  } catch (...) {
    return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
  }
}

glm::vec4 Theme::withAlpha(const glm::vec4 &color, float alpha) {
  return glm::vec4(color.r, color.g, color.b, alpha);
}