// © Joseph Cameron - MIT License

#include <gdk/windowing/null_context.h>

#include <gdk/windowing/null_window.h>

using namespace gdk::windowing;

std::shared_ptr<null_context> null_context::make() {
    return std::shared_ptr<null_context>(new null_context());
}

window_ptr_type null_context::make_window(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    ++mWindowCount;

    return null_window::make(aName, aWindowSize, aVsync);
}

void null_context::poll_events() { ++mPollCount; }

std::size_t null_context::poll_count() const { return mPollCount; }

std::size_t null_context::window_count() const { return mWindowCount; }
