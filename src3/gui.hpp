#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

#include <string>

class Gui
{
  public:
    Gui(const std::string& windowName);
    ~Gui();

    void startFrame();
    void endFrameAndRender();
    bool shouldClose() const;

  protected:
    float m_mainScale{};

    void tableHelper(const std::string& label);

  private:
    GLFWwindow* m_window{};
};
