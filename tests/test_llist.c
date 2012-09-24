#include <stdlib.h> /* EXIT_SUCCESS and EXIT_FAILURE */
#include <check.h>
#include "../llist.h"

/* path to use for testing file io */
char *path = "tests/testing_data";
Line *line;

START_TEST (test_llist_newline){
	Line *nl = newline(4, line, 0);

	fail_unless( nl->mul == 4, "newline mul set incorrectly" );
	fail_unless( nl->prev == line, "newline prev set incorrectly" );
	fail_unless( nl->next == 0, "newline next set incorrectly" );
	fail_unless( nl->len == 0, "newline len set incorrectly" );
}
END_TEST

START_TEST (test_llist_insert){
	char *str = "hello world";
	Pos p = {line, 0};

	p = insert(p, str);
	/* len should be the same as strlen, neither takes \0 into account */
	fail_unless( line->len == strlen(str), "length after insert is incorrect (strlen of str)" );
	fail_unless( line->len == strlen(line->contents), "length after insert is incorrect (strlen of contents)" );
	fail_unless( line->mul * LINESIZE > line->len, "mul after insert is incorrect" );
	fail_if ( strcmp(str, line->contents), "contents do not match inserted text" );
}
END_TEST

START_TEST (test_llist_insert_realloc){
	char *str = "hello world";
	Pos p = {line, 0};
	int i=0;

	/* keep inserting until we realloc */
	for( i=0; p.line->mul<2; ++i ){
		p = insert(p, str);
	}
	fail_unless( strlen(p.line->contents) == i*strlen(str), "multiple inserts failed" );

}
END_TEST

START_TEST (test_llist_insert_newline){
	char *str="hello\nworld";
	Pos p = {line, 0};
	p = insert(p, str);

	/* '\n'(s) should appear in the text */
	fail_unless( strlen(line->contents) == strlen("hello\n"), "inserting hello\\n failed" );
	fail_unless( line->next != 0, "inserting \\n failed to create a newline" );
	fail_unless( strlen(p.line->contents) == strlen("world"), "newline doesnt match expected text ('world')" );
	fail_unless( p.line->prev == line, "newline prev doesnt match line" );
}
END_TEST

START_TEST (test_llist_movement){
	Pos p = {line, 0};

	p = m_startofline(p);
	fail_unless( p.line == line, "startofline moved off line" );
	fail_unless( p.offset == 0, "startofline did not move to start of line" );

	p = m_endofline(p);
	fail_unless( p.line == line, "endofline moved off line" );
	/* character is where to insert next, so end of line is after the last char and before the \0 */
	fail_unless( p.offset == strlen(line->contents) + 1, "endofline did not move to end of line" );

	p = m_startoffile(p);
	p = insert(p, "\n\n");
	p = m_startoffile(p);
	fail_unless( p.line == line, "start of file is not line");

	p = m_endoffile(p);
	fail_unless( p.line == line->next->next, "end of file is not expected line");
}
END_TEST

START_TEST (test_llist_load){
	char *str = "hello\nthere \n\nworld";
	FILE *f = fopen(path, "w");
	fwrite(str, sizeof(char), strlen(str), f);
	fclose(f);
	f = fopen(path, "r");
	load( (Pos){line,0}, f);

	fail_if( strcmp(line->contents, "hello\n"), "line contents did not match expected ('hello\\n')" );
	fail_if( strcmp(line->next->contents, "there \n"), "line->next contents did not match expected ('there \\n')" );
	fail_if( strcmp(line->next->next->contents, "\n"), "line->next->next contents did not match expected ('\\n')" );
	fail_if( strcmp(line->next->next->next->contents, "world"), "l->n->n->n->c did not match expected ('world')" );
	fail_if( line->next->next->next->next != 0, "more lines in llist than expected" );
}
END_TEST

START_TEST (test_llist_save){
	char *str = "hello\nthere \n\nworld";
	FILE *f = fopen(path, "w");
	Pos p = insert( (Pos){line, 0}, str );
	save(f);
	fclose(f);
	f = fopen(path, "r");
	char buf[20];
	fread(buf, sizeof(char), 20, f);
	buf[19] = 0;
	fail_if( strcmp(str, buf), "contents read from file did not match expected" );
}
END_TEST

void
llist_setup(){
	line = newline(1, 0, 0);
	fail_unless( line != 0, "newline failed" );
	fail_unless( line->mul == 1, "mul set incorrectly" );
	fail_unless( line->len == 0, "length set incorrectly" );
	fail_unless( line->prev == 0, "prev not set correctly" );
	fail_unless( line->next== 0, "next not set correctly" );
}

void
llist_teardown(){
	if( line ){
		free(line->contents);
		free(line);
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
