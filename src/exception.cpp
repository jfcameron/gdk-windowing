// © Joseph Cameron - MIT License

#include <gdk/windowing/exception.h>

#include <utility>

namespace gdk::windowing {
    exception::exception(std::string aWhat)
    : mWhat(std::move(aWhat))
    {}

    const char *exception::what() const noexcept { return mWhat.c_str(); }
}
