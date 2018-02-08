#include "common.h"
#include "gamemode.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"
#include "xgm.h"

int main() {
	mem_init();
	vdp_init();
	vdp_reset();
	joy_init();
	xgm_init();
	
	gamemode = GM_SPLASH;
	while(TRUE) {
		switch(gamemode) {
			case GM_SPLASH: splash(); break;
			case GM_GAME:   game();   break;
			case GM_RESULT: result(); break;
		}
	}
}
