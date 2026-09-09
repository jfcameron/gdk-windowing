# © 2018 Joseph Cameron - All Rights Reserved

if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    include("${CMAKE_CURRENT_LIST_DIR}/glfw.cmake")
else()
    set(glfw_INCLUDE_DIR "/Users/josephcameron/Workspace/emscripten/system/include")

    if (NOT TARGET glfw)
        add_library(glfw INTERFACE)

        target_include_directories(glfw INTERFACE "$<BUILD_INTERFACE:${glfw_INCLUDE_DIR}>")
    endif()
endif()
