// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_NULL_WINDOW_H
#define GDK_WINDOWING_NULL_WINDOW_H

#include <gdk/windowing/window.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace gdk::windowing {
    /// \brief a window that is not on a screen
    class null_window final : public window {
    public:
        [[nodiscard]] static std::shared_ptr<null_window> make(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive);

        [[nodiscard]] vsync_mode vsync() const;

        virtual void set_icons(const icon_image_collection_type &aIconImages) override;
        virtual void set_cursor(const cursor_image_type &aRGBA32) override;
        virtual void set_cursor(const standard_cursor_graphic aCursor) override;
        virtual void close() override;
        virtual void keep_open() override;
        virtual void swap_buffers() override;

        [[nodiscard]] virtual std::string_view name() const override;
        [[nodiscard]] virtual window_size_type window_size() const override;
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const override;
        [[nodiscard]] virtual bool should_close() const override;

        void set_window_size(const window_size_type aWindowSize);

        [[nodiscard]] std::size_t swap_count() const;

        [[nodiscard]] std::size_t icon_count() const;

        [[nodiscard]] std::optional<standard_cursor_graphic> last_standard_cursor() const;

        virtual ~null_window() override = default;

    private:
        null_window(const std::string_view aName, const window_size_type aWindowSize,
            const vsync_mode aVsync);

        std::string mName;
        window_size_type mWindowSize;
        vsync_mode mVsync;
        std::size_t mSwapCount{0};
        std::size_t mIconCount{0};
        std::optional<standard_cursor_graphic> mLastStandardCursor;
        bool mShouldClose{false};
    };
}

#endif
