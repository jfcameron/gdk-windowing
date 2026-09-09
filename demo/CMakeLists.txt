# © Joseph Cameron - All Rights Reserved

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html) 
endif()

project(gdkwindow-demo)

add_executable(gdkwindow-demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(gdkwindow-demo PRIVATE gdkwindow)

target_compile_features(gdkwindow-demo PRIVATE cxx_std_20)
