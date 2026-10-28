# gdk-windowing
The gdk window interface: a `context` that makes windows and pumps their events, and a `window` a renderer draws into. It carries no implementation but a null one, for tests.

API documentation: https://jfcameron.github.io/gdk-windowing/

Implementations:
- [gdk-windowing_glfw](https://github.com/jfcameron/gdk-windowing_glfw): desktop and the browser, through GLFW
- [gdk-windowing_android](https://github.com/jfcameron/gdk-windowing_android): an Android activity's window, through EGL

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library and its tests on linux using gcc
cmake --preset linux-gcc

# build the library and tests
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::windowing`, or install it and `find_package(gdk-windowing)`.

## Usage
Code that only draws into a window, or only reads its size, names the interface:

```cpp
#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>

void run(const gdk::windowing::context_ptr_type &pContext) {
    const auto pWindow = pContext->make_window("my game", {800, 600});

    while (!pWindow->should_close()) {
        pContext->poll_events();

        // draw, into pWindow->window_size() pixels

        pWindow->swap_buffers();
    }
}
```

Whatever composes the program picks the implementation, for example `gdk::windowing::glfw_context::make()`.

## Conformance
`gdkwindowing_conformance` is the suite every implementation passes. An implementation's test target links it and defines `gdk::windowing::conformance::make_context()` and `implementation()` (see `test/conformance/conformance.h`); the null implementation runs it here.
