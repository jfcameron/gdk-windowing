# © Joseph Cameron - All Rights Reserved

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX .html) # bit of a hack. Probably should be parameterized.
endif()

jfc_project(executable
    NAME "gdkwindow-demo"
    VERSION 1.0
    DESCRIPTION "demo gdkwindow app"
    C++_STANDARD 20
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    
    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkwindow_INCLUDE_DIRECTORIES}"

    LIBRARIES
        "${gdkwindow_LIBRARIES}"

    DEPENDENCIES
        "gdkwindow"
)
