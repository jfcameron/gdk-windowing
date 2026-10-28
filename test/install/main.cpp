// © Joseph Cameron - MIT License

#include <gdk/windowing/null_context.h>
#include <gdk/windowing/window.h>

int main() {
    using namespace gdk::windowing;

    const context_ptr_type pContext = null_context::make();
    const auto pWindow = pContext->make_window("installed", {64, 32});

    pContext->poll_events();
    pWindow->swap_buffers();

    return pWindow->aspect_ratio() == 2.0 ? 0 : 1;
}
