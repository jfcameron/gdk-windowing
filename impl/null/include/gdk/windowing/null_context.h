// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_NULL_CONTEXT_H
#define GDK_WINDOWING_NULL_CONTEXT_H

#include <gdk/windowing/context.h>

#include <cstddef>
#include <memory>
#include <string_view>

namespace gdk::windowing {
    /// \brief a window system with no screen: makes \ref null_window "null windows" and counts what it is asked
    class null_context final : public context {
    public:
        [[nodiscard]] static std::shared_ptr<null_context> make();

        [[nodiscard]] virtual window_ptr_type make_window(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive) override;

        virtual void poll_events() override;

        [[nodiscard]] std::size_t poll_count() const;

        [[nodiscard]] std::size_t window_count() const;

        virtual ~null_context() override = default;

    private:
        null_context() = default;

        std::size_t mPollCount{0};
        std::size_t mWindowCount{0};
    };
}

#endif
