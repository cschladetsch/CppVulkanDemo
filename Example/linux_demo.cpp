#include <iostream>
#include <fstream>
#include <vector>

#include <GLFW/glfw3.h>
#include "vtb/Glm.hpp"
#include "vtb/App.hpp"

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
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Demo", nullptr, nullptr);
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        const char* error_desc;
        int error_code = glfwGetError(&error_desc);
        if (error_desc) {
            std::cerr << "GLFW Error " << error_code << ": " << error_desc << std::endl;
        }
        glfwTerminate();
        return -1;
    }

    VulkanApp app;
    vector<char> vert, frag;
    string res = "resources/";
    vert = ReadFileBinary((res + "vert.spv").c_str());
    frag = ReadFileBinary((res + "frag.spv").c_str());

    std::cout << "Creating Vulkan app..." << std::endl;
    app.Create(window, vert, frag, (res + "chalet.jpg").c_str(), (res + "chalet.obj").c_str());
    std::cout << "Vulkan app created successfully!" << std::endl;

    std::cout << "Starting render loop..." << std::endl;
    int frame_count = 0;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        app.Render();
        frame_count++;
        if (frame_count % 60 == 0) {
            std::cout << "Rendered " << frame_count << " frames" << std::endl;
        }
    }

    std::cout << "Closing application after " << frame_count << " frames" << std::endl;
    app.Destroy();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}