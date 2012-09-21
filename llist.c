#include <stdlib.h>

#include "llist.h"

/* state */
Pos fstart={0,0}, fend={0,0};

/* management of Line(s) */
/* where n_elems is the number of chars in the line */
Line*
newline(int mul, Line *prev, Line *next){
	Line *l = malloc(sizeof(Line));
	if( !l )
		return 0; /* error */
	l->prev = 0;
	l->next = 0;
	l->mul = mul;
	l->len = 0;
	l->contents = malloc( sizeof(char) * LINESIZE * mul );
	if( ! l->contents )
		return 0; /* error */
	return l;
}

/* Movement functions */
Pos
m_startofline(Pos pos){
	return (Pos){0,0};
}

Pos
m_endofline(Pos pos){
	return (Pos){0,0};
}

Pos
m_startoffile(Pos pos){
	return (Pos){0,0};
}

Pos
m_endoffile(Pos pos){
	return (Pos){0,0};
}

Pos
m_prevchar(Pos pos){
	return (Pos){0,0};
}

Pos
m_nextchar(Pos pos){
	return (Pos){0,0};
}

Pos
m_prevline(Pos pos){
	return (Pos){0,0};
}

Pos
m_nextline(Pos pos){
	return (Pos){0,0};
}

Pos
m_prevword(Pos pos){
	return (Pos){0,0};
}

Pos
m_nextword(Pos pos){
	return (Pos){0,0};
}

/** llist functions **/
void
load(FILE *f){
}

void
save(FILE *f){
}

Pos
insert(Pos pos, const char *str){
}
