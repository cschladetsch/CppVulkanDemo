#include <iostream>
#include <fstream>
#include <vector>

#include "Glfw.hpp"
#include "Glm.hpp"

#include "App.hpp"

using namespace std;

vector<char> ReadFileBinary(const char *filename)
{
    ifstream input(filename, ios::binary);
    return vector<char>((
        istreambuf_iterator<char>(input)),
        (istreambuf_iterator<char>()));
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    VulkanApp app;
    vector<char> vert, frag;
    string res = "../../resources/";
    vert = ReadFileBinary((res + "vert.spv").c_str());
    frag = ReadFileBinary((res + "frag.spv").c_str());

    auto hwnd = glfwGetWin32Window(window);
    app.Create(hwnd, vert, frag, (res + "chalet.jpg").c_str(), (res + "chalet.obj").c_str());

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        app.Render();
    }

    app.Destroy();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
