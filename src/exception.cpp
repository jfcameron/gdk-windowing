// © Joseph Cameron - All Rights Reserved

#include <gdk/windowing/exception.h>

namespace gdk::windowing {
    exception::exception(std::string aWhat)
    : mWhat(std::move(aWhat))
    {}

    const char *exception::what() const noexcept { return mWhat.c_str(); }
}
