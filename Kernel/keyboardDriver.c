#include "include/scheduler.h"
#include "include/videoDriver.h"
#include <stdint.h>
#include <keyboardDriver.h>
#include <interrupts.h>
#include <videoDriver.h>

#define BUF_SIZE 55
#define CTRL 31

extern uint8_t keyPressed(void);

uint8_t getKey(uint8_t id);

typedef struct buf
{
    uint8_t keys[BUF_SIZE];
    uint8_t count;
} bufT;

bufT buf = {{0}, 0};
uint8_t shift = 0;
uint8_t ctrl = 0;

void saveKey(uint8_t c)
{
    if (c == 0b111000)
    {
        return;
    }
    if (c == 1)
    {
        buf.keys[buf.count++] = '\n';
        return;
    }
    if (c == 0x2A || c == 0x36)
    {
        shift = 1;
        return;
    }
    if (c == 0xAA || c == 0xB6)
    {
        shift = 0;
        return;
    }

    if (c == 0xE0 || c == 0x0)
    {
        ctrl = 0;
        return;
    }

    if (c == 0x1D)
    {
        ctrl = 1;
        return;
    }
    if (shift)
    {
        if (c == 0x8)
        {
            buf.keys[buf.count++] = '&';
            return;
        }
        if (c == 0x2B)
        {
            buf.keys[buf.count++] = '|';
            return;
        }
    }
    if (ctrl)
    {
        if (getKey(c) == 'c')
        {
            kill_foreground_proc();
            ready_foreground_proc();
            ctrl = 0;
            return;
        }
        if (getKey(c) == 'd')
        {
            buf.keys[buf.count++] = 4;
            ctrl = 0;
            return;
        }
    }
    if (c > 128)
        return;
    buf.keys[buf.count++] = getKey(c) + ('A' - 'a') * shift;
}

uint32_t readBuf(char *str, uint32_t count)
{
    _cli();
    int i = 0;
    while (i < buf.count && i < count)
    {
        str[i] = buf.keys[i];
        i++;
    }
    clearKeyboardBuffer();
    _sti();
    return i;
}

void clearKeyboardBuffer()
{
    buf.count = 0;
}

uint8_t getCount()
{
    return buf.count;
}

uint8_t getKey(uint8_t id)
{
    if (id >= 128)
        return -1;

    char kbd_US[128] =
        {
            0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
            '\t', /* <-- Tab */
            'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
            5, /* <-- control key */
            'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '^', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '^',
            '*',
            '^', /* Alt */
            ' ', /* Space bar */
            '^', /* Caps lock */
            '^', /* 59 - F1 key ... > */
            '^', '^', '^', '^', '^', '^', '^', '^',
            '^', /* < ... F10 */
            '^', /* 69 - Num lock*/
            '^', /* Scroll Lock */
            '^', /* Home key */
            3,   /* Up Arrow */
            '^', /* Page Up */
            '-',
            1, /* Left Arrow */
            '^',
            2, /* Right Arrow */
            '+',
            '^', /* 79 - End key*/
            4,   /* Down Arrow */
            '^', /* Page Down */
            '^', /* Insert Key */
            '^', /* Delete Key */
            '^', '^', '^',
            '^', /* F11 Key */
            '^', /* F12 Key */
            '^', /* All other keys are undefined */
        };

    return kbd_US[id];
}
