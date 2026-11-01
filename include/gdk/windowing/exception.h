// © Joseph Cameron - MIT License

#ifndef GDK_WINDOWING_EXCEPTION_H
#define GDK_WINDOWING_EXCEPTION_H

#include <gdk/windowing/types.h>

#include <exception>
#include <memory>
#include <string>

namespace gdk::windowing {
    /// \brief root exception type for this project
    class exception : public std::exception {
        std::shared_ptr<const std::string> mpWhat;

    public:
        exception();

        exception(const std::string &aWhat);

        exception(const exception &aOther) noexcept = default;

        exception &operator=(const exception &aOther) noexcept = default;

        const char *what() const noexcept override;
    };
}

#endif
