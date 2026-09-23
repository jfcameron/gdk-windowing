// © Joseph Cameron - All Rights Reserved

#include <gdk/windowing/build_info.h>

#include <gdk/windowing/impl_glfw_window.h>

#include <gdk/windowing/exception.h>

#ifdef JFC_TARGET_PLATFORM_Emscripten
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#define GLFW_INCLUDE_ES2
#elif defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <algorithm>
#include <exception>
#include <functional>
#include <iostream>
#include <cstdio>
#include <mutex>
#include <optional>
#include <sstream>
#include <vector>

static constexpr char TAG[] = "impl_glfw_window";

namespace {
    char gLastError[256] = {};

    bool gHasError = false;

    void record_error(int, const char *const aMessage) noexcept {
        std::snprintf(gLastError, sizeof gLastError, "%s",
            aMessage ? aMessage : "glfw reported an error it could not describe");

        gHasError = true;
    }

    void clear_error() noexcept {
        gLastError[0] = '\0';

        gHasError = false;
    }

    [[nodiscard]] std::optional<std::string> taken_error() {
        if (!gHasError) return {};

        std::string out(gLastError);

        clear_error();

        return out;
    }

    void throw_recorded_error(const char *const aWhere) {
        if (const auto error = taken_error())
            throw gdk::windowing::exception(std::string(TAG).append("/").append(aWhere)
                .append(": ").append(*error));
    }

    class glfw_session final {
    public:
        glfw_session(const glfw_session &) = delete;
        glfw_session &operator=(const glfw_session &) = delete;

        glfw_session() {
            glfwSetErrorCallback(record_error);

            clear_error();

            if (!glfwInit())
                throw gdk::windowing::exception(std::string(TAG).append("/glfwInit failed: ")
                    .append(taken_error().value_or("glfw gave no reason")));

            glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }

        ~glfw_session() { glfwTerminate(); }

        [[nodiscard]] GLFWwindow *share_root() const {
            return mLive.empty() ? nullptr : mLive.front();
        }

        void joined(GLFWwindow *const apWindow) { mLive.push_back(apWindow); }

        void left(GLFWwindow *const apWindow) {
            mLive.erase(std::remove(mLive.begin(), mLive.end(), apWindow), mLive.end());
        }

    private:
        std::vector<GLFWwindow *> mLive;
    };

    [[nodiscard]] std::shared_ptr<glfw_session> acquire_session() {
        static std::weak_ptr<glfw_session> weak;

        auto shared = weak.lock();

        if (!shared) {
            shared = std::make_shared<glfw_session>();

            weak = shared;
        }

        return shared;
    }
}

using namespace gdk::windowing;

std::shared_ptr<impl_glfw_window> impl_glfw_window::make(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return std::shared_ptr<impl_glfw_window>(new impl_glfw_window(aName, aWindowSize, aVsync));
}

void impl_glfw_window::poll_events() {
    clear_error();

    glfwPollEvents();

    throw_recorded_error("poll_events");
}

void impl_glfw_window::swap_buffers() {
    glfwSwapBuffers(m_pGLFWWindow.get());
}

namespace {
    [[nodiscard]] gdk::windowing::impl_glfw_window::window_pointer_type create_glfw_window(
        const std::string_view aName, const gdk::windowing::window_size_type aWindowSize,
        const gdk::windowing::vsync_mode aVsync) {
        auto pSession = acquire_session();

        const std::string name(aName);

        GLFWwindow *const pWindow = glfwCreateWindow(aWindowSize.first, aWindowSize.second,
            name.c_str(), nullptr, pSession->share_root());

        if (!pWindow)
            throw gdk::windowing::exception(std::string(TAG).append(
                "/glfwCreateWindow failed. Can the environment provide a GLES2.0/WebGL1.0 context? "
                "glfw said: ").append(taken_error().value_or("nothing")));

        glfwMakeContextCurrent(pWindow);

        clear_error();

        switch (aVsync) {
            case gdk::windowing::vsync_mode::off: glfwSwapInterval(0); break;
            case gdk::windowing::vsync_mode::on: glfwSwapInterval(1); break;

            case gdk::windowing::vsync_mode::adaptive: {
                glfwSwapInterval(-1);

                if (taken_error()) glfwSwapInterval(1);
            } break;
        }

        clear_error();

#if !defined JFC_TARGET_PLATFORM_Emscripten
        if (glfwRawMouseMotionSupported()) glfwSetInputMode(pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        clear_error();
#endif

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
        static std::once_flag glewInitFlag;
        std::call_once(glewInitFlag, []() {
            glewExperimental = true; 

            if (GLenum err = glewInit() != GLEW_OK) {
                std::stringstream ss;
                ss << TAG << "/glewinit failed: " << glewGetErrorString(err);
                throw gdk::windowing::exception(ss.str());
            }
        });
#endif

        glClearColor(0, 0, 0, 1); 
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(pWindow); 
        glfwPollEvents(); 

        pSession->joined(pWindow);

        return gdk::windowing::impl_glfw_window::window_pointer_type(pWindow,
            [pSession](GLFWwindow *const ptr) {
                pSession->left(ptr);

                glfwDestroyWindow(ptr);
            });
    }
}

impl_glfw_window::impl_glfw_window(const std::string_view aName, const window_size_type aWindowSize,
    const vsync_mode aVsync)
: m_pGLFWWindow(create_glfw_window(aName, aWindowSize, aVsync))
, m_Name(aName)
, m_WindowSize(aWindowSize) {
    glfwSetWindowUserPointer(m_pGLFWWindow.get(), static_cast<void *>(this));

    glfwSetWindowSizeCallback(m_pGLFWWindow.get(),
        [](GLFWwindow *const pCurrentGLFWwindow, int aX, int aY) {
            if (auto pCurrentWrapper =
                static_cast<impl_glfw_window *>(glfwGetWindowUserPointer(pCurrentGLFWwindow))) {
                pCurrentWrapper->m_WindowSize.first = aX;
                pCurrentWrapper->m_WindowSize.second = aY;
            }
            else record_error(0, "window resized but no wrapper is associated with it");
        });

    glfwSetWindowCloseCallback(m_pGLFWWindow.get(), [](GLFWwindow *const pCurrentWindow) {
        glfwSetWindowShouldClose(pCurrentWindow, GLFW_TRUE);
    });

    clear_error();
}

impl_glfw_window::window_pointer_type impl_glfw_window::ptr_to_implementation() { return m_pGLFWWindow; }

window_size_type impl_glfw_window::window_size() const {
    return m_WindowSize;
}

window_aspect_ratio_type impl_glfw_window::aspect_ratio() const {
    return aspect_ratio_of(m_WindowSize);
}

std::string_view impl_glfw_window::name() const {
    return m_Name;
}

bool impl_glfw_window::should_close() const {
    return glfwWindowShouldClose(m_pGLFWWindow.get());
}

void impl_glfw_window::close() {
	glfwSetWindowShouldClose(m_pGLFWWindow.get(), true);
}

void impl_glfw_window::keep_open() {
	glfwSetWindowShouldClose(m_pGLFWWindow.get(), false);
}

void impl_glfw_window::set_icons(const icon_image_collection_type &aIconImages) {
    if (aIconImages.empty()) {
        glfwSetWindowIcon(m_pGLFWWindow.get(), 0, nullptr);

        return;
    }

    std::vector<GLFWimage> glfwImages;

    glfwImages.reserve(aIconImages.size());

    for (const auto &iconImage : aIconImages) {
        const auto required = iconImage.width_pixels * iconImage.height_pixels
            * icon_image_type::CHANNEL_COUNT;

        if (iconImage.data_rgba32.size() < required)
            throw exception(std::string(TAG).append("/set_icons: an icon says it is ")
                .append(std::to_string(iconImage.width_pixels)).append("x")
                .append(std::to_string(iconImage.height_pixels)).append(", which needs ")
                .append(std::to_string(required)).append(" bytes, but carries ")
                .append(std::to_string(iconImage.data_rgba32.size())));

        GLFWimage image;
        image.width = static_cast<int>(iconImage.width_pixels);
        image.height = static_cast<int>(iconImage.height_pixels);
        image.pixels = const_cast<unsigned char *>(
            reinterpret_cast<const unsigned char *>(iconImage.data_rgba32.data()));

        glfwImages.push_back(image);
    }

    glfwSetWindowIcon(m_pGLFWWindow.get(), static_cast<int>(glfwImages.size()), glfwImages.data());
}

void impl_glfw_window::set_cursor(const cursor_image_type &aRGBA32PNG) {
    GLFWimage image;
    image.width = 16;
    image.height = 16;
    image.pixels = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(&aRGBA32PNG[0]));
    
    m_pGLFWCursor = decltype(m_pGLFWCursor)(
        glfwCreateCursor(&image, 0, 0),
        [](GLFWcursor *p) {
            if (p) glfwDestroyCursor(p);
        });

    glfwSetCursor(m_pGLFWWindow.get(), m_pGLFWCursor.get());
}

void impl_glfw_window::set_cursor(const standard_cursor_graphic cursor) {
    decltype(GLFW_ARROW_CURSOR) glfwStandardCursor;

    switch(cursor) {
        case standard_cursor_graphic::arrow: glfwStandardCursor = GLFW_ARROW_CURSOR; break;
        case standard_cursor_graphic::ibeam: glfwStandardCursor = GLFW_IBEAM_CURSOR; break;
        case standard_cursor_graphic::crosshair: glfwStandardCursor = GLFW_CROSSHAIR_CURSOR; break;
        case standard_cursor_graphic::hand: glfwStandardCursor = GLFW_HAND_CURSOR; break;
        case standard_cursor_graphic::horizontal_resizer: glfwStandardCursor = GLFW_HRESIZE_CURSOR; break;
        case standard_cursor_graphic::vertical_resizer: glfwStandardCursor = GLFW_VRESIZE_CURSOR; break;
        
        default: throw std::invalid_argument("unhandled standard_cursor_graphic type");
    }

    m_pGLFWCursor = decltype(m_pGLFWCursor)(
        glfwCreateStandardCursor(glfwStandardCursor),
        [](GLFWcursor *p) {
            if (p) glfwDestroyCursor(p);
        });

    glfwSetCursor(m_pGLFWWindow.get(), m_pGLFWCursor.get());
}

context_ptr_type impl_glfw_context::make() {
    return context_ptr_type(new impl_glfw_context());
}

window_ptr_type impl_glfw_context::make_window(const std::string_view aName,
    const window_size_type aWindowSize, const vsync_mode aVsync) {
    return impl_glfw_window::make(aName, aWindowSize, aVsync);
}

void impl_glfw_context::poll_events() { impl_glfw_window::poll_events(); }
