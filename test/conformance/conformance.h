// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_CONFORMANCE_H
#define GDK_WINDOWING_CONFORMANCE_H

#include <gdk/windowing/context.h>
#include <gdk/windowing/types.h>

#include <string_view>

namespace gdk::windowing::conformance {
    [[nodiscard]] context_ptr_type make_context();

    [[nodiscard]] std::string_view implementation();
}

#endif
