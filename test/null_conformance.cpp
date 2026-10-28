// © Joseph Cameron - MIT License

#include "conformance.h"

#include <gdk/windowing/null_context.h>

namespace gdk::windowing::conformance {
    context_ptr_type make_context() { return null_context::make(); }

    std::string_view implementation() { return "null"; }
}
