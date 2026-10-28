// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/windowing/exception.h>
#include <gdk/windowing/null_context.h>
#include <gdk/windowing/null_window.h>

#include <string>
#include <vector>

using namespace gdk::windowing;

TEST_CASE("a null window reports the size it was made with", "[null]")
{
    SECTION("400x300 unless asked otherwise")
    {
        REQUIRE(null_window::make()->window_size() == window_size_type{400, 300});
    }

    SECTION("and any size can be asked for")
    {
        REQUIRE(null_window::make("w", {1920, 1080})->window_size() == window_size_type{1920, 1080});
    }
}

TEST_CASE("a null window's aspect ratio follows its size", "[null]")
{
    auto pWindow = null_window::make("Sized", {800, 400});

    REQUIRE(pWindow->aspect_ratio() == Approx(2.0));

    SECTION("through a resize")
    {
        pWindow->set_window_size({400, 800});

        REQUIRE(pWindow->aspect_ratio() == Approx(0.5));
        REQUIRE(pWindow->window_size() == window_size_type{400, 800});
    }

    SECTION("and a window with no height has a defined answer rather than a divide by zero")
    {
        pWindow->set_window_size({0, 0});

        REQUIRE(pWindow->aspect_ratio() == 0);
    }
}

TEST_CASE("the aspect ratio rule has one definition", "[types]")
{
    static_assert(aspect_ratio_of({800, 400}) == 2.0, "width over height");
    static_assert(aspect_ratio_of({400, 800}) == 0.5, "and the other way up");
    static_assert(aspect_ratio_of({0, 0}) == 0, "a minimised window is defined, not undefined");
    static_assert(aspect_ratio_of({640, 0}) == 0, "no height is the case that would divide by zero");

    REQUIRE(aspect_ratio_of({512, 512}) == Approx(1.0));
    REQUIRE(aspect_ratio_of({0, 480}) == 0);
}

TEST_CASE("a null window records what it was asked", "[null]")
{
    auto pNull = null_window::make();

    const window_ptr_type pWindow = pNull;

    SECTION("each presentation")
    {
        pWindow->swap_buffers();
        pWindow->swap_buffers();

        REQUIRE(pNull->swap_count() == 2);
    }

    SECTION("a standard cursor, as one")
    {
        pWindow->set_cursor(window::standard_cursor_graphic::crosshair);

        REQUIRE(pNull->last_standard_cursor() == window::standard_cursor_graphic::crosshair);
    }

    SECTION("and a bitmap cursor, as not a standard one")
    {
        pWindow->set_cursor(window::standard_cursor_graphic::hand);
        pWindow->set_cursor(window::cursor_image_type{});

        REQUIRE_FALSE(pNull->last_standard_cursor().has_value());
    }

    SECTION("the icons it was given")
    {
        window::icon_image_collection_type icons;

        icons.push_back(window::icon_image_type{2, 2, std::vector<std::byte>(2 * 2 * 4)});
        icons.push_back(window::icon_image_type{4, 4, std::vector<std::byte>(4 * 4 * 4)});

        pWindow->set_icons(icons);

        REQUIRE(pNull->icon_count() == 2);
    }

    SECTION("its pacing, adaptive unless asked otherwise")
    {
        REQUIRE(pNull->vsync() == vsync_mode::adaptive);
        REQUIRE(null_window::make("w", {8, 8}, vsync_mode::off)->vsync() == vsync_mode::off);
        REQUIRE(null_window::make("w", {8, 8}, vsync_mode::on)->vsync() == vsync_mode::on);
    }
}

TEST_CASE("the null context records what it was asked for", "[null][context]")
{
    auto pContext = null_context::make();

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

TEST_CASE("the library has one exception type at its root", "[exception]")
{
    REQUIRE(std::string(exception("something went wrong").what()) == "something went wrong");
    REQUIRE(std::string(exception().what()) == "gdk::windowing::exception");

    try { throw exception("from a window"); }
    catch (const std::exception &e) { REQUIRE(std::string(e.what()) == "from a window"); }
}
