// © Joseph Cameron - All Rights Reserved

#ifndef GDK_WINDOWING_WINDOW_H
#define GDK_WINDOWING_WINDOW_H

#include <gdk/windowing/types.h>

#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace gdk::windowing {
    /// \brief a place on the screen that a renderer can draw into, and that the user can interact with
    class window {
    public:
        using cursor_image_type = std::array<unsigned char, 16 * 16 * 4>;

        //! POD structure for holding icon image data. RGBA32 format of userdefined dimensions
        struct icon_image_type {
            size_t width_pixels; //!< number of pixels wide
            size_t height_pixels; //!< number of pixels tall

            std::vector<std::byte> data_rgba32; //!< raw byte data
            static constexpr size_t CHANNEL_COUNT = 4; //!< number of channels per pixel
        };

        //! alias for collection of icon images.
        using icon_image_collection_type = std::vector<icon_image_type>;

        //! cursor graphics that are provided by the system.
        enum class standard_cursor_graphic {
            arrow, //!< regular arrow cursor
            ibeam, //!< text input I-beam cursor shape
            crosshair, //!< cross shaped cursor
            hand, //!< indexing finger
            horizontal_resizer, //!< cursor used to indicate some GUI element is being horizontally resized
            vertical_resizer //!< cursor used to indicate some GUI element is being vertically resized
        };

        //! provide a collection of icons of userdefined sizes, to be used by the system to render program icons where required.
        /// The icon whose dimensions best fit the system's requirements for some specific use case (window frame, 
        /// item in a taskbar, etc.) will be used for that purpose. 
        /// It is best practice to provide images of the same subject at a variety of resolutions, to minimize the undesirable 
        /// visual effects of a minifying or maximizing function (non-uniform column widths and or row heights, blurring).
        /// Common icon sizes are 16x16, 32x32, 48x48.
        /// Data format is RGBA 32bit: 4 channels, 1 byte per channel.
        /// \warning does not affect MacOS. Runtime icon is determined by icons provided in the application bundle. This must be handled at bundle creation time.
        virtual void set_icons(const icon_image_collection_type &aIconImages) = 0;

        //! sets the window's cursor graphic from a 16x16 RGBA32 bitmap
        virtual void set_cursor(const cursor_image_type &aRGBA32PNG) = 0;

        //! set the window's cursor graphic from a standard graphic provided by the system
        virtual void set_cursor(const standard_cursor_graphic cursor) = 0;

        //! sets the should close flag to true
        virtual void close() = 0;

        /// \brief clears the should close flag
        virtual void keep_open() = 0;

        //! present whatever has been drawn since the last call
        /// \attention must be called to see an updated image. usually done once per frame
        virtual void swap_buffers() = 0;

        //! get the name of the window
        [[nodiscard]] virtual std::string_view name() const = 0; 

        //! Get size in pixels of the window
        [[nodiscard]] virtual window_size_type window_size() const = 0;

        //! Get the aspect ratio of the window
        [[nodiscard]] virtual window_aspect_ratio_type aspect_ratio() const = 0;

        //! Indicates to the caller than the window has been requested to be closed
        [[nodiscard]] virtual bool should_close() const = 0;

        /// There is deliberately no cursor position here. Reading the user is gdk-input's domain, and
        /// it already offers the same value -- `input_context::mouse_cursor_position`, normalised the
        /// same way. This interface carries what a renderer needs from a surface.
        /// \see ROADMAP_GDK_WINDOW.md §6b

        /// Present so that deleting through a pointer to this interface is defined. An abstract base
        /// without one is a leak waiting for the first caller who holds the base type.
        virtual ~window() = default;
    };
}

#endif
