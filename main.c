#include "llist.h"
#include "ark.h"
#include "ui.h"
#include <stdlib.h> /* free */

/* FIXME move main into a mian.c file, this can be the dirty glue */
int /* handle args, setup and then being mainloop */
main(int argc, char **argv){
	ui_window *uiw;

	if( argc <2 ){
		puts("No file provided, usage: ark filename");
		return 1;
	}

	Buffer *buf = newbuffer(argv[1]);
	if( ! buf ){ // FIXME error
		puts("Failed to allocate memory for buffer in main:main call to newbuffer");
		return 1;
	}

	load(buf); /* llist / buffer */
	m_startoffile(buf); /* llist / buffer */

	/* FIXME need to sort out isolation between ui, ark and llist. The below 2 lines are problematic (uiw isnt init) */
	uiw = ui_setup(buf);
	ui_mainloop(uiw);
	ui_teardown(uiw);

	free(buf);

	return 0;
}
