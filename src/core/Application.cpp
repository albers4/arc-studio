#include "Application.h"
#include "../renderer/Shader.h"
#include "../renderer/BatchRenderer.h"
#include "../ui/FontAtlas.h"
#include "../ui/UIManager.h"
#include "../ui/Button.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Application::Application(const std::string& title, int w, int h) 
    : width(w), height(h) 
{
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
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

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
    
    if (font->textureID == 0) {
        std::cerr << "Failed to load font!" << std::endl;
        exit(EXIT_FAILURE);
    }

    renderer = new BatchRenderer(*uiShader);
    ui = new UIManager(*renderer);

    auto& myButton = ui->addWidget<Button>(20, 10, 100, 30, "Render", *font);
    myButton.onClick = []() {
        std::cout << "Render button clicked" << std::endl;
    };
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        update();
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::update() {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    bool leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    ui->updateMouse(mx, my, leftClick);
}

void Application::render() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);

    // --- UI Drawing Code ---
    ui->rect(0, 0, w, 50, glm::vec4(0.3f, 0.3f, 0.35f, 1.0f)); // Header

    ui->flush(proj, *font);
}