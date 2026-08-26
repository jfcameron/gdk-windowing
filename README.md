## gdk-window

windowing library specific for games. Use to make one or more windows, the library prepares the drawing surface, the graphics context, etc, as well as clipboard support, input events and so on.
Strong separation between interface and implementation. C++20 conforming. Provides a glfw based implementation.

Example:

```cpp
#include <gdk/windowing/impl_glfw_window.h> // choosing the glfw implementation
#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>

using namespace gdk::windowing;

const context_ptr_type pContext = impl_glfw_context::make();
const window_ptr_type pWindow = pContext->make_window("my game", {800, 600});

while (!pWindow->should_close()) {
    pContext->poll_events();

    // draw

    pWindow->swap_buffers();
}
```
## building

see `CMakePresets.json` 

```
cmake --preset linux-gcc
cmake --build --preset linux-gcc
ctest --preset linux-gcc
```

