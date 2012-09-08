#include "ark.h"

/* declaration of bindable functions */
void
f_quit(){
	running = 0;
}

/* internal functions */
void
setup(){
}

void
teardown(){
}

int /* handle args, setup and then being mainloop */
main(int argc, char **argv){
	running = 1;

	setup();
	ui_setup();
	ui_mainloop();
	ui_teardown();
	teardown();

	return 0;
}
