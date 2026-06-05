#include "Application.h"
#include "../renderer/BatchRenderer.h"
#include "../renderer/Shader.h"
#include "../ui/Button.h"
#include "../ui/FontAtlas.h"
#include "../ui/Slider.h"
#include "../ui/TextInput.h"
#include "../ui/Theme.h"
#include "../ui/UIManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Application::Application(const std::string &title, int w, int h)
    : width(w), height(h) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    std::cerr << "Failed to create GLFW window" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwMaximizeWindow(window);
  glfwMakeContextCurrent(window);
  glfwSetWindowSizeLimits(window, 1200, 800, GLFW_DONT_CARE, GLFW_DONT_CARE);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwSetWindowUserPointer(window, this);
  glfwSetScrollCallback(
      window, [](GLFWwindow *w, double xoffset, double yoffset) {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(w));
        if (app) {
          app->onScroll(xoffset, yoffset);
        }
      });
  glfwSetKeyCallback(
      window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(w));
        if (app)
          app->onKey(key, scancode, action, mods);
      });
  glfwSetCharCallback(window, [](GLFWwindow *w, unsigned int codepoint) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(w));
    if (app)
      app->onChar(codepoint);
  });

  initOpenGL();
}

Application::~Application() {
  delete ui;
  delete renderer;
  delete font;
  delete uiShader;

  if (window) {
    glfwDestroyWindow(window);
  }
  glfwTerminate();
}

void Application::initOpenGL() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize Resources
  uiShader = new Shader("../shaders/ui.vert", "../shaders/ui.frag");
  font = new FontAtlas("../assets/fonts/arial.ttf", 16.0f);
  const auto &theme = Theme::get();

  if (font->textureID == 0) {
    std::cerr << "Failed to load font!" << std::endl;
    exit(EXIT_FAILURE);
  }

  renderer = new BatchRenderer(*uiShader);
  ui = new UIManager(*renderer);

  ui->getClipboard = [this]() {
    const char *clip = glfwGetClipboardString(window);
    return clip ? std::string(clip) : "";
  };
  ui->setClipboard = [this](const std::string &text) {
    glfwSetClipboardString(window, text.c_str());
  };

  auto &myButton = ui->addWidget<Button>(20, 10, 100, 30, "Render", *font);
  myButton.onClick = []() {
    std::cout << "Render button clicked" << std::endl;
  };

  auto &mySlider = ui->addWidget<Slider>(20, 60, 200, 30, "Brush Slider", *font,
                                         0.0f, 100.0f, 50.0f);
  mySlider.onClick = [&]() {
    std::cout << "Slider value: " << mySlider.getValue() << std::endl;
  };

  static std::string myName = "ArcStudio";
  auto &nameInput = ui->addWidget<TextInput>(20, 100, 200, 30, "Name", *font);
  nameInput.bind(myName);

  static int brushSize = 50;
  auto &sizeInput =
      ui->addWidget<TextInput>(20, 150, 200, 30, "Brush Size", *font);
  sizeInput.bind(brushSize, 1, 100);

  static float exposure = 1.0f;
  auto &expInput =
      ui->addWidget<TextInput>(20, 200, 200, 30, "Exposure", *font);
  expInput.bind(exposure, 0.0f, 1.0f);
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    update();
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Application::onScroll(double xoffset, double yoffset) {
  if (ui)
    ui->updateScroll(yoffset);
}

void Application::onKey(int key, int scancode, int action, int mods) {
  if (ui)
    ui->onKey(key, action, mods);
}

void Application::onChar(unsigned int codepoint) {
  if (ui)
    ui->onChar(codepoint);
}

void Application::update() {
  double mx, my;
  glfwGetCursorPos(window, &mx, &my);
  bool leftClick =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

  static bool wasPressed = false;
  bool justPressed = leftClick && !wasPressed;

  ui->updateMouse(mx, my, leftClick, justPressed);
  wasPressed = leftClick;
}

void Application::render() {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);
  glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);

  // --- UI Drawing Code ---
  // ui->rect(0, 0, w, 50, glm::vec4(0.3f, 0.3f, 0.35f, 1.0f)); // Header

  ui->flush(proj, *font);
}