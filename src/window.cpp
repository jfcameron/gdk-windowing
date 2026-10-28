// © Joseph Cameron - MIT License

#include <gdk/windowing/window.h>

#include <gdk/windowing/exception.h>

#include <string>

using namespace gdk::windowing;

void window::require_complete(const icon_image_collection_type &aIconImages) {
    for (const auto &iconImage : aIconImages) {
        const auto required = iconImage.width_pixels * iconImage.height_pixels
            * icon_image_type::CHANNEL_COUNT;

        if (iconImage.data_rgba32.size() < required)
            throw exception(std::string("gdk::windowing::window::set_icons: an icon says it is ")
                .append(std::to_string(iconImage.width_pixels)).append("x")
                .append(std::to_string(iconImage.height_pixels)).append(", which needs ")
                .append(std::to_string(required)).append(" bytes, but carries ")
                .append(std::to_string(iconImage.data_rgba32.size())));
    }
}
