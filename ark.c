#include "ark.h"

/* declaration of bindable functions */
void
f_quit(){
	ui_stop();
}

/* internal functions */
static void
setup(){
}

static void
teardown(){
}

int /* handle args, setup and then being mainloop */
main(int argc, char **argv){

	setup();
	ui_setup();
	ui_mainloop();
	ui_teardown();
	teardown();

	return 0;
}
