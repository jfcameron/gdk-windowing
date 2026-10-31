# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkwindowing)
    return()
endif()

project(gdk-windowing
    VERSION 1.0
    DESCRIPTION "the gdk window interface: a context that makes windows, and a null implementation"
    LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKWINDOWING_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKWINDOWING_BUILD_DOCS "Build documentation" OFF)

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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(gdkwindowing STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_window.cpp")

add_library(gdk::windowing ALIAS gdkwindowing)

set_target_properties(gdkwindowing PROPERTIES EXPORT_NAME windowing PREFIX "lib")

target_include_directories(gdkwindowing
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_features(gdkwindowing PUBLIC cxx_std_20)

add_library(gdkwindowing_conformance INTERFACE)

target_sources(gdkwindowing_conformance INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/context_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/window_conformance.cpp")

target_include_directories(gdkwindowing_conformance INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance")

target_link_libraries(gdkwindowing_conformance INTERFACE gdkwindowing)

install(TARGETS gdkwindowing
    EXPORT gdk-windowing-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(EXPORT gdk-windowing-targets
    FILE gdk-windowing-targets.cmake
    NAMESPACE gdk::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-windowing")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/gdk-windowing-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

file(WRITE "${PROJECT_BINARY_DIR}/gdk-windowing-config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-windowing-targets.cmake\")
")

install(FILES
    "${PROJECT_BINARY_DIR}/gdk-windowing-config.cmake"
    "${PROJECT_BINARY_DIR}/gdk-windowing-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-windowing")

if (GDKWINDOWING_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (GDKWINDOWING_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        PROJECT_LOGO     ${CMAKE_CURRENT_LIST_DIR}/docs/icon.png

        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
            impl/null/include/
    )
endif()
