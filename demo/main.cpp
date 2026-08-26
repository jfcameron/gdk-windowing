// © Joseph Cameron - All Rights Reserved

#include <gdk/windowing/impl_glfw_window.h>
#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>

#ifdef JFC_TARGET_PLATFORM_Emscripten
#include <emscripten/emscripten.h>
#endif

#include <cstdlib>
#include <iostream>

using namespace gdk;

namespace {
    windowing::context_ptr_type pContext;
    windowing::window_ptr_type pWindow;

    void do_frame() {
        pContext->poll_events();

        pWindow->swap_buffers();
    }
}

int main() {
    std::cout << "beginning demo...\n";

    pContext = windowing::impl_glfw_context::make();
    pWindow = pContext->make_window("demo");

#ifdef JFC_TARGET_PLATFORM_Emscripten
    emscripten_set_main_loop(do_frame, 0, 1);
#else
    while (!pWindow->should_close()) do_frame();
#endif

    return EXIT_SUCCESS;
}
