// © Joseph Cameron - All Rights Reserved

#ifndef GDK_WINDOWING_IMPL_ANDROID_WINDOW_H
#define GDK_WINDOWING_IMPL_ANDROID_WINDOW_H

#include <gdk/windowing/window.h>

#include <memory>
#include <string>

struct ANativeWindow;

namespace gdk::windowing {
    /// \brief an Android activity's window, drawn into with OpenGL ES 2 through EGL
    class impl_android_window final : public window {
    public:
        /// \param apWindow the activity's window, which must be there: a context needs a surface to
        /// start with. The context is current on the calling thread once this returns
        [[nodiscard]] static std::shared_ptr<impl_android_window> make(ANativeWindow *const apWindow,
            const std::string_view aName = "Window");

        /// \brief the activity's window 
        void set_native_window(ANativeWindow *const apWindow);

        [[nodiscard]] bool has_native_window() const;

        virtual void set_icons(const icon_image_collection_type &aIconImages) override;
        virtual void set_cursor(const cursor_image_type &aRGBA32PNG) override;
        virtual void set_cursor(const standard_cursor_graphic aCursor) override;
        virtual void close() override;
        virtual void keep_open() override;
        virtual void swap_buffers() override;

        [[nodiscard]] virtual std::string_view name() const override;
        [[nodiscard]] virtual window_size_type window_size() const override;
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const override;
        [[nodiscard]] virtual bool should_close() const override;

        virtual ~impl_android_window() override;

        impl_android_window(const impl_android_window &) = delete;
        impl_android_window &operator=(const impl_android_window &) = delete;

    private:
        impl_android_window(ANativeWindow *const apWindow, const std::string_view aName);

        struct impl;
        std::unique_ptr<impl> m_pImpl;
    };
}

#endif
