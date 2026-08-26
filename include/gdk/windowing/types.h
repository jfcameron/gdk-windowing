// © Joseph Cameron - All Rights Reserved

#ifndef GDK_WINDOWING_TYPES_H
#define GDK_WINDOWING_TYPES_H

#include <memory>
#include <utility>

namespace gdk::windowing {
    class window;
    class context;

    using window_ptr_type = std::shared_ptr<window>;

    using context_ptr_type = std::shared_ptr<context>;

    using window_size_type = std::pair<int, int>;

    using window_aspect_ratio_type = double;

    /// \brief how a window paces presentation against the display's refresh
    enum class vsync_mode {
        off,      //!< present as soon as the frame is drawn. Tearing is possible but latency is lowest.
        on,       //!< wait for the display's refresh before presenting. No tearing but latency is highest.
        adaptive  //!< same as `on`, except a late frame is presented immediately. Middle ground
    };

    /// \brief the aspect ratio of a window size
    /// TODO: move to more appropriate file
    [[nodiscard]] constexpr window_aspect_ratio_type aspect_ratio_of(const window_size_type aSize) {
        return aSize.second == 0
            ? 0
            : static_cast<window_aspect_ratio_type>(aSize.first) /
              static_cast<window_aspect_ratio_type>(aSize.second);
    }
}

#endif
