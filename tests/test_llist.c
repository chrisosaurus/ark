#include <stdlib.h> /* EXIT_SUCCESS and EXIT_FAILURE */
#include <check.h>
#include "../llist.h"

START_TEST (test_llist_newline){
	Line *l = newline(3, 0, 0);
	fail_unless( l != 0, "newline failed" );
	fail_unless( l->mul == 3, "length set incorrectly" );
	fail_unless( l->prev == 0, "prev not set correctly" );
	fail_unless( l->next== 0, "next not set correctly" );
}
END_TEST

void
llist_setup(){
}

void
llist_teardown(){
}

Suite *
llist_suite(void){
	Suite *s = suite_create("llist");

	/* individual test case */
	TCase *tc_newline = tcase_create("newline");
	tcase_add_checked_fixture(tc_newline, llist_setup, llist_teardown);
	tcase_add_test(tc_newline, test_llist_newline);
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
