# gdk-windowing

windowing library specific for games. Use to make one or more windows, the library prepares the drawing surface, the graphics context, etc, as well as clipboard support, input events and so on. The library defines the interface and provides a GLFW based implementation.

## Getting Started
Install cmake version 3.21 or greater, and a c++20 compatible compiler 
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the demo programs on linux using gcc
cmake --preset linux-gcc-demo

# build the library and demo programs
cmake --build --preset linux-gcc-demo
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see exactly how CI runners build it on different platforms.

## Example usage

```cpp
#include <gdk/windowing/impl_glfw_window.h> // choosing the glfw implementation
#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>

using namespace gdk::windowing;

const context_ptr_type pContext = impl_glfw_context::make();
const window_ptr_type pWindow = pContext->make_window("my game", {800, 600});

while (!pWindow->should_close()) {
    pContext->poll_events();

    // your draw code goes here

    pWindow->swap_buffers();
}
```
