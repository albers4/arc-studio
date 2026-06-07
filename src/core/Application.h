#pragma once
#include <glad/glad.h>

#include "../ui/Split.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

class UIManager;
class FontAtlas;
class Shader;
class BatchRenderer;

class Application {
public:
  Application(const std::string &title = "ArcStudio", int width = 1200,
              int height = 800);
  ~Application();

  // Prevent copying
  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void run();
  void onScroll(double xoffset, double yoffset);
  void onKey(int key, int scancode, int action, int mods);
  void onChar(unsigned int codepoint);
  void onFramebufferResize(int width, int height);

private:
  GLFWwindow *window = nullptr;
  int width, height;

  GLFWcursor *cursorArrow = nullptr;
  GLFWcursor *cursorResizeNS = nullptr;
  GLFWcursor *cursorResizeEW = nullptr;
  GLFWcursor *cursorIBeam = nullptr;
  GLFWcursor *cursorHand = nullptr;

  float mouseX = 0.0f;
  float mouseY = 0.0f;
  bool mousePressed = false;
  bool mouseJustPressed = false;

  Shader *uiShader = nullptr;
  FontAtlas *font = nullptr;
  BatchRenderer *renderer = nullptr;
  UIManager *ui = nullptr;
  std::unique_ptr<Split> rootSplit = nullptr;

  void initOpenGL();
  void update();
  void render();
};