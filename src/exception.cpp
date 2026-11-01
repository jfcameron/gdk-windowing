// © Joseph Cameron - MIT License

#include <gdk/windowing/exception.h>

namespace gdk::windowing {
    exception::exception()
    : exception("gdk::windowing::exception")
    {}

    exception::exception(const std::string &aWhat)
    : mpWhat(std::make_shared<const std::string>(aWhat))
    {}

    const char *exception::what() const noexcept { return mpWhat->c_str(); }
}
