// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/windowing/impl_null_window.h>
#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>
#include <gdk/windowing/exception.h>

#include <string>
#include <vector>

using namespace gdk::windowing;

TEST_CASE("a window reports what it was made with", "[window]")
{
    const window_ptr_type pWindow = impl_null_window::make("Cool Window", {640, 480});

    REQUIRE(pWindow);
    REQUIRE(pWindow->name() == "Cool Window");
    REQUIRE(pWindow->window_size() == window_size_type{640, 480});
}

TEST_CASE("closing is a flag the caller sets and reads back", "[window]")
{
    const window_ptr_type pWindow = impl_null_window::make();

    REQUIRE_FALSE(pWindow->should_close());

    pWindow->close();

    REQUIRE(pWindow->should_close());

    SECTION("and asking twice does not undo it")
    {
        pWindow->close();

        REQUIRE(pWindow->should_close());
    }
}

TEST_CASE("aspect ratio follows the size", "[window]")
{
    auto pWindow = impl_null_window::make("Sized", {800, 400});

    SECTION("width over height")
    {
        REQUIRE(pWindow->aspect_ratio() == Approx(2.0));
    }

    SECTION("and it tracks a resize")
    {
        pWindow->set_window_size({400, 800});

        REQUIRE(pWindow->aspect_ratio() == Approx(0.5));
        REQUIRE(pWindow->window_size() == window_size_type{400, 800});
    }

    SECTION("a window with no height has a defined answer rather than a divide by zero")
    {
        pWindow->set_window_size({0, 0});

        REQUIRE(pWindow->aspect_ratio() == 0);
    }
}

TEST_CASE("the aspect ratio rule has one definition", "[window]")
{
    static_assert(aspect_ratio_of({800, 400}) == 2.0, "width over height");
    static_assert(aspect_ratio_of({400, 800}) == 0.5, "and the other way up");
    static_assert(aspect_ratio_of({0, 0}) == 0, "a minimised window is defined, not undefined");
    static_assert(aspect_ratio_of({640, 0}) == 0, "no height is the case that would divide by zero");

    SECTION("a square window")
    {
        REQUIRE(aspect_ratio_of({512, 512}) == Approx(1.0));
    }

    SECTION("no width is representable, since only the divisor can be degenerate")
    {
        REQUIRE(aspect_ratio_of({0, 480}) == 0);
    }
}

TEST_CASE("presentation and cursor requests reach the implementation", "[window]")
{
    auto pNull = impl_null_window::make();

    const window_ptr_type pWindow = pNull;

    SECTION("swap_buffers is on the interface, so a frame can be presented without naming a backend")
    {
        REQUIRE(pNull->swap_count() == 0);

        pWindow->swap_buffers();
        pWindow->swap_buffers();

        REQUIRE(pNull->swap_count() == 2);
    }

    SECTION("a standard cursor is remembered as one")
    {
        pWindow->set_cursor(window::standard_cursor_graphic::crosshair);

        REQUIRE(pNull->last_standard_cursor() == window::standard_cursor_graphic::crosshair);
    }

    SECTION("and a bitmap cursor is not a standard one")
    {
        pWindow->set_cursor(window::standard_cursor_graphic::hand);
        pWindow->set_cursor(window::cursor_image_type{});

        REQUIRE_FALSE(pNull->last_standard_cursor().has_value());
    }

    SECTION("icons arrive as given")
    {
        window::icon_image_collection_type icons;

        icons.push_back(window::icon_image_type{2, 2, std::vector<std::byte>(2 * 2 * 4)});
        icons.push_back(window::icon_image_type{4, 4, std::vector<std::byte>(4 * 4 * 4)});

        pWindow->set_icons(icons);

        REQUIRE(pNull->icon_count() == 2);
    }
}

TEST_CASE("deleting through the interface is defined", "[window]")
{
    static_assert(std::has_virtual_destructor<window>::value,
        "deleting a window through the interface must reach the implementation's destructor");

    window *pRaw = new impl_null_window(*impl_null_window::make("Owned"));

    delete pRaw;

    SUCCEED("destroyed through the base");
}

TEST_CASE("the library has one exception type at its root", "[window]")
{
    REQUIRE(std::string(exception("something went wrong").what()) == "something went wrong");

    try { throw exception("from a window"); }
    catch (const std::exception &e) { REQUIRE(std::string(e.what()) == "from a window"); }
}

TEST_CASE("presentation pacing is the caller's decision", "[window]")
{
    SECTION("adaptive unless asked otherwise, which is what it always used to do")
    {
        REQUIRE(impl_null_window::make()->vsync() == vsync_mode::adaptive);
    }

    SECTION("and any of the three can be asked for")
    {
        REQUIRE(impl_null_window::make("w", {8, 8}, vsync_mode::off)->vsync() == vsync_mode::off);
        REQUIRE(impl_null_window::make("w", {8, 8}, vsync_mode::on)->vsync() == vsync_mode::on);
    }
}

TEST_CASE("the size is the caller's decision too", "[window]")
{
    SECTION("400x300 unless asked otherwise, which is what it always used to be")
    {
        REQUIRE(impl_null_window::make()->window_size() == window_size_type{400, 300});
    }

    SECTION("and any size can be asked for")
    {
        REQUIRE(impl_null_window::make("w", {1920, 1080})->window_size()
            == window_size_type{1920, 1080});
    }
}

TEST_CASE("a whole frame loop can be written without naming a backend", "[window][context]")
{
    const context_ptr_type pContext = impl_null_context::make();

    const window_ptr_type pWindow = pContext->make_window("Framed", {320, 240}, vsync_mode::off);

    REQUIRE(pWindow);
    REQUIRE(pWindow->name() == "Framed");
    REQUIRE(pWindow->window_size() == window_size_type{320, 240});

    int frames = 0;

    while (!pWindow->should_close()) {
        pContext->poll_events();

        pWindow->swap_buffers();

        if (++frames >= 10) pWindow->close();
    }

    REQUIRE(frames == 10);
}

TEST_CASE("the null context records what it was asked for", "[window][context]")
{
    auto pContext = impl_null_context::make();

    REQUIRE(pContext->poll_count() == 0);
    REQUIRE(pContext->window_count() == 0);

    const auto pFirst = pContext->make_window("one");
    const auto pSecond = pContext->make_window("two");

    pContext->poll_events();
    pContext->poll_events();
    pContext->poll_events();

    REQUIRE(pContext->window_count() == 2);
    REQUIRE(pContext->poll_count() == 3);
    REQUIRE(pFirst->name() == "one");
    REQUIRE(pSecond->name() == "two");
}
