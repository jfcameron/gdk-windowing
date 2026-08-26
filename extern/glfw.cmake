# © Joseph Cameron - All Rights Reserved

if(EMSCRIPTEN)
    # Emscripten ships glfw as a port of its own, reached with `-sUSE_GLFW=3`: it provides the headers
    # at compile time and wires the implementation to the browser's canvas and event loop at link time.
    jfc_set_dependency_symbols(INCLUDE_PATHS "${PROJECT_BINARY_DIR}")

    return()
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
#set(GLFW_VULKAN_STATIC OFF CACHE BOOL "")

# Workaround to copy glfw output (at build time) to expected location (at generate time). add_custom_command must be called "in same dir" (meaning same CMakeList.txt) so must be injected.
# Note this is only necessary for multi-profile generators (Visual Studio, Xcode)
jfc_git(COMMAND reset --hard WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/glfw") #jfc_git(COMMAND checkout src/CMakeLists.txt WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/glfw")

file(APPEND ${CMAKE_CURRENT_LIST_DIR}/glfw/src/CMakeLists.txt "
    # == Hack that copies output to expected location. If you see this, checkout this file
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX \"lib\")
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND \${CMAKE_COMMAND} -E copy \$<TARGET_FILE:${PROJECT_NAME}> \"\${PROJECT_BINARY_DIR}/src/\$<TARGET_FILE_NAME:${PROJECT_NAME}>\")
    # == Hack that copies output to expected location. If you see this, checkout this file
")

add_subdirectory(${PROJECT_NAME})

jfc_git(COMMAND reset --hard WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/glfw")
#end workaround

if(CMAKE_SYSTEM_NAME MATCHES "Darwin" OR CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
    set (OpenGL_GL_PREFERENCE "GLVND") #Hint for linux + cmake version < 11, ignored by others
    find_package(OpenGL REQUIRED) # find_package(vulkan REQUIRED)

    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        FIND_LIBRARY(COCOA_LIBRARY Cocoa)
        FIND_LIBRARY(CORE_VIDEO CoreVideo)
        FIND_LIBRARY(IO_KIT IOKit)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
        find_package(OpenGL REQUIRED)

         project("GLEW")
 
         add_library(${PROJECT_NAME} STATIC
             ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/src/glew.c)
 
         target_include_directories(${PROJECT_NAME} PRIVATE
             ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include)
 
         set_target_properties(${PROJECT_NAME} PROPERTIES
             RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
 
         set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
 
         add_custom_command(TARGET ${PROJECT_NAME}
             POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")
 
         set(GLEW_LIBRARIES ${PROJECT_BINARY_DIR}/libGLEW${CMAKE_STATIC_LIBRARY_SUFFIX})
         set(GLEW_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include)

        if(CMAKE_SYSTEM_NAME MATCHES "Linux")
            find_package(X11 REQUIRED) # Mir? Wayland?
            find_package(Threads REQUIRED)
        endif()
    endif()
else()
    message(FATAL_ERROR "${PROJECT_NAME}.cmake has not been configured to handle platform \"${CMAKE_SYSTEM_NAME}\".")
endif()

jfc_set_dependency_symbols(
    INCLUDE_PATHS
        ${CMAKE_CURRENT_LIST_DIR}/glfw/include

        # Graphics interface
        ${OPENGL_INCLUDE_DIR}
        ${Vulkan_INCLUDE_DIR}

        # Linux or Windows
        ${GLEW_INCLUDE_DIR}

        # Linux
        ${X11_INCLUDE_DIR}

    LIBRARIES
        ${PROJECT_BINARY_DIR}/glfw/src/libglfw3${CMAKE_STATIC_LIBRARY_SUFFIX}
        
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

        #Linux specific
        ${X11_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

