file(GLOB Sources *.cpp)

add_library(vtb ${Sources})

target_link_libraries(vtb glfw glm ${Vulkan_Libraries})

