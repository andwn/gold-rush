#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

// Memory stuff

void mem_init();
uint16_t mem_avail();

void* malloc(uint16_t size);
void free(void *ptr);

void memset(void *to, uint8_t value, uint16_t len);
void memcpy(void *to, const void *from, uint16_t len);

// String stuff

#define isdigit(c) ((c) >= '0' && (c) <= '9')

typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG)                                           \
 (AP = ((__gnuc_va_list) __builtin_next_arg (LASTARG)))

#define va_end(AP)      ((void)0)

#define va_arg(AP, TYPE)                                                \
 (AP = (__gnuc_va_list) ((char *) (AP) + __va_rounded_size (TYPE)),     \
  *((TYPE *) (void *) ((char *) (AP)                                    \
                       - ((sizeof (TYPE) < __va_rounded_size (char)     \
                           ? sizeof (TYPE) : __va_rounded_size (TYPE))))))

uint16_t strlen(const char *str);
uint16_t strnlen(const char *str, uint16_t maxlen);

uint16_t sprintf(char *buffer, const char *fmt, ...);
