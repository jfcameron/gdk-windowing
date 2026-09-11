# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(gdkwindow
    VERSION 0.5
    DESCRIPTION "crossplatform window interface and a glfw implementation of it")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DEMO "Build the demo" OFF)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

set(_jfc_outer_build_tests "${JFC_BUILD_TESTS}")
set(_jfc_outer_build_docs "${JFC_BUILD_DOCS}")
set(_jfc_outer_build_demo "${JFC_BUILD_DEMO}")

set(JFC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO OFF CACHE BOOL "" FORCE)

add_subdirectory(extern)

set(JFC_BUILD_TESTS "${_jfc_outer_build_tests}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS "${_jfc_outer_build_docs}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO "${_jfc_outer_build_demo}" CACHE BOOL "" FORCE)

if(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(platform_dependencies "GLEW")
endif()

if(NOT EMSCRIPTEN)
    set(platform_dependencies ${platform_dependencies} "glfw")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkwindow STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/impl_glfw_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/impl_null_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp)

add_library(gdk::window ALIAS gdkwindow)

set_target_properties(gdkwindow PROPERTIES EXPORT_NAME window)

set_target_properties(gdkwindow PROPERTIES PREFIX "lib")

jfc_generate_build_info()

target_include_directories(gdkwindow
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/include>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        "${PROJECT_BINARY_DIR}/generated_include/"
    )

target_compile_features(gdkwindow PUBLIC cxx_std_20)

target_link_libraries(gdkwindow PUBLIC glfw)

if (platform_dependencies)
    add_dependencies(gdkwindow ${platform_dependencies})
endif()

set_property(TARGET gdkwindow PROPERTY CXX_EXTENSIONS OFF)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()
