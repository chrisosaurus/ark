#include <stdlib.h>
#include "ark.h"
#include "ui.h"
#include "llist.h"

/* declaration of bindable functions */
void
f_quit(ui_window *uiw, const Arg *arg){
	if( !arg->i && uiw->buf && uiw->buf->mod )
		return;
	ui_stop(uiw);
}

void /* execute function in arg->f */
f_func(ui_window *uiw, const Arg *arg){
	if( arg->f )
		(arg->f)(uiw->buf);
}

void /* save */
f_save(ui_window *uiw, const Arg*arg){
	if( ! uiw->buf )
		return;
	uiw->buf->mod = 0;
	/* FIXME need to deal with return code */
	save(uiw->buf);
}

/* internal functions */
static void
setup(ui_window *uiw, char *file){
	uiw->buf = newbuffer(file);
	load(uiw->buf);
	m_startoffile(uiw->buf);
}

/* FIXME this could be an issue, need to work out who frees buffer (and who frees uiw, which has a pointer to buffer */
static void
teardown(ui_window *uiw){
	free(uiw->buf);
}

/* FIXME move main into a mian.c file, this can be the dirty glue */
int /* handle args, setup and then being mainloop */
main(int argc, char **argv){
	ui_window *uiw;

	if( argc <2 ){
		puts("No file provided, usage: ark filename");
		return 1;
	}
	/* FIXME need to sort out isolation between ui, ark and llist. The below 2 lines are problematic (uiw isnt init) */
	setup(uiw, argv[1]);
	uiw = ui_setup(uiw->buf);
	ui_mainloop(uiw);
	ui_teardown(uiw);
	teardown(uiw);

	return 0;
}
