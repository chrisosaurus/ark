#include <stdlib.h>
#include "ark.h"
#include "ui.h"
#include "llist.h"

/* buffer shared with ui, owned by ark */
static Buffer *buf;

/* declaration of bindable functions */
void
f_quit(const Arg *arg){
	if( !arg->i && buf && buf->modified )
		return;
	ui_stop();
}

void /* execute function in arg->f */
f_func(const Arg *arg){
	if( arg->f )
		(arg->f)(buf);
}

void /* save */
f_save(const Arg*arg){
	if( ! buf )
		return;
	buf->modified = 0;
	/* FIXME need to deal with return code */
	save(buf);
}

/* internal functions */
static void
setup(char *file){
	buf = newbuffer(file);
	load(buf);
	m_startoffile(buf);
}

static void
teardown(){
	free(buf);
}

int /* handle args, setup and then being mainloop */
main(int argc, char **argv){

	if( argc <2 ){
		puts("No file provided, usage: ark filename");
		return 1;
	}
	setup(argv[1]);
	ui_setup(buf);
	ui_mainloop();
	ui_teardown();
	teardown();

	return 0;
}
