#include <Hardware/Port.hpp>
#include <Lib/Log.hpp>
#include <Lib/String.hpp>
#include <Lib/Types.hpp>
#include <VGA.hpp>

void print(const char* data, u32 length) {
    for (u32 i = 0; i < length; i++) {
        VGA::instance().putChar(data[i]);
    }
}

void serialPrint(const char* data, u32 length) {
    auto* bytes = (u8*) data;
    Port8Bit port(0x3F8);

    for (u32 i = 0; i < length; i++) {
        port.write(bytes[i]);
    }
}

void printf(const char* __restrict format, va_list parameters, bool serial) {
    auto printFunction = serial ? serialPrint : print;

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }

            u32 amount = 1;

            while (format[amount] && format[amount] != '%') {
                amount++;
            }

            printFunction((char*) format, amount);
            format += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            printFunction(&c, sizeof(c));
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            u32 len = strlen(str);
            printFunction((char*) str, len);
        } else if (*format == 'd') {
            format++;
            int n = va_arg(parameters, int);
            int numChars = countDigit(n);

            char str[numChars];
            itos(n, str, 10);

            printFunction(str, numChars);
        } else if (*format == 'x') {
            format++;
            u32 n = va_arg(parameters, u32);
            int numChars = countDigit(n, 16);

            char str[numChars];
            itos(n, str, 16);

            printFunction(str, numChars);
        } else {
            format = format_begun_at;
            u32 len = strlen(format);
            printFunction((char*) format, len);
            format += len;
        }
    }
}

void log(int level, const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    switch (level) {
        case 0:
            printf("[", parameters);
            VGA::instance().setTextColor(VGA_COLOR_GREEN);
            printf(" LOG ", parameters);
            VGA::instance().setTextColor(VGA_COLOR_LIGHT_GREY);
            printf("] ", parameters);
            break;
        case 1:
            printf("[", parameters);
            VGA::instance().setTextColor(VGA_COLOR_LIGHT_RED);
            printf(" WARN ", parameters);
            VGA::instance().setTextColor(VGA_COLOR_LIGHT_GREY);
            printf("] ", parameters);
            break;
        case 2:
            printf("[", parameters);
            VGA::instance().setTextColor(VGA_COLOR_RED);
            printf(" ERR ", parameters);
            VGA::instance().setTextColor(VGA_COLOR_LIGHT_GREY);
            printf("] ", parameters);
            break;
        default:
            break;
    }

    printf(format, parameters);
    printf("\n", parameters);
    va_end(parameters);
}

void klog(int level, const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    switch (level) {
        case 0:
            printf("[\033[32mLOG\033[39m] ", parameters, true);
            break;
        case 1:
            printf("[\033[33mWARN\033[39m] ", parameters, true);
            break;
        case 2:
            printf("[\033[31mERR\033[39m] ", parameters, true);
            break;
        case 3:
            printf("[\033[36mDBG\033[39m] ", parameters, true);
            break;
        default:
            break;
    }

    printf(format, parameters, true);
    printf("\n", parameters, true);
    va_end(parameters);
}

[[noreturn]] void panic(const char* message) {
    klog(2, "Kernel panic!");
    klog(2, message);

    log(2, "Kernel panic!");
    log(2, message);

    while (true) {
    }
}