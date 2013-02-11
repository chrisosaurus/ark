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
f_save(ui_window *uiw, const Arg *arg){
	if( ! uiw->buf )
		return;
	uiw->buf->mod = 0;
	/* FIXME need to deal with return code */
	save(uiw->buf);
}

