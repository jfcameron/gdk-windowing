// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/windowing/context.h>
#include <gdk/windowing/window.h>

using namespace gdk::windowing;

TEST_CASE("conformance: a context makes a window and pumps its events", "[conformance][context]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = pContext->make_window("conformance", {320, 240}, vsync_mode::off);

    REQUIRE(pWindow);

    REQUIRE_NOTHROW(pContext->poll_events());
}

TEST_CASE("conformance: a whole frame loop runs through the interface", "[conformance][context]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();
    const auto pWindow = pContext->make_window("frames", {320, 240}, vsync_mode::off);

    int frames = 0;

    while (!pWindow->should_close()) {
        pContext->poll_events();

        pWindow->swap_buffers();

        if (++frames >= 10) pWindow->close();
    }

    REQUIRE(frames == 10);
}

TEST_CASE("conformance: every pacing mode can be asked for", "[conformance][context]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();

    for (const auto mode : {vsync_mode::off, vsync_mode::on, vsync_mode::adaptive}) {
        const auto pWindow = pContext->make_window("paced", {64, 64}, mode);

        REQUIRE(pWindow);

        pWindow->swap_buffers();
    }
}

TEST_CASE("conformance: a window can be dropped and another made", "[conformance][context]")
{
    INFO("implementation: " << conformance::implementation());

    const auto pContext = conformance::make_context();

    {
        const auto pFirst = pContext->make_window("first", {64, 64}, vsync_mode::off);

        pFirst->set_cursor(window::standard_cursor_graphic::hand);
        pFirst->swap_buffers();
    }

    const auto pSecond = pContext->make_window("second", {64, 64}, vsync_mode::off);

    REQUIRE(pSecond->name() == "second");

    pContext->poll_events();
    pSecond->swap_buffers();
}
