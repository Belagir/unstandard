
#include <ustd/testutilities.h>

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// -------------------------------------------------------------------------------------------------
void __tst_print(char *message, ...) {
    va_list args;
    va_start(args, message);

    vprintf(message, args);

    va_end(args);
}

// -------------------------------------------------------------------------------------------------
int __tst_compare_mem(void *addr1, void *addr2, unsigned long size_bytes) {
    char *byte_addr1 = (char *) addr1;
    char *byte_addr2 = (char *) addr2;
    char *byte_end = ((char *) addr1) + size_bytes;
    int is_same = 1;

    while ((byte_addr1 != byte_end) && is_same){
        is_same = (*byte_addr1 == *byte_addr2);

        byte_addr1++;
        byte_addr2++;
    }

    return is_same;
}

// -------------------------------------------------------------------------------------------------
int __tst_print_test_case_report(char *case_name, unsigned nb_assertions, unsigned nb_failed) {
    if (nb_failed == 0) {
        __tst_print("\033[1;32m[TEST PASSED] :\033[0m \033[1m`%s'\033[0m with %d assertions ran.\n", case_name, nb_assertions);
    } else {
        __tst_print("\033[1;31m[TEST FAILED] :\033[0m \033[1m`%s'\033[0m with %d assertions failed out of %d ran.\n", case_name, nb_failed, nb_assertions);
    }
}
