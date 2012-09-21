#include <stdlib.h> /* EXIT_SUCCESS and EXIT_FAILURE */
#include <check.h>
#include "../llist.h"

Line *line;

START_TEST (test_llist_newline){
	line = newline(3, 0, 0);
	fail_unless( line != 0, "newline failed" );
	fail_unless( line->mul == 3, "length set incorrectly" );
	fail_unless( line->prev == 0, "prev not set correctly" );
	fail_unless( line->next== 0, "next not set correctly" );
}
END_TEST

START_TEST (test_llist_newline_more){
	Line *nl = newline(4, 0, 0);
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
	/* FIXME these currently segfault */
	fail_unless( line->len == strlen(str), "length after insert is incorrect (strlen of str)" );
	fail_unless( line->len == strlen(line->contents), "length after insert is incorrect (strlen of contents)" );
	fail_unless( line->mul * LINESIZE > line->len, "mul after insert is incorrect" );
}
END_TEST

START_TEST (test_llist_movement){
	Pos p = {line, 0};

	p = m_startofline(p);
	fail_unless( p.line == line, "startofline moved off line" );
	fail_unless( p.offset == 0, "startofline did not move to start of line" );

	p = m_endofline(p);
	fail_unless( p.line == line, "endofline moved off line" );
	fail_unless( p.offset == strlen(line->contents), "endofline did not move to end of line" );
}
END_TEST

void
llist_setup(){
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
	tcase_add_test(tc_newline, test_llist_newline_more);
	tcase_add_test(tc_newline, test_llist_insert);
	tcase_add_test(tc_newline, test_llist_movement);

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
