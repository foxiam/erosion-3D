#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "test.h"

namespace Engine {
	int Engine::CheckGLFW()
	{
		std::cout << "Engine start" << std::endl;


        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(1024, 768, "Tech demo", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "LOG CRITICAL GLAD" << std::endl;
            return -1;
        }

        std::string vendor = "Vendror: ";
        vendor += (const char*)glGetString(GL_VENDOR);
        std::string renderer = "Renderer: ";
        renderer += (const char*)glGetString(GL_RENDERER);
        std::string version = "Version: ";
        version += (const char*)glGetString(GL_VERSION);
        std::string glsl_version = "GLSL Version: ";
        glsl_version += (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);


        std::cout << vendor << std::endl;
        std::cout << renderer << std::endl;
        std::cout << version << std::endl;
        std::cout << glsl_version << std::endl;

        glClearColor(0, 0, 1, 0);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);


            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize.x = static_cast<float>(1024);
            io.DisplaySize.y = static_cast<float>(768);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
	}
}

