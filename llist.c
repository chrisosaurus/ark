#include <stdlib.h>
#include <stdio.h> /* FIXME testing */
#include <string.h> /* strlen, memmove */

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
	l->prev = prev;
	l->next = next;
	l->mul = mul;
	l->len = 0;
	l->contents = malloc( sizeof(char) * LINESIZE * mul );
	if( ! l->contents )
		return 0; /* error */
	l->contents[0] = 0;
	return l;
}

/* Movement functions */
Pos
m_startofline(Pos pos){
	return (Pos){pos.line,0};
}

Pos
m_endofline(Pos pos){
	return (Pos){pos.line, pos.line->len + 1};
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
	return (Pos){pos.line->prev, 0};
}

Pos
m_nextline(Pos pos){
	return (Pos){pos.line->next, 0};
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
	int i=0, l=strlen(str);
	for( i=0; i<l; ++i ){
		/* handle resizing */
		if( pos.line->len + 2 > pos.line->mul * LINESIZE ){
			pos.line->contents = realloc(pos.line->contents, ++pos.line->mul * LINESIZE);
			if( ! pos.line->contents )
				return (Pos){0,0}; /* FIXME error */
		}

		memmove( &pos.line->contents[pos.offset+1], & pos.line->contents[pos.offset], pos.line->len - pos.offset + 1 );
		pos.line->contents[pos.offset] = str[i];
		++pos.line->len;
		++pos.offset;

		/* handle \n in str */
		if( str[i] == '\n' ){
			Line *nl = newline(1, pos.line, pos.line->next);
			if( ! nl )
				return (Pos){0,0}; /* FIXME error */
			pos.line->next = nl;

			/* copy over old contents */
			insert( (Pos){nl, 0}, &pos.line->contents[pos.offset] );
			/* add a null character */
			pos.line->contents[pos.offset] = 0;
			/* carry on from the start of the next line */
			pos = (Pos){nl, 0};
		}
	}
	return pos;
}
