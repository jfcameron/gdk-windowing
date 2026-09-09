# © Joseph Cameron - All Rights Reserved

if (TARGET glfw)
    return()
endif()

if(EMSCRIPTEN)
    add_library(glfw INTERFACE)

    target_include_directories(glfw INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

    return()
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(glfw)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin" OR CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
    set (OpenGL_GL_PREFERENCE "GLVND") #Hint for linux + cmake version < 11, ignored by others
    find_package(OpenGL REQUIRED) 

    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        FIND_LIBRARY(COCOA_LIBRARY Cocoa)
        FIND_LIBRARY(CORE_VIDEO CoreVideo)
        FIND_LIBRARY(IO_KIT IOKit)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
        find_package(OpenGL REQUIRED)

         add_library(GLEW STATIC ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/src/glew.c)

         target_include_directories(GLEW PUBLIC
             "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include>")

         set_target_properties(GLEW PROPERTIES PREFIX "lib")

         set(GLEW_LIBRARIES GLEW)
         set(GLEW_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include)

        if(CMAKE_SYSTEM_NAME MATCHES "Linux")
            find_package(X11 REQUIRED) # Mir? Wayland?
            find_package(Threads REQUIRED)
        endif()
    endif()
else()
    message(FATAL_ERROR "${PROJECT_NAME}.cmake has not been configured to handle platform \"${CMAKE_SYSTEM_NAME}\".")
endif()

target_include_directories(glfw INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glfw/include>"

    # Graphics interface
    ${OPENGL_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIR}

    # Linux or Windows
    ${GLEW_INCLUDE_DIR}

    # Linux
    ${X11_INCLUDE_DIR}
)

set_property(TARGET glfw APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    # Linux or Windows
    ${GLEW_LIBRARIES}
    ${CMAKE_DL_LIBS}

    # Graphics interface
    ${OPENGL_LIBRARIES}
    ${Vulkan_LIBRARIES}

    # Macos
    ${COCOA_LIBRARY}
    ${CORE_VIDEO}
    ${IO_KIT}

    # Linux specific
    ${X11_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})
