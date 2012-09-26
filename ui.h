#ifndef ui_h
#define ui_h

#include <stdio.h>

/* should the X includes be here or within ark.c ? */
#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_* */

#include "llist.h"

typedef union Arg Arg;
union Arg{
	int i;
	void *v;
	void (*m)(Buffer*);
};

typedef struct Key Key;
struct Key{ /* structure used in config.h to bind a function to a key */
	int mods; /* modifiers */
	KeySym keysym; /* keysym to bind to */
	void (*f_func) (const Arg *arg); /* function to call */
	const Arg arg; /* argument to pass */
};

/* calling this will cause ui_mainloop to return */
void ui_stop();

/* initialise */
void ui_setup(Buffer *buffer);

/* tidyup */
void ui_teardown();

/* handles XEvents and dispatches to handlers */
void ui_mainloop();

#endif
