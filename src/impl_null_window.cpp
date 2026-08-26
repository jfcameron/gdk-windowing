// © Joseph Cameron - All Rights Reserved

#include <gdk/windowing/impl_null_window.h>


#include <utility>

namespace gdk::windowing {
    std::shared_ptr<impl_null_window> impl_null_window::make(const std::string_view aName,
        const window_size_type aWindowSize, const vsync_mode aVsync) {
        return std::shared_ptr<impl_null_window>(new impl_null_window(aName, aWindowSize, aVsync));
    }

    impl_null_window::impl_null_window(const std::string_view aName,
        const window_size_type aWindowSize, const vsync_mode aVsync)
    : mName(aName)
    , mWindowSize(aWindowSize)
    , mVsync(aVsync)
    {}

    vsync_mode impl_null_window::vsync() const { return mVsync; }

    void impl_null_window::set_icons(const icon_image_collection_type &aIconImages) {
        mIconCount = aIconImages.size();
    }

    void impl_null_window::set_cursor(const cursor_image_type &) {
        mLastStandardCursor.reset();
    }

    void impl_null_window::set_cursor(const standard_cursor_graphic aCursor) {
        mLastStandardCursor = aCursor;
    }

    void impl_null_window::close() { mShouldClose = true; }

    void impl_null_window::swap_buffers() { ++mSwapCount; }

    std::string_view impl_null_window::name() const { return mName; }

    window_size_type impl_null_window::window_size() const { return mWindowSize; }

    window_aspect_ratio_type impl_null_window::aspect_ratio() const {
        return aspect_ratio_of(mWindowSize);
    }

    bool impl_null_window::should_close() const { return mShouldClose; }

    void impl_null_window::set_window_size(const window_size_type aWindowSize) {
        mWindowSize = aWindowSize;
    }

    std::size_t impl_null_window::swap_count() const { return mSwapCount; }

    std::size_t impl_null_window::icon_count() const { return mIconCount; }

    std::optional<window::standard_cursor_graphic> impl_null_window::last_standard_cursor() const {
        return mLastStandardCursor;
    }

    std::shared_ptr<impl_null_context> impl_null_context::make() {
        return std::shared_ptr<impl_null_context>(new impl_null_context());
    }

    window_ptr_type impl_null_context::make_window(const std::string_view aName,
        const window_size_type aWindowSize, const vsync_mode aVsync) {
        ++mWindowCount;

        return impl_null_window::make(aName, aWindowSize, aVsync);
    }

    void impl_null_context::poll_events() { ++mPollCount; }

    std::size_t impl_null_context::poll_count() const { return mPollCount; }

    std::size_t impl_null_context::window_count() const { return mWindowCount; }
}
