# internal OpenGL rendering support
if("${PERIDYNO_GPU_BACKEND}" STREQUAL "Vulkan")
    add_subdirectory(VkRenderEngine)
endif()

add_subdirectory(OpenGL)
