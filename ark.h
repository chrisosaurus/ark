#ifndef ark_h
#define ark_h

#include "ui.h"

/** bindable Functions **/
void f_quit(const Arg *arg); /* if arg->i then will quit regardless of changes, otherwise will not quit until changes are saved */
void f_func(const Arg *arg); /* execute function in arg->f */
void f_save(const Arg *arg);

#endif
