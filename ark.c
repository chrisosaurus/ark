#include <stdlib.h>
#include "ark.h"

/* buffer shared with ui, owned by ark */
static Buffer *buf;

/* declaration of bindable functions */
void
f_quit(){
	ui_stop();
}

/* internal functions */
static void
setup(){
	buf = newbuffer("llist.c");
	load(buf);
	m_startoffile(buf);
}

static void
teardown(){
	free(buf);
}

int /* handle args, setup and then being mainloop */
main(int argc, char **argv){

	setup();
	ui_setup(buf);
	ui_mainloop();
	ui_teardown();
	teardown();

	return 0;
}
