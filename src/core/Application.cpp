#include "Application.h"
#include "../renderer/BatchRenderer.h"
#include "../renderer/Shader.h"
#include "../ui/Area.h"
#include "../ui/Button.h"
#include "../ui/FontAtlas.h"
#include "../ui/Slider.h"
#include "../ui/Split.h"
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

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *w, int width, int height) {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(w));
        if (app) {
          app->onFramebufferResize(width, height);
        }
      });

  initOpenGL();
}

Application::~Application() {
  delete ui;
  delete renderer;
  delete font;
  delete uiShader;

  if (cursorArrow)
    glfwDestroyCursor(cursorArrow);
  if (cursorResizeNS)
    glfwDestroyCursor(cursorResizeNS);
  if (cursorResizeEW)
    glfwDestroyCursor(cursorResizeEW);
  if (cursorIBeam)
    glfwDestroyCursor(cursorIBeam);
  if (cursorHand)
    glfwDestroyCursor(cursorHand);

  if (window) {
    glfwDestroyWindow(window);
  }
  glfwTerminate();
}

void Application::initOpenGL() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  cursorArrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  cursorResizeNS = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  cursorResizeEW = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  cursorIBeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  cursorHand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

  float targetFontSize = 16;
  float atlasFontSize = targetFontSize * 2.0f;

  // Initialize Resources
  uiShader = new Shader("../shaders/ui.vert", "../shaders/ui.frag");
  // font = new FontAtlas("../assets/fonts/Inter/Inter-Regular.ttf",
  // atlasFontSize);
  font = new FontAtlas("../assets/fonts/geist-font/Geist/ttf/Geist-Regular.ttf",
                       targetFontSize, atlasFontSize);

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

  // --- dynamic layout setup ---

  rootSplit = std::make_unique<Split>(0, 0, 1200, 800,
                                      Split::Direction::Vertical, 0.75f);

  auto &viewport3d = rootSplit->addChild<Area>(
      0, 0, 0, 0, Area::Type::Viewport3D, "File", *font);
  auto &properties = rootSplit->addChild<Area>(
      0, 0, 0, 0, Area::Type::Properties, "Properties", *font);

  auto &myButton1 =
      viewport3d.addChild<Button>(20, 10, 100, 30, "Render", *font);
  auto &myButton2 =
      viewport3d.addChild<Button>(20, 200, 100, 30, "Render", *font);
  auto &myButton3 =
      viewport3d.addChild<Button>(20, 300, 100, 30, "Render", *font);
  auto &myButton4 =
      viewport3d.addChild<Button>(20, 400, 100, 30, "Render", *font);

  auto &mySlider = properties.addChild<Slider>(20, 60, 200, 30, "Brush Slider",
                                               *font, 0.0f, 100.0f, 50.0f);
  mySlider.onClick = [&]() {
    std::cout << "Slider value: " << mySlider.getValue() << std::endl;
  };

  static std::string myName = "ArcStudio";
  auto &nameInput =
      viewport3d.addChild<TextInput>(20, 110, 200, 30, "Name", *font);
  nameInput.bind(myName);

  /*
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
  auto &expInput = viewport.addChild<TextInput>(20, 20, 200, 30, "Exposure",
  *font); expInput.bind(exposure, 0.0f, 1.0f);
  */
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
  if (rootSplit) {
    rootSplit->cascadeScroll(static_cast<float>(yoffset));
  }
  if (ui) {
    ui->updateScroll(yoffset);
  }
}

void Application::onKey(int key, int scancode, int action, int mods) {
  if (ui)
    ui->onKey(key, action, mods);
}

void Application::onChar(unsigned int codepoint) {
  if (ui)
    ui->onChar(codepoint);
}

void Application::onFramebufferResize(int width, int height) {
  glViewport(0, 0, width, height);

  render();
  glfwSwapBuffers(window);
}

void Application::update() {
  double mx, my;
  glfwGetCursorPos(window, &mx, &my);
  bool leftClick =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

  mouseJustPressed = leftClick && !mousePressed;
  mousePressed = leftClick;
  mouseX = static_cast<float>(mx);
  mouseY = static_cast<float>(my);

  if (ui) {
    ui->updateMouse(mouseX, mouseY, mousePressed, mouseJustPressed);
  }

  if (rootSplit) {
    rootSplit->update(mouseX, mouseY, mousePressed, *ui);
  }

  GLFWcursor *targetCursor = cursorArrow;
  if (ui) {
    switch (ui->desiredCursor) {
    case CursorType::ResizeNS:
      targetCursor = cursorResizeNS;
      break;
    case CursorType::ResizeEW:
      targetCursor = cursorResizeEW;
      break;
    case CursorType::IBeam:
      targetCursor = cursorIBeam;
      break;
    case CursorType::Hand:
      targetCursor = cursorHand;
      break;
    default:
      targetCursor = cursorArrow;
      break;
    }
  }
  glfwSetCursor(window, targetCursor);

  if (ui) {
    ui->finalizeFocus();
  }
}

void Application::render() {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);
  glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
  if (ui) {
    ui->windowHeight = (float)h;
    ui->prepareForFrame(proj, *font);
  }

  if (rootSplit) {
    rootSplit->position = {0.0f, 0.0f};
    rootSplit->size = {(float)w, (float)h};
    rootSplit->draw(*ui);
  }

  if (ui) {
    ui->flush();
  }
}