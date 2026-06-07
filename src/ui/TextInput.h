#pragma once
#include "FontAtlas.h"
#include "Widget.h"
#include <functional>
#include <string>

class TextInput : public Widget {
private:
  const FontAtlas &font;
  std::string editBuffer;
  std::string originalBuffer;

  int cursorPos = 0;
  int selectionStart = -1;
  float scrollOffset = 0.0f;
  double lastBlinkTime = 0.0;
  bool cursorVisible = true;

  bool isSelecting = false;
  bool wasPressed = false;

  enum class Type { String, Int, Float } valueType = Type::String;
  void *valuePtr = nullptr;
  double minValue = 0.0, maxValue = 100.0;

  void commitValue();
  void loadValueToBuffer();

  void selectAll() {
    selectionStart = 0;
    cursorPos = editBuffer.length();
  }
  void clearSelection() { selectionStart = -1; }
  std::string getSelectedText();
  void deleteSelection();

public:
  TextInput(float x, float y, float w, float h, const std::string &label,
            const FontAtlas &font);

  void bind(std::string &value);
  void bind(int &value, int min, int max);
  void bind(float &value, float min, float max);

  void setFocused(bool focused) override;
  void update(float mouseX, float mouseY, bool mousePressed,
              UIManager &ui) override;
  void draw(UIManager &ui) override;

  void onKey(int key, int action, int mods, UIManager &ui) override;
  void onChar(unsigned int codepoint) override;
};