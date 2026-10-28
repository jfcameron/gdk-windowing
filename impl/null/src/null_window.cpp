// © Joseph Cameron - MIT License

#include <gdk/windowing/null_window.h>

using namespace gdk::windowing;

std::shared_ptr<null_window> null_window::make(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return std::shared_ptr<null_window>(new null_window(aName, aWindowSize, aVsync));
}

null_window::null_window(const std::string_view aName, const window_size_type aWindowSize,
    const vsync_mode aVsync)
: mName(aName)
, mWindowSize(aWindowSize)
, mVsync(aVsync)
{}

vsync_mode null_window::vsync() const { return mVsync; }

void null_window::set_icons(const icon_image_collection_type &aIconImages) {
    require_complete(aIconImages);

    mIconCount = aIconImages.size();
}

void null_window::set_cursor(const cursor_image_type &) { mLastStandardCursor.reset(); }

void null_window::set_cursor(const standard_cursor_graphic aCursor) { mLastStandardCursor = aCursor; }

void null_window::close() { mShouldClose = true; }

void null_window::keep_open() { mShouldClose = false; }

void null_window::swap_buffers() { ++mSwapCount; }

std::string_view null_window::name() const { return mName; }

window_size_type null_window::window_size() const { return mWindowSize; }

window_aspect_ratio_type null_window::aspect_ratio() const { return aspect_ratio_of(mWindowSize); }

bool null_window::should_close() const { return mShouldClose; }

void null_window::set_window_size(const window_size_type aWindowSize) { mWindowSize = aWindowSize; }

std::size_t null_window::swap_count() const { return mSwapCount; }

std::size_t null_window::icon_count() const { return mIconCount; }

std::optional<window::standard_cursor_graphic> null_window::last_standard_cursor() const {
    return mLastStandardCursor;
}
