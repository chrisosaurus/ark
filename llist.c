#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, realloc */
#include <string.h> /* strlen, memmove */

#include "llist.h"

#define BUFSIZE 80

/* state */
Pos fstart={0,0}, fend={0,0};

/* management of Line(s) */
/* where n_elems is the number of chars in the line */
Line*
newline(int mul, Line *prev, Line *next){
	Line *l = malloc(sizeof(Line));
	if( !l ){
		perror("Failed to malloc in llist:newline for ");
		return 0; /* error */
	}
	l->prev = prev;
	l->next = next;
	l->mul = mul;
	l->len = 0;
	l->contents = malloc( sizeof(char) * LINESIZE * mul );
	if( ! l->contents ){
		perror("Failed to malloc in llist:newline for l->contents");
		return 0; /* error */
	}
	l->contents[0] = 0;
	if( ! fstart.line ){
		fstart.line = l;
		fend.line = l;
	}
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
	return fstart;
}

Pos
m_endoffile(Pos pos){
	return fend;
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
Pos
load(Pos pos, FILE *f){
	int read;
	/* +1 as we need a \0, [read+1] will be \0 */
	char buf[BUFSIZE+1];

	while( (read = fread(buf, sizeof(char), BUFSIZE, f)) ){
		if( read == EOF ){
			printf("EOF\n");
			return pos;
		}
		buf[read] = '\0';
		pos = insert( pos, buf );
	}
	return pos;
}

void
save(Pos from, Pos to, FILE *f){
}

Pos
insert(Pos pos, const char *str){
	/* inserts character by character, performs far too many memmoves but is simple */

	int i=0, l=strlen(str);
	for( i=0; i<l; ++i ){
		/* handle resizing */
		if( pos.line->len + 2 > pos.line->mul * LINESIZE ){
			pos.line->contents = realloc(pos.line->contents, ++pos.line->mul * LINESIZE);
			if( ! pos.line->contents ){
				perror("Failed realloc in llist:insert");
				return (Pos){0,0}; /* FIXME error */
			}
		}

		/* move old contents out of the way and insert character */
		memmove( &pos.line->contents[pos.offset+1], & pos.line->contents[pos.offset], pos.line->len - pos.offset + 1 );


		pos.line->contents[pos.offset] = str[i];
		++pos.line->len;
		++pos.offset;

		/* handle \n in str */
		if( str[i] == '\n' ){
			Line *nl = newline(1, pos.line, pos.line->next);
			if( ! nl ){
				perror("Failed call to newline from llist:insert");
				return (Pos){0,0}; /* FIXME error */
			}
			if( pos.line->next )
				pos.line->next->prev = nl;
			pos.line->next = nl;

			/* copy over old contents */
			insert( (Pos){nl, 0}, &pos.line->contents[pos.offset] );
			/* add a null character */
			pos.line->contents[pos.offset] = 0;
			/* carry on from the start of the next line */
			pos = (Pos){nl, 0};
			/* if the previous line was fend, update fend */
			if( pos.line->prev == fend.line )
				fend = pos;
		}
	}
	return pos;
}
