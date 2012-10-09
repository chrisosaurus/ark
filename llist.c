#include <stdio.h> /* perror */
#include <stdlib.h> /* malloc, realloc */
#include <string.h> /* strlen, memmove */

#include "llist.h"
#include "config.h"

#define BUFSIZE 80

static int /* return 1 if item breaks a word, otherwise return 0 */
wordbreak(char ch){
	switch(ch){
		case ' ':
			return 1;
			break;
		case '\n':
			return 1;
			break;
		case '\t':
			return 1;
			break;
		case '\0':
			return 1;
			break;
		default:
			return 0;
			break;
	}
}

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
	b->sstart = b->start = 0;
	b->send = b->end = 0;
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
	if( --buf->cursor.offset < 0 ){
		if( buf->cursor.line->prev ){
			buf->cursor.line = buf->cursor.line->prev;
			buf->cursor.offset = buf->cursor.line->len;
		} else {
			buf->cursor.offset = 0;
		}
	}
}

void
m_nextchar(Buffer *buf){
	if( ++buf->cursor.offset > buf->cursor.line->len ){
		if( buf->cursor.line->next ){
			buf->cursor.line = buf->cursor.line->next;
			buf->cursor.offset = 0;
		} else {
			buf->cursor.offset = buf->cursor.line->len;
		}
	}
}

void
m_prevline(Buffer *buf){
	if( ! buf->cursor.line->prev )
		return;
	int vo = i_to_vo(buf->cursor.line, buf->cursor.offset);
	buf->cursor.line = buf->cursor.line->prev;
	int i = vo_to_i(buf->cursor.line, vo);
	buf->cursor.offset = i;
}

void
m_nextline(Buffer *buf){
	if( ! buf->cursor.line->next )
		return;
	int vo = i_to_vo(buf->cursor.line, buf->cursor.offset);
	buf->cursor.line = buf->cursor.line->next;
	int i = vo_to_i(buf->cursor.line, vo);
	buf->cursor.offset = i;
}

void
m_prevword(Buffer *buf){
	for( m_prevchar(buf); ; m_prevchar(buf) ){
		if( wordbreak( buf->cursor.line->contents[buf->cursor.offset] ) )
			break;
		if( buf->cursor.offset == 0 )
			break;
	}
}

void
m_nextword(Buffer *buf){
	for( m_nextchar(buf); ; m_nextchar(buf) ){
		if( wordbreak( buf->cursor.line->contents[buf->cursor.offset] ) )
			break;
		if( buf->cursor.offset == 0 )
			break;
	}
}

void
m_scrolldown(Buffer *buf){
	int i;
	for( i=0; i<scrolldistance; ++i ){
		if( buf->sstart->next )
			buf->sstart = buf->sstart->next;
		else
			break;
	}
}

void
m_scrollup(Buffer *buf){
	int i;
	for( i=0; i<scrolldistance; ++i ){
		if( buf->sstart->prev )
			buf->sstart = buf->sstart->prev;
		else
			break;
	}

}
/** llist functions **/
int
vo_to_i(Line *l, int voffset){
	int i, vo;
	if( !l )
		return -1;

	/* find appropriate i for voffset */
	for( i=0, vo=0; vo < voffset; ++i ){
		if( i >= l->len )
			break; /* if we are off the end of the line, then return the end of the line */
		if( l->contents[i] == '\t' )
			vo += tabwidth;
		else
			++vo;
	}
	return i;
}

int
i_to_vo(Line *l, int offset){
	int vo;
	if( !l )
		return -1;

	/* find highest offset within line */
	for( ; offset > l->len; --offset ) ;

	/* find appropriate voffset for offset */
	for( vo=0; offset > 0; --offset ){
		if( l->contents[offset-1] == '\t' )
			vo += tabwidth;
		else
			++vo;
	}
	return vo;
}

void /* move cursor to linenum, voffset from current line */
position_cursor(Buffer *buf, int linenum, int voffset){
	Line *l;
	if( ! buf->sstart )
		return;

	for( l=buf->sstart; linenum && l; --linenum, l=l->next ) ;

	if( !l )
		return;

	buf->cursor.line = l;
	buf->cursor.offset = vo_to_i(l, voffset);
}

void
backspace(Buffer *buf){
	buf->modified = 1;
	if( buf->cursor.offset < 1 ){
		/* FIXME TODO deal with case of backspacing over lines */
		return;
	}
	int len = buf->cursor.line->len - buf->cursor.offset + 1;
	memmove( &buf->cursor.line->contents[buf->cursor.offset-1], &buf->cursor.line->contents[buf->cursor.offset], len);
	--buf->cursor.line->len;
	--buf->cursor.offset;
}

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
		buf->sstart = buf->start = newline(1, 0, 0);
		buf->send = buf->end = buf->start;
		buf->cursor = (Pos){buf->start, 0};
	}

	while( (read = fread(tmp, sizeof(char), BUFSIZE, f)) ){
		if( read == EOF )
			break;
		tmp[read] = '\0';
		ret = insert( buf, tmp);
		if( ret )
			return ret;
	}
	fclose(f);
	buf->modified = 0;
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
		if( l->next ) /* only append newline if there is a line following */
			fwrite("\n", sizeof(char), 1, f);
	}
	fclose(f);
	return 0;
}

int /* insert at cursor and move cursor along, returns 0 on success and 1 on error */
insert(Buffer *buf, const char *str){
	/* inserts character by character, performs far too many memmoves but is simple */
	buf->modified = 1;

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
		/* handle newline in string, should not be inserted */
		if( str[i] == '\n' || str[i] == '\r' ){
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
			ret = insert( buf, &ol->contents[oo]);
			if( ret )
				return 1;
			/* add a null character */
			ol->contents[oo] = 0;
			ol->len = oo;
			/* carry on from the start of the next line */
			buf->cursor = (Pos){nl, 0};
		} else {

			/* move old contents out of the way and insert character */
			memmove( &buf->cursor.line->contents[buf->cursor.offset+1], & buf->cursor.line->contents[buf->cursor.offset], buf->cursor.line->len - buf->cursor.offset + 1 );

			buf->cursor.line->contents[buf->cursor.offset] = str[i];
			++buf->cursor.line->len;
			++buf->cursor.offset;
		}
	}
	return 0;
}

