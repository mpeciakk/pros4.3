#ifndef __VGA_HPP
#define __VGA_HPP

#include <Hardware/Port.hpp>
#include <Lib/Types.hpp>

#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

class VGA {
public:
    ~VGA() = default;

    static VGA& instance() {
        static VGA vga;
        return vga;
    }

    void clear();
    void putChar(char ch);

    void scroll();
    void updateCursor();

    void setTextColor(u8 color);
    void setBackgroundColor(u8 color);

    static inline u8 vgaColor(u8 foreground, u8 background) {
        return foreground | background << 4;
    }

    static inline u16 vgaEntry(char ch, u8 color) {
        return (u16) ch | (u16) color << 8;
    }

private:
    VGA();

    u8 width;
    u8 height;
    u8 x;
    u8 y;
    u8 textColor = VGA_COLOR_LIGHT_GREY;
    u8 backgroundColor = VGA_COLOR_BLACK;
    u8 color;

    u16* memory;

    Port8Bit commandPort;
    Port8Bit dataPort;
};

#endif
