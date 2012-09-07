#include <stdlib.h> /* malloc family */
#include <string.h> /* strlen */
#include <stdio.h> /* printf, puts */
#include <assert.h> /* assert */

#include <X11/Xlib.h>
#include <X11/keysym.h> /* XK_*, used in config.h */

/* Argument for functions bound to key presses */
typedef union {
	int i; /* FIXME what do */
} Arg;

/* key bindings defined in config.h */
typedef struct {
	unsigned int mod; /* modifiers */
	KeySym keysym; /* key */
	void (*func)(const Arg *); /* function to call */
	const Arg arg; /* arg to pass to function */
} Key;

#include "config.h" /* include our configuration */

int
main(){
}
