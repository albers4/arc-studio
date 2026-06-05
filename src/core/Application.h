#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>
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

private:
  GLFWwindow *window = nullptr;
  int width, height;

  Shader *uiShader = nullptr;
  FontAtlas *font = nullptr;
  BatchRenderer *renderer = nullptr;
  UIManager *ui = nullptr;

  void initOpenGL();
  void update();
  void render();
};