#include <Lib/String.hpp>

u32 strlen(const char* str) {
    u32 len = 0;

    while (str[len]) {
        len++;
    }

    return len;
}

void* memset(void* dest, char val, u32 count) {
    u8* temp = (u8*) dest;
    for (; count != 0; count--) {
        temp[count] = val;
    }

    return dest;
}

void memcpy(void* dest, void* src, u32 size) {
    char* csrc = (char*) src;
    char* cdest = (char*) dest;

    for (u32 i = 0; i < size; i++) {
        cdest[i] = csrc[i];
    }
}

int strcmp(const char* a, const char* b) {
    while (*a) {
        if (*a != *b) {
            break;
        }

        a++;
        b++;
    }

    return *(const unsigned char*) a - *(const unsigned char*) b;
}

char* strcpy(char* strDest, const char* strSrc) {
    char* temp = strDest;
    while ((*strDest++ = *strSrc++) != '\0') {
    }
    return temp;
}

u32 countDigit(u32 n, u32 base) {
    if (n == 0) {
        return 1;
    }

    int count = 0;
    while (n != 0) {
        n = n / base;
        ++count;
    }
    return count;
}

u32 abs(int n) {
    return (n >= 0) ? n : -n;
}

void swap(char* x, char* y) {
    char t = *x;
    *x = *y;
    *y = t;
}

char* reverse(char* buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

char* itos(int value, char* buffer, int base) {
    // Only when heap is uninitialized
    // TODO: write better way with heap initialized
    if (base < 2 || base > 32) {
        return buffer;
    }

    u32 n = abs(value);

    u32 i = 0;
    while (n) {
        u32 r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        } else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    if (i == 0) {
        buffer[i++] = '0';
    }

    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    return reverse(buffer, 0, i - 1);
}