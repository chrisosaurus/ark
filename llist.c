#include <stdio.h> /* perror */
#include <stdlib.h> /* calloc, realloc */
#include <string.h> /* strlen, memmove */

#include "llist.h"
#include "config.h"

#define BUFSIZE 80

/* insert item after pre and before next,
 * return 0 on success and 1 on failure */
int ll_insert(void *item, void *prev, void *next){
	ll_remove(item);
	Llist *li = item;
	Llist *lp = prev;
	Llist *ln = next;

	if( ! li ) return 1;

	if( lp )
		lp->next = li;
	if( ln )
		ln->prev = li;

	li->prev = lp;
	li->next = ln;
	return 0;
}
/* remove node, linking node->prev and node->next
 * returns 0 on success and 1 on failure */
int ll_remove(void *node){
	Llist *li = node;
	if( ! li ) return 1;
	if( !li->next && !li->prev ) return 1; /* nothing to remove from */

	if( li->prev )
		li->prev->next = li->next;
	if( li->next )
		li->next->prev = li->prev;

	/* some nice tidying up, makes sure that ln doesn't think it's still in the list */
	li->next = 0;
	li->prev = 0;
	return 0;
}
/* returns previous node or 0*/
void * ll_prev(void *node){
	Llist *ln = node;
	return ln->prev;
}
/* returns next node or 0 */
void * ll_next(void *node){
	Llist *ln = node;
	return ln->next;
}


/** --- below this line needs to be moved --- **/

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
	Line *l = calloc(1, sizeof(Line));
	if( !l ){
		perror("Failed to calloc in llist:newline for l ");
		return 0; /* error */
	}
	if( ll_insert(l, prev, next) ){
		perror("Failed to insert a line in llist:newline for l ");
		return 0;
	}
	l->mul = mul;
	l->len = 0;
	l->con = calloc(LINESIZE * mul,  sizeof(char));
	if( ! l->con ){
		perror("Failed to calloc in llist:newline for l->con ");
		return 0; /* error */
	}
	return l;
}

Buffer*
newbuffer(char *path){
	Buffer *b = calloc(1, sizeof(Buffer));
	if( !b ){
		perror("Failed to ammloc in llist:newfile for b ");
		return 0; /* error */
	}
	b->s_start = b->start = 0;
	b->s_end = b->end = 0;
	b->path = path;
	return b;
}

/* Movement functions */
void
m_startofline(Buffer *buf){
	buf->cur.offset = 0;
}

void
m_endofline(Buffer *buf){
	/* cur if where to insert next, end of line is after last character in line */
	/* this SHOULDNt actually be safe though as last char may be \n... FIXME */
	buf->cur.offset = buf->cur.line->len + 1;
}

void
m_startoffile(Buffer *buf){
	buf->cur.line = buf->start;
	buf->cur.offset = 0;
}

void
m_endoffile(Buffer *buf){
	buf->cur.line = buf->end;
	buf->cur.offset = buf->start->len;
}

void
m_prevchar(Buffer *buf){
	if( --buf->cur.offset < 0 ){
		if( ll_prev(buf->cur.line) ){
			buf->cur.line = ll_prev(buf->cur.line);
			buf->cur.offset = buf->cur.line->len;
		} else {
			buf->cur.offset = 0;
		}
	}
}

void
m_nextchar(Buffer *buf){
	if( ++buf->cur.offset > buf->cur.line->len ){
		if( ll_next(buf->cur.line) ){
			buf->cur.line = ll_next(buf->cur.line);
			buf->cur.offset = 0;
		} else {
			buf->cur.offset = buf->cur.line->len;
		}
	}
}

void
m_prevline(Buffer *buf){
	if( ! ll_prev(buf->cur.line) )
		return;
	int vo = i_to_vo(buf->cur.line, buf->cur.offset);
	buf->cur.line = ll_prev(buf->cur.line);
	int i = vo_to_i(buf->cur.line, vo);
	buf->cur.offset = i;
}

void
m_nextline(Buffer *buf){
	if( ! ll_next(buf->cur.line) )
		return;
	int vo = i_to_vo(buf->cur.line, buf->cur.offset);
	buf->cur.line = ll_next(buf->cur.line);
	int i = vo_to_i(buf->cur.line, vo);
	buf->cur.offset = i;
}

void
m_prevword(Buffer *buf){
	for( m_prevchar(buf); ; m_prevchar(buf) ){
		if( wordbreak( buf->cur.line->con[buf->cur.offset] ) )
			break;
		if( buf->cur.offset == 0 )
			break;
	}
}

void
m_nextword(Buffer *buf){
	for( m_nextchar(buf); ; m_nextchar(buf) ){
		if( wordbreak( buf->cur.line->con[buf->cur.offset] ) )
			break;
		if( buf->cur.offset == 0 )
			break;
	}
}

void
m_scrolldown(Buffer *buf){
	int i;
	for( i=0; i<scrolldistance; ++i ){
		if( ll_next(buf->s_start) )
			buf->s_start = ll_next(buf->s_start);
		else
			break;
	}
}

void
m_scrollup(Buffer *buf){
	int i;
	for( i=0; i<scrolldistance; ++i ){
		if( ll_prev(buf->s_start) )
			buf->s_start = ll_prev(buf->s_start);
		else
			break;
	}
}

int
vo_to_i(Line *l, int voffset){
	int i, vo;
	if( !l )
		return -1;

	/* find appropriate i for voffset */
	for( i=0, vo=0; vo < voffset; ++i ){
		if( i >= l->len )
			break; /* if we are off the end of the line, then return the end of the line */
		if( l->con[i] == '\t' )
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
		if( l->con[offset-1] == '\t' )
			vo += tabwidth;
		else
			++vo;
	}
	return vo;
}

void /* move cur to linenum, voffset from current line */
position_cursor(Buffer *buf, int linenum, int voffset){
	Line *l;
	if( ! buf->s_start )
		return;

	for( l=buf->s_start; linenum && l; --linenum, l=ll_next(l) ) ;

	if( !l ){
		buf->cur.line = buf->end;
		buf->cur.offset = buf->end->len;
		return;
	}

	buf->cur.line = l;
	buf->cur.offset = vo_to_i(l, voffset);
}

void
backspace(Buffer *buf){
	int oo; /* old offset */
	Line *ol; /* old line */

	buf->mod = 1;
	if( buf->cur.offset < 1 ){
		/* FIXME TODO deal with case of backspacing over lines */
		oo = buf->cur.offset;
		ol = buf->cur.line;

		/* insert line at end of previous line, correct links, free */
		if( ! ll_prev(ol) )
			return; /* cannot delete the first line */

		buf->cur.line = ll_prev(ol);
		buf->cur.offset = buf->cur.line->len;
		insert(buf, &ol->con[oo]);

		if( ll_remove(ol) ){
			perror("ll_remove failed in backspace for ol ");
			return;
		}
		free( ol->con );
		free(ol);
		return;
	}
	int len = buf->cur.line->len - buf->cur.offset + 1;
	memmove( &buf->cur.line->con[buf->cur.offset-1], &buf->cur.line->con[buf->cur.offset], len);
	--buf->cur.line->len;
	--buf->cur.offset;
}
int /* load file buf->path into buf, returns 0 on success and 1 on error */
load(Buffer *buf){
	int read; /* returned by fread */
	char tmp[BUFSIZE+1]; /* for fread to read into, +1 as we need a \0, [read+1] will be \0 */
	int ret;
	FILE *f = fopen(buf->path, "r");

	if( !f ){
		fprintf(stderr, "Error opening file '%s'", buf->path);
		perror(" in llist:load (passed as buf->path) ");
		return 1; /* error */
	}

	if( ! buf->start){
		buf->s_start = buf->start = newline(1, 0, 0);
		buf->s_end = buf->end = buf->start;
		buf->cur = (Pos){buf->start, 0};
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
	buf->mod = 0;
	/* FIXME consider setting buf->end, I think insert should do this*/
	return 0;
}

int /* save buf into file buf->path, returns 0 on success and 1 on error */
save(Buffer *buf){
	Line *l;
	FILE *f = fopen(buf->path, "w");
	if( !f ){
		perror("Failed to open buf->path in llist:save ");
		return 1; /* error */
	}
	for( l=buf->start ; l; l=ll_next(l) ){
		fwrite(l->con, sizeof(char), l->len, f);
		if( ll_next(l) ) /* only append newline if there is a line following */
			fwrite("\n", sizeof(char), 1, f);
	}
	fclose(f);
	return 0;
}

int /* insert at cur and move cur along, returns 0 on success and 1 on error */
insert(Buffer *buf, const char *str){
	/* inserts character by character, performs far too many memmoves but is simple */
	buf->mod = 1;

	int i=0, l=strlen(str);
	int ret; /* return value from insert */
	int oo; /* old offset */
	Line *ol, *nl; /* old line, new line */

	for( i=0; i<l; ++i ){
		/* handle resizing */
		if( buf->cur.line->len + 2 > buf->cur.line->mul * LINESIZE ){
			buf->cur.line->con = realloc(buf->cur.line->con, ++buf->cur.line->mul * LINESIZE);
			if( ! buf->cur.line->con ){
				perror("Failed realloc in llist:insert ");
				return 1; /* FIXME error */
			}
		}
		/* handle newline in string, should not be inserted */
		if( str[i] == '\n' || str[i] == '\r' ){
			nl = newline(1, buf->cur.line, ll_next(buf->cur.line));
			if( ! nl ){
				perror("Failed call to newline from llist:insert ");
				return 1; /* FIXME error */
			}
			//ll_insert(nl, buf->cur.line, ll_next(buf->cur.line));

			/* old line */
			ol = buf->cur.line;
			/* old offset */
			oo = buf->cur.offset;

			/* move cur to start of next line */
			buf->cur = (Pos){nl, 0};
			/* copy over old con */
			ret = insert( buf, &ol->con[oo]);
			if( ret )
				return 1;
			/* add a null character */
			ol->con[oo] = 0;
			ol->len = oo;
			/* carry on from the start of the next line */
			buf->cur = (Pos){nl, 0};
		} else {

			/* move old con out of the way and insert character */
			memmove( &buf->cur.line->con[buf->cur.offset+1], & buf->cur.line->con[buf->cur.offset], buf->cur.line->len - buf->cur.offset + 1 );

			buf->cur.line->con[buf->cur.offset] = str[i];
			++buf->cur.line->len;
			++buf->cur.offset;
		}
	}
	buf->end = nl;
	return 0;
}


