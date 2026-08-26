// © Joseph Cameron - All Rights Reserved

#ifndef GDK_WINDOWING_IMPL_GLFW_WINDOW_H
#define GDK_WINDOWING_IMPL_GLFW_WINDOW_H

#include <gdk/windowing/window.h>
#include <gdk/windowing/context.h>

#include <functional>
#include <memory>
#include <string>

struct GLFWwindow;
struct GLFWcursor;

namespace gdk::windowing {
    /// \brief the glfw implementation of \ref window
    class impl_glfw_window final : public window {
    public:
        using window_pointer_type = std::shared_ptr<GLFWwindow>;
        using cursor_pointer_type = std::unique_ptr<GLFWcursor, std::function<void(GLFWcursor *)>>;

        //! Makes a window instance using glfw as the implementation
        ///
        /// \param aName what the window system should call it
        /// \param aWindowSize how big to make it, in pixels
        /// \param aVsync how to pace presentation. 
        [[nodiscard]] static std::shared_ptr<impl_glfw_window> make(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive);

        //! Processess all glfw events
        /// \attention this must be called in order for any windows to respond to input events. typically call it once per frame in your game loop.
        static void poll_events();

        //! gets a shared_ptr to the underlying glfw window
        [[nodiscard]] window_pointer_type ptr_to_implementation();

        virtual void swap_buffers() override;
        [[nodiscard]] virtual bool should_close() const override;
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const override;
        [[nodiscard]] virtual std::string_view name() const override;
        virtual void set_cursor(const cursor_image_type &aRGBA32PNG) override;
        virtual void set_cursor(const standard_cursor_graphic cursor) override;
        virtual void set_icons(const icon_image_collection_type &aIconImages) override;
        [[nodiscard]] virtual window_size_type window_size() const override;
        virtual void close() override;

        virtual void keep_open() override;

        virtual ~impl_glfw_window() override = default;

    private:
        impl_glfw_window(const std::string_view aName, const window_size_type aWindowSize,
            const vsync_mode aVsync);

        cursor_pointer_type m_pGLFWCursor;

        window_pointer_type m_pGLFWWindow;

        std::string m_Name;
        window_size_type m_WindowSize;
    };

    class impl_glfw_context final : public context {
    public:
        [[nodiscard]] static context_ptr_type make();

        [[nodiscard]] virtual window_ptr_type make_window(
            const std::string_view aName = "Window",
            const window_size_type aWindowSize = {400, 300},
            const vsync_mode aVsync = vsync_mode::adaptive) override;

        virtual void poll_events() override;

        virtual ~impl_glfw_context() override = default;

    private:
        impl_glfw_context() = default;
    };
}

#endif
