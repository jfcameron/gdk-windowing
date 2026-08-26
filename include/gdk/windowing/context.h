// © Joseph Cameron - All Rights Reserved

#ifndef GDK_WINDOWING_CONTEXT_H
#define GDK_WINDOWING_CONTEXT_H

#include <gdk/windowing/types.h>

#include <string_view>

namespace gdk::windowing {
    /// \brief a connection to the window system: makes windows, and pumps the events they receive
    class context {
    public:
        /// \brief make a window belonging to this context
        ///
        /// \param aName what the window system should call it
        /// \param aWindowSize how big to make it, in pixels
        /// \param aVsync how to pace presentation against the display's refresh
        [[nodiscard]] virtual window_ptr_type make_window(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive) = 0;

        /// \brief process whatever the window system has queued
        ///
        /// \attention nothing this context made responds to input until this is called. call this in your update loop
        virtual void poll_events() = 0;

        virtual ~context() = default;
    };
}

#endif
