// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/windowing/context.h>
#include <gdk/windowing/exception.h>
#include <gdk/windowing/window.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

using namespace gdk::windowing;

namespace {
    [[nodiscard]] window_ptr_type a_window(const context_ptr_type &aContext,
        const std::string_view aName = "conformance") {
        return aContext->make_window(aName, {320, 240}, vsync_mode::off);
    }
}

TEST_CASE("conformance: a window reports the name it was given", "[conformance][window]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();

    SECTION("as given")
    {
        REQUIRE(a_window(pContext, "Cool Window")->name() == "Cool Window");
    }

    SECTION("even from a view that is not null terminated")
    {
        const std::string longer("named window, and more besides");

        REQUIRE(a_window(pContext, std::string_view(longer).substr(0, 12))->name() == "named window");
    }
}

TEST_CASE("conformance: a window has a drawing surface", "[conformance][window]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = a_window(pContext);

    pContext->poll_events();

    const auto size = pWindow->window_size();

    REQUIRE(size.first > 0);
    REQUIRE(size.second > 0);

    SECTION("and its aspect ratio is its size's")
    {
        REQUIRE(pWindow->aspect_ratio() == Approx(aspect_ratio_of(size)));
    }
}

TEST_CASE("conformance: closing is a flag the caller sets and clears", "[conformance][window]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = a_window(pContext);

    REQUIRE_FALSE(pWindow->should_close());

    pWindow->close();
    REQUIRE(pWindow->should_close());

    pWindow->close();
    REQUIRE(pWindow->should_close());

    pWindow->keep_open();
    REQUIRE_FALSE(pWindow->should_close());
}

TEST_CASE("conformance: every cursor can be asked for", "[conformance][window]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = a_window(pContext);

    for (const auto cursor : {window::standard_cursor_graphic::arrow, window::standard_cursor_graphic::ibeam,
        window::standard_cursor_graphic::crosshair, window::standard_cursor_graphic::hand,
        window::standard_cursor_graphic::horizontal_resizer, window::standard_cursor_graphic::vertical_resizer})
        REQUIRE_NOTHROW(pWindow->set_cursor(cursor));

    REQUIRE_NOTHROW(pWindow->set_cursor(window::cursor_image_type{}));
}

TEST_CASE("conformance: icons are checked against their dimensions", "[conformance][window]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = a_window(pContext);

    SECTION("none asks for the system's default")
    {
        REQUIRE_NOTHROW(pWindow->set_icons({}));
    }

    SECTION("a well formed set is accepted")
    {
        window::icon_image_collection_type icons;
        icons.push_back(window::icon_image_type{16, 16, std::vector<std::byte>(16 * 16 * 4)});
        icons.push_back(window::icon_image_type{32, 32, std::vector<std::byte>(32 * 32 * 4)});

        REQUIRE_NOTHROW(pWindow->set_icons(icons));
    }

    SECTION("an icon carrying less data than its dimensions claim is refused")
    {
        window::icon_image_collection_type icons;
        icons.push_back(window::icon_image_type{16, 16, std::vector<std::byte>(16 * 16 * 4 - 1)});

        REQUIRE_THROWS_AS(pWindow->set_icons(icons), exception);
    }
}

TEST_CASE("conformance: deleting through the interface is defined", "[conformance][window]")
{
    STATIC_REQUIRE(std::has_virtual_destructor_v<window>);
    STATIC_REQUIRE(std::has_virtual_destructor_v<context>);
}
