// © Joseph Cameron - All Rights Reserved

#include <gdk/windowing/impl_android_window.h>

#include <gdk/windowing/exception.h>

#include <EGL/egl.h>
#include <android/native_window.h>

#include <cstdio>
#include <string>

using namespace gdk::windowing;

namespace {
    [[nodiscard]] std::string egl_failure(const char *const apWhat) {
        return std::string("gdk::windowing::impl_android_window: ") + apWhat + " failed: EGL error 0x"
            + [] {
                char hex[16];
                std::snprintf(hex, sizeof(hex), "%04X", static_cast<unsigned>(eglGetError()));
                return std::string(hex);
            }();
    }
}

struct impl_android_window::impl final {
    std::string name;
    EGLDisplay display{EGL_NO_DISPLAY};
    EGLConfig config{nullptr};
    EGLContext context{EGL_NO_CONTEXT};
    EGLSurface surface{EGL_NO_SURFACE};
    ANativeWindow *pWindow{nullptr};
    window_size_type size{0, 0};
    bool shouldClose{false};

    void attach(ANativeWindow *const apWindow) {
        pWindow = apWindow;

        EGLint format = 0;
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

        ANativeWindow_setBuffersGeometry(apWindow, 0, 0, format);

        surface = eglCreateWindowSurface(display, config, apWindow, nullptr);

        if (surface == EGL_NO_SURFACE) throw exception(egl_failure("eglCreateWindowSurface"));

        if (!eglMakeCurrent(display, surface, surface, context)) throw exception(egl_failure("eglMakeCurrent"));

        eglSwapInterval(display, 1);

        update_size();
    }

    void detach() {
        if (surface != EGL_NO_SURFACE) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(display, surface);

            surface = EGL_NO_SURFACE;
        }

        pWindow = nullptr;
    }

    void update_size() {
        EGLint width = 0, height = 0;

        if (surface != EGL_NO_SURFACE && eglQuerySurface(display, surface, EGL_WIDTH, &width)
            && eglQuerySurface(display, surface, EGL_HEIGHT, &height) && width > 0 && height > 0)
            size = {width, height};
    }
};

std::shared_ptr<impl_android_window> impl_android_window::make(ANativeWindow *const apWindow, const std::string_view aName) {
    return std::shared_ptr<impl_android_window>(new impl_android_window(apWindow, aName));
}

impl_android_window::impl_android_window(ANativeWindow *const apWindow, const std::string_view aName)
: m_pImpl(std::make_unique<impl>()) {
    if (!apWindow) throw exception("gdk::windowing::impl_android_window: needs the activity's window to start with");

    auto &self = *m_pImpl;

    self.name = aName;
    self.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (self.display == EGL_NO_DISPLAY || !eglInitialize(self.display, nullptr, nullptr))
        throw exception(egl_failure("eglInitialize"));

    const EGLint attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE};

    EGLint count = 0;

    if (!eglChooseConfig(self.display, attributes, &self.config, 1, &count) || count < 1)
        throw exception(egl_failure("eglChooseConfig"));

    const EGLint contextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    self.context = eglCreateContext(self.display, self.config, EGL_NO_CONTEXT, contextAttributes);

    if (self.context == EGL_NO_CONTEXT) throw exception(egl_failure("eglCreateContext"));

    self.attach(apWindow);
}

impl_android_window::~impl_android_window() {
    auto &self = *m_pImpl;

    self.detach();

    if (self.context != EGL_NO_CONTEXT) eglDestroyContext(self.display, self.context);
    if (self.display != EGL_NO_DISPLAY) eglTerminate(self.display);
}

void impl_android_window::set_native_window(ANativeWindow *const apWindow) {
    auto &self = *m_pImpl;

    if (apWindow == self.pWindow) return;

    self.detach();

    if (apWindow) self.attach(apWindow);
}

bool impl_android_window::has_native_window() const { return m_pImpl->surface != EGL_NO_SURFACE; }

void impl_android_window::set_icons(const icon_image_collection_type &) {}

void impl_android_window::set_cursor(const cursor_image_type &) {}

void impl_android_window::set_cursor(const standard_cursor_graphic) {}

void impl_android_window::close() { m_pImpl->shouldClose = true; }

void impl_android_window::keep_open() { m_pImpl->shouldClose = false; }

void impl_android_window::swap_buffers() {
    auto &self = *m_pImpl;

    if (self.surface == EGL_NO_SURFACE) return;

    eglSwapBuffers(self.display, self.surface);

    self.update_size();
}

std::string_view impl_android_window::name() const { return m_pImpl->name; }

window_size_type impl_android_window::window_size() const { return m_pImpl->size; }

window_aspect_ratio_type impl_android_window::aspect_ratio() const { return aspect_ratio_of(m_pImpl->size); }

bool impl_android_window::should_close() const { return m_pImpl->shouldClose; }
