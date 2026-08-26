# © Joseph Cameron - All Rights Reserved

if(ANDROID)
elseif(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    include("${CMAKE_CURRENT_LIST_DIR}/glfw.cmake")
else()
    if (NOT TARGET glfw)
        add_library(glfw INTERFACE)

        target_include_directories(glfw INTERFACE "$<BUILD_INTERFACE:${EMSCRIPTEN_SYSROOT}/include>")

        target_link_options(glfw INTERFACE "-sUSE_GLFW=3")
    endif()
endif()

