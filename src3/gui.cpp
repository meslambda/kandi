#include "gui.hpp"

Gui::Gui(const std::string& windowName)
{
    glfwInit();

    // Create window with graphics context
    m_mainScale          = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    auto horizontalScale = static_cast<int>((1280 * m_mainScale));
    auto verticalScale   = static_cast<int>((800 * m_mainScale));
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    m_window = glfwCreateWindow(horizontalScale, verticalScale, windowName.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    static_cast<void>(io);

    ImGui::StyleColorsDark(); // Setup darkmode

    // Setup scaling
    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(m_mainScale);         // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi         = m_mainScale; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts     = true;        // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;        // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Gui::startFrame()
{
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::endFrameAndRender()
{
    auto clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Render();
    int displayW{}, displayH{};
    glfwGetFramebufferSize(m_window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

bool Gui::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Gui::tableHelper(const std::string& label)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(label.c_str());
    ImGui::TableSetColumnIndex(1);
}
