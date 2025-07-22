file(GLOB Sources *.cpp)

add_library(vtb ${Sources})

if(WIN32)
    target_link_libraries(vtb glfw ${Vulkan_LIBRARIES})
else()
    target_link_libraries(vtb ${Vulkan_LIBRARIES})
endif()

