#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, realloc */
#include <string.h> /* strlen, memmove */

#include "llist.h"

#define BUFSIZE 80

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
	return l;
}

Buffer*
newbuffer(char *path){
	Buffer *b = malloc(sizeof(Buffer));
	if( !b ){
		perror("Failed to ammloc in llist:newfile for ");
		return 0; /* error */
	}
	b->start = 0;
	b->end = 0;
	b->path = path;
	return b;
}

/* Movement functions */
void
m_startofline(Buffer *buf){
	buf->cursor.offset = 0;
}

void
m_endofline(Buffer *buf){
	/* cursor if where to insert next, end of line is after last character in line */
	/* this SHOULDNt actually be safe though as last char may be \n... FIXME */
	buf->cursor.offset = buf->cursor.line->len + 1;
}

void
m_startoffile(Buffer *buf){
	buf->cursor.line = buf->start;
	buf->cursor.offset = 0;
}

void
m_endoffile(Buffer *buf){
	buf->cursor.line = buf->end;
	buf->cursor.offset = buf->start->len;
}

void
m_prevchar(Buffer *buf){
}

void
m_nextchar(Buffer *buf){
}

void
m_prevline(Buffer *buf){
	buf->cursor.line = buf->cursor.line->prev;
	buf->cursor.offset = 0;
}

void
m_nextline(Buffer *buf){
	buf->cursor.line = buf->cursor.line->next;
	buf->cursor.offset = 0;
}

void
m_prevword(Buffer *buf){
}

void
m_nextword(Buffer *buf){
}

/** llist functions **/
int /* load file buf->path into buf, returns 0 on success and 1 on error */
load(Buffer *buf){
	int read; /* returned by fread */
	char tmp[BUFSIZE+1]; /* for fread to read into, +1 as we need a \0, [read+1] will be \0 */
	int ret;
	FILE *f = fopen(buf->path, "r");

	if( !f ){
		perror("Failed to open buf->path in llist:load : ");
		return 1; /* error */
	}

	if( ! buf->start){
		buf->start = newline(1, 0, 0);
		buf->end = buf->start;
		buf->cursor = (Pos){buf->start, 0};
	}

	while( (read = fread(tmp, sizeof(char), BUFSIZE, f)) ){
		if( read == EOF )
			break;
		tmp[read] = '\0';
		ret = insert( buf, tmp );
		if( ret )
			return ret;
	}
	fclose(f);
	/* FIXME consider setting buf->end, I think insert should do this*/
	return 0;
}

int /* save buf into file buf->path, returns 0 on success and 1 on error */
save(Buffer *buf){
	Line *l;
	FILE *f = fopen(buf->path, "w");
	if( !f ){
		perror("Failed to open buf->path in llist:save : ");
		return 1; /* error */
	}
	for( l=buf->start ; l; l=l->next ){
		fwrite(l->contents, sizeof(char), l->len, f);
	}
	fclose(f);
	return 0;
}

int /* insert at cursor and move cursor along, returns 0 on success and 1 on error */
insert(Buffer *buf, const char *str){
	/* inserts character by character, performs far too many memmoves but is simple */

	int i=0, l=strlen(str);
	int ret; /* return value from insert */
	int oo; /* old offset */
	Line *ol, *nl; /* old line, new line */

	for( i=0; i<l; ++i ){
		/* handle resizing */
		if( buf->cursor.line->len + 2 > buf->cursor.line->mul * LINESIZE ){
			buf->cursor.line->contents = realloc(buf->cursor.line->contents, ++buf->cursor.line->mul * LINESIZE);
			if( ! buf->cursor.line->contents ){
				perror("Failed realloc in llist:insert");
				return 1; /* FIXME error */
			}
		}

		/* move old contents out of the way and insert character */
		memmove( &buf->cursor.line->contents[buf->cursor.offset+1], & buf->cursor.line->contents[buf->cursor.offset], buf->cursor.line->len - buf->cursor.offset + 1 );

		buf->cursor.line->contents[buf->cursor.offset] = str[i];
		++buf->cursor.line->len;
		++buf->cursor.offset;

		/* handle \n in str */
		if( str[i] == '\n' ){
			nl = newline(1, buf->cursor.line, buf->cursor.line->next);
			if( ! nl ){
				perror("Failed call to newline from llist:insert");
				return 1; /* FIXME error */
			}
			if( buf->cursor.line->next )
				buf->cursor.line->next->prev = nl;
			buf->cursor.line->next = nl;

			/* old line */
			ol = buf->cursor.line;
			/* old offset */
			oo = buf->cursor.offset;

			/* move cursor to start of next line */
			buf->cursor = (Pos){nl, 0};
			/* copy over old contents */
 /* FIXME TODO this will insert the \n which will cause a trailing newline to be inserted, silly */
			ret = insert( buf, &ol->contents[oo] );
			if( ret )
				return 1;
			/* add a null character */
			ol->contents[oo] = 0;
			ol->len = oo;
			/* carry on from the start of the next line */
			buf->cursor = (Pos){nl, 0};
		}
	}
	return 0;
}
