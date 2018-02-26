#define BTN_UP       0x0001
#define BTN_DOWN     0x0002
#define BTN_LEFT     0x0004
#define BTN_RIGHT    0x0008
#define BTN_A        0x0040
#define BTN_B        0x0010
#define BTN_C        0x0020
#define BTN_START    0x0080
#define BTN_X        0x0400
#define BTN_Y        0x0200
#define BTN_Z        0x0100
#define BTN_MODE     0x0800

#define BTN_DIR      0x000F
#define BTN_ABC      0x0070
#define BTN_XYZ      0x0700
#define BTN_ALL      0x0FFF

uint16_t joystate, oldstate;

#define joy_down(b)    (joystate & (b))
#define joy_press(b)   ((joystate & (b)) && !(oldstate & (b)))
#define joy_release(b) (!(joystate & (b)) && (oldstate & (b)))

void joy_init();
void joy_update();
