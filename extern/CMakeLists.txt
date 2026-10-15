# © 2018 Joseph Cameron - All Rights Reserved

if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    include("${CMAKE_CURRENT_LIST_DIR}/glfw.cmake")
else()
    # emscripten's GLFW is built in: its headers are in the toolchain's sysroot, and -sUSE_GLFW=3
    # links it. The target stands for that, so what links glfw asks for nothing else
    if (NOT TARGET glfw)
        add_library(glfw INTERFACE)

        target_include_directories(glfw INTERFACE "$<BUILD_INTERFACE:${EMSCRIPTEN_SYSROOT}/include>")

        target_link_options(glfw INTERFACE "-sUSE_GLFW=3")
    endif()
endif()
