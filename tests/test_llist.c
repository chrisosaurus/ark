#include <stdlib.h> /* EXIT_SUCCESS and EXIT_FAILURE */
#include <check.h>
#include "../llist.h"

/* path to use for testing file io */
char *path = "tests/testing_data";
Buffer *buf;

START_TEST (test_llist_newline){
	Line *nl = newline(4, buf->start, 0);

	fail_unless( nl->mul == 4, "newline mul set incorrectly" );
	fail_unless( nl->prev == buf->start, "newline prev set incorrectly" );
	fail_unless( nl->next == 0, "newline next set incorrectly" );
	fail_unless( nl->len == 0, "newline len set incorrectly" );
}
END_TEST

START_TEST (test_llist_insert){
	char *str = "hello world";
	int ret;

	ret = insert(buf, str);
	fail_if( ret, "insert returned non-0" );
	/* len should be the same as strlen, neither takes \0 into account */
	fail_unless( buf->cursor.line->len == strlen(str), "length after insert is incorrect (strlen of str)" );
	fail_unless( buf->cursor.line->len == strlen(buf->cursor.line->contents), "length after insert is incorrect (strlen of contents)" );
	fail_unless( buf->cursor.line->mul * LINESIZE > buf->cursor.line->len, "mul after insert is incorrect" );
	fail_if ( strcmp(str, buf->cursor.line->contents), "contents do not match inserted text" );
}
END_TEST

START_TEST (test_llist_insert_realloc){
	char *str = "hello world";
	int i=0;
	int ret;

	/* keep inserting until we realloc */
	for( i=0; buf->start->mul<2; ++i ){
		ret = insert(buf, str);
		fail_if( ret, "insert returned non-0");
	}
	fail_unless( strlen(buf->cursor.line->contents) == i*strlen(str), "multiple inserts failed" );

}
END_TEST

START_TEST (test_llist_insert_newline){
	char *str="hello\nworld";
	int ret;

	ret = insert(buf, str);
	fail_if( ret, "insert returned non-0 error code in test_llist_insert_newline" );

	/* '\n'(s) should appear in the text */
	fail_unless( strlen(buf->start->contents) == strlen("hello\n"), "inserting hello\\n failed" );
	fail_unless( buf->start->next != 0, "inserting \\n failed to create a newline" );
	fail_unless( strlen(buf->start->next->contents) == strlen("world"), "newline doesnt match expected text ('world')" );
	fail_unless( buf->start->next->prev == buf->start, "newline prev doesnt match line" );
}
END_TEST

START_TEST (test_llist_movement){
	m_startofline(buf);
	fail_unless( buf->cursor.line == buf->start, "startofline moved off line" );
	fail_unless( buf->cursor.offset == 0, "startofline did not move to start of line" );

	m_endofline(buf);
	fail_unless( buf->cursor.line == buf->start, "endofline moved off line" );
	/* character is where to insert next, so end of line is after the last char and before the \0 */
	fail_unless( buf->cursor.offset == strlen(buf->start->contents)+1, "endofline did not move to end of line" );

	m_startoffile(buf);
	//insert(p, "\n\n");
	m_startoffile(buf);
	fail_unless( buf->cursor.line == buf->start, "start of file is not buf->start");

	m_endoffile(buf);
	fail_unless( buf->cursor.line == buf->end, "end of file is not buf->end");
}
END_TEST

START_TEST (test_llist_load){
	char *str = "hello\nthere \n\nworld";

	FILE *f = fopen(path, "w");
	fwrite(str, sizeof(char), strlen(str), f);
	fclose(f);

	Buffer *buf = newbuffer(path);
	load(buf);

	fail_if( strcmp(buf->start->contents, "hello\n"), "line contents did not match expected ('hello\\n')" );
	fail_if( strcmp(buf->start->next->contents, "there \n"), "line->next contents did not match expected ('there \\n')" );
	fail_if( strcmp(buf->start->next->next->contents, "\n"), "line->next->next contents did not match expected ('\\n')" );
	fail_if( strcmp(buf->start->next->next->next->contents, "world"), "l->n->n->n->c did not match expected ('world')" );
	fail_if( buf->start->next->next->next->next != 0, "more lines in llist than expected" );
}
END_TEST

START_TEST (test_llist_save){
	char *str = "hello\nthere \n\nworld";
	int ret;

	ret = insert( buf, str );
	fail_if( ret, "insert returned non-0 in test_llist_save" );
	save(buf);

	FILE *f = fopen(path, "r");
	char tmp[20];
	fread(tmp, sizeof(char), 19, f);
	tmp[19] = 0;
	fail_if( strcmp(str, tmp), "contents read from file did not match expected" );
}
END_TEST

void
llist_setup(){
	Line *line = newline(1, 0, 0);
	fail_unless( line != 0, "newline failed" );
	fail_unless( line->mul == 1, "mul set incorrectly" );
	fail_unless( line->len == 0, "length set incorrectly" );
	fail_unless( line->prev == 0, "prev not set correctly" );
	fail_unless( line->next== 0, "next not set correctly" );

	buf = newbuffer(path);
	fail_unless( buf != 0, "newbuffer failed" );
	fail_if( strcmp(path, buf->path), "newbuffer set incorrect path" );
	buf->start = buf->end = line;
	buf->cursor = (Pos){line, 0};
}

void
llist_teardown(){
	Line *l;
	if( buf ){
		for( l=buf->start; l; l=l->next ){
			free(l->contents);
			free(l);
		}
		free(buf);
	}
}

Suite *
llist_suite(void){
	Suite *s = suite_create("llist");

	/* individual test case */
	TCase *tc_newline = tcase_create("newline");
	tcase_add_checked_fixture(tc_newline, llist_setup, llist_teardown);

	tcase_add_test(tc_newline, test_llist_newline);
	tcase_add_test(tc_newline, test_llist_insert);
	tcase_add_test(tc_newline, test_llist_insert_realloc);
	tcase_add_test(tc_newline, test_llist_insert_newline);
	tcase_add_test(tc_newline, test_llist_movement);
	tcase_add_test(tc_newline, test_llist_load);
	tcase_add_test(tc_newline, test_llist_save);

	suite_add_tcase(s, tc_newline);

	return s;
}

int main(){
	int number_failed = 0;
	Suite *s = llist_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
