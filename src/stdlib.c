#include "common.h"
#include "stdlib.h"

#define MEMORY_HIGH 0xFFA400
#define USED        1

// end of bss segment --> start of heap
extern uint32_t _end;

static uint16_t* s_free;
static uint16_t* s_heap;

static uint16_t* pack(uint16_t nsize) {
    uint16_t *b = s_heap;
    uint16_t *best = b;
    uint16_t bsize = 0;
    uint16_t psize;

    while ((psize = *b)) {
        if (psize & USED) {
            if (bsize != 0) {
                *best = bsize;
                if (bsize >= nsize) return best;
                bsize = 0;
            }
            b += psize >> 1;
            best = b;
        } else {
            bsize += psize;
            b += psize >> 1;
        }
    }

    if (bsize != 0) {
        *best = bsize;
        if (bsize >= nsize) return best;
    }

    return NULL;
}

void mem_init() {
    // point to end of bss (start of heap)
    uint32_t h = (uint32_t)&_end;
    // 2 bytes aligned
    h += 1;
    h &= ~1;
    // define available memory (sizeof(uint16_t) is the memory reserved to indicate heap end)
    uint32_t len = MEMORY_HIGH - (h + sizeof(uint16_t));
    // define heap
    s_heap = (uint16_t*) h;
    // and its size
    *s_heap = len;
    // free memory : whole heap
    s_free = s_heap;
    // mark end of heap memory
    s_heap[len >> 1] = 0;
}

uint16_t mem_avail() {
    uint16_t *b = s_heap;
    uint16_t res = 0;
    uint16_t bsize;

    while ((bsize = *b)) {
        // memory block not used --> add available size to result
        if (!(bsize & USED)) res += bsize;
        // pass to next block
        b += bsize >> 1;
    }

    return res;
}

void free(void *ptr) {
    // valid block --> mark block as no more used
    if (ptr) ((uint16_t*)ptr)[-1] &= ~USED;
}

void* malloc(uint16_t size) {
    if (size == 0) return NULL;
    
    // 2 bytes aligned
    uint16_t adjsize = (size + sizeof(uint16_t) + 1) & 0xFFFE;
    uint16_t* p;
    uint16_t remaining;
    
    if (adjsize > *s_free) {
        p = pack(adjsize);
        // no enough memory
        if (p == NULL) return NULL;
        s_free = p;
    } else {
        // at this point we can allocate memory
        p = s_free;
	}
    // set free to next free block
    s_free += adjsize >> 1;
    // get remaining (old - allocated)
    remaining = *p - adjsize;
    // adjust remaining free space
    if (remaining > 0) {
		*s_free = remaining;
    } else {
        // no more space in bloc so we have to find the next free bloc
        uint16_t *newfree = s_free;
        uint16_t bloc;
        while((bloc = *newfree) & USED) newfree += bloc >> 1;
        s_free = newfree;
    }
    // set block size, mark as used and point to free region
    *p++ = adjsize | USED;
    // return block
    return p;
}

// String stuff

static const char uppercase_hexchars[] = "0123456789ABCDEF";
static const char lowercase_hexchars[] = "0123456789abcdef";

uint16_t strlen(const char *str) {
	const char *src;
	src = str;
	while (*src++);
	return (src - str) - 1;
}

uint16_t strnlen(const char *str, uint16_t maxlen) {
	const char *src;
	for (src = str; maxlen-- && *src != '\0'; ++src) /* nothing */;
	return src - str;
}

static uint16_t skip_atoi(const char **s) {
    uint16_t i = 0;
    while(isdigit(**s))
        i = (i * 10) + *((*s)++) - '0';
    return i;
}

static uint16_t vsprintf(char *buf, const char *fmt, va_list args) {
    char tmp_buffer[12];
    int16_t i;
    int16_t len;
    int16_t *ip;
    uint16_t num;
    char *s;
    const char *hexchars;
    char *str;
    int16_t left_align;
    int16_t plus_sign;
    int16_t zero_pad;
    int16_t space_sign;
    int16_t field_width;
    int16_t precision;

    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        space_sign = zero_pad = plus_sign = left_align = 0;

        // Process the flags
repeat:
        ++fmt;          // this also skips first '%'

        switch (*fmt)
        {
        case '-':
            left_align = 1;
            goto repeat;

        case '+':
            plus_sign = 1;
            goto repeat;

        case ' ':
            if ( !plus_sign )
                space_sign = 1;

            goto repeat;

        case '0':
            zero_pad = 1;
            goto repeat;
        }

        // Process field width and precision

        field_width = precision = -1;

        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++fmt;
            // it's the next argument
            field_width = va_arg(args, s16);

            if (field_width < 0)
            {
                field_width = -field_width;
                left_align = 1;
            }
        }

        if (*fmt == '.')
        {
            ++fmt;

            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++fmt;
                // it's the next argument
                precision = va_arg(args, s16);
            }

            if (precision < 0)
                precision = 0;
        }

        if ((*fmt == 'h') || (*fmt == 'l') || (*fmt == 'L'))
            ++fmt;

        if (left_align)
            zero_pad = 0;

        switch (*fmt)
        {
        case 'c':
            if (!left_align)
                while(--field_width > 0)
                    *str++ = ' ';

            *str++ = (unsigned char) va_arg(args, s16);

            while(--field_width > 0)
                *str++ = ' ';

            continue;

        case 's':
            s = va_arg(args, char *);

            if (!s)
                s = "<NULL>";

            len = strnlen(s, precision);

            if (!left_align)
                while(len < field_width--)
                    *str++ = ' ';

            for (i = 0; i < len; ++i)
                *str++ = *s++;

            while(len < field_width--)
                *str++ = ' ';

            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void *);
                zero_pad = 1;
            }

            hexchars = uppercase_hexchars;
            goto hexa_conv;

        case 'x':
            hexchars = lowercase_hexchars;
            goto hexa_conv;

        case 'X':
            hexchars = uppercase_hexchars;

hexa_conv:
            s = &tmp_buffer[12];
            *--s = 0;
            num = va_arg(args, u16);

            if (!num)
                *--s = '0';

            while(num)
            {
                *--s = hexchars[num & 0xF];
                num >>= 4;
            }

            num = plus_sign = 0;

            break;

        case 'n':
            ip = va_arg(args, s16*);
            *ip = (str - buf);
            continue;

        case 'u':
            s = &tmp_buffer[12];
            *--s = 0;
            num = va_arg(args, u16);

            if (!num)
                *--s = '0';

            while(num)
            {
                *--s = (num % 10) + 0x30;
                num /= 10;
            }

            num = plus_sign = 0;

            break;

        case 'd':
        case 'i':
            s = &tmp_buffer[12];
            *--s = 0;
            i = va_arg(args, s16);

            if (!i)
                *--s = '0';

            if (i < 0)
            {
                num = 1;

                while(i)
                {
                    *--s = 0x30 - (i % 10);
                    i /= 10;
                }
            }
            else
            {
                num = 0;

                while(i)
                {
                    *--s = (i % 10) + 0x30;
                    i /= 10;
                }
            }

            break;

        default:
            continue;
        }

        len = strnlen(s, precision);

        if (num)
        {
            *str++ = '-';
            field_width--;
        }
        else if (plus_sign)
        {
            *str++ = '+';
            field_width--;
        }
        else if (space_sign)
        {
            *str++ = ' ';
            field_width--;
        }

        if ( !left_align)
        {
            if (zero_pad)
            {
                while(len < field_width--)
                    *str++ = '0';
            }
            else
            {
                while(len < field_width--)
                    *str++ = ' ';
            }
        }

        for (i = 0; i < len; ++i)
            *str++ = *s++;

        while(len < field_width--)
            *str++ = ' ';
    }

    *str = '\0';

    return str - buf;
}

uint16_t sprintf(char *buffer, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    uint16_t i = vsprintf(buffer, fmt, args);
    va_end(args);
    return i;
}
