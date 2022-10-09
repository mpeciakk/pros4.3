#include <VGA.hpp>
#include <Lib/Log.hpp>

#define VGA_ADDRESS 0xC00B8000

VGA::VGA() : commandPort(0x3D4), dataPort(0x3D5) {
    width = 80;
    height = 25;
    x = 0;
    y = 0;

    color = vgaColor(textColor, backgroundColor);
    memory = (u16*) VGA_ADDRESS;

    klog(3, "VGA: Located at 0x%x", (u32) VGA_ADDRESS);
}

void VGA::clear() {
    for (u32 i = 0; i < height; i++) {
        for (u32 j = 0; j < width; j++) {
            memory[i * width + j] = vgaEntry(' ', color);
        }
    }
}

void VGA::putChar(char ch) {
    if (ch == '\n') {
        x = 0;
        y++;
    }

    if (ch >= ' ') {
        memory[y * width + x] = vgaEntry(ch, color);

        x++;
    }

    if (x >= width) {
        x = 0;
        y++;
    }

    scroll();
    updateCursor();
}

void VGA::scroll() {
    if (y >= height) {
        for (auto i = 0; i < width * height; i++) {
            if ((i - width) < 0) {
                continue;
            }

            memory[i - width] = memory[i];
        }

        for (auto i = 0; i < width; i++) {
            memory[(height - 1) * width + i] = vgaEntry(' ', color);
        }

        y--;
        x = 0;
    }
}

void VGA::updateCursor() {
    auto pos = y * width + x;

    commandPort.write(0x0F);
    dataPort.write(pos & 0xFF);
    commandPort.write(0x0E);
    dataPort.write((pos >> 8) & 0xFF);
}

void VGA::setTextColor(u8 color) {
    textColor = color;
    this->color = vgaColor(textColor, backgroundColor);
}

void VGA::setBackgroundColor(u8 color) {
    backgroundColor = color;
    this->color = vgaColor(textColor, backgroundColor);
}
