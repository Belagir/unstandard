/**
 * @file testutilities.h
 * @author gabriel
 * @brief Light assertion-oriented test framework. The framework is primarely defined as a set of macros, and with all things macros, headeache is likely to occur when the module is actually used.
 * @version 0.1
 * @date 2023-02-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef UNSTANDARD_TESTUTILITIES_H__
#define UNSTANDARD_TESTUTILITIES_H__

/**
 * @brief Asserts that test is true ; log the failure otherwise.
 */
#define tst_assert(test, message, ...) \
        do { \
            *__tst_local_nb_assertions += 1u; \
            if (!(test)) { \
                tstprivate_print("\033[0;31m[ASSERTION FAILED] :\033[0m %s:%d \033[1m`%s' : ", FILE__, LINE__, test_name); \
                tstprivate_print((message) VA_OPT__(,) VA_ARGS__); \
                tstprivate_print("\n\033[0m"); \
                *__tst_local_nb_failed += 1u; \
            } \
        } while (0)

/**
 * @brief Assert that two memory regions of the same size contains the same bytes.
 *
 */
#define tst_assert_memory_equal(mem1, mem2, size, message, ...) \
        tst_assert(tstprivate_compare_mem((void *) (mem1), (void *) (mem2), (size)), (message) VA_OPT__(,) VA_ARGS__)

/**
 * @brief Asserts that two values of the same type are equal (as per the == operator)
 *
 */
#define tst_assert_equal(val1, val2, format_mod) \
        tst_assert((val1) == (val2), "values mismatch : expected " format_mod ", got " format_mod, (val1), (val2))


#define tst_assert_equal_ext(val1, val2, format_mod, format_ext, ...) \
        tst_assert((val1) == (val2), "values mismatch : expected " format_mod ", got " format_mod " " format_ext, (val1), (val2) VA_OPT__(,) VA_ARGS__)

/**
 * @brief Creates a unit named test scenario.
 *
 */
#define tst_CREATE_TEST_SCENARIO(identifier, data_structure, test_scenario) \
        struct tst_scenario_datastruct_ ## identifier data_structure; \
        static void tst_scenario_function_ ## identifier (struct tst_scenario_datastruct_ ## identifier *data, char *test_name, unsigned *__tst_local_nb_assertions, unsigned *__tst_local_nb_failed) test_scenario

/**
 * @brief Creates a unit test case from a scenario.
 *
 */
#define tst_CREATE_TEST_CASE(identifier_case, identifier_scenario, ...) \
        static void tst_case_function_ ## identifier_case (void) { \
            struct tst_scenario_datastruct_ ## identifier_scenario data = { VA_ARGS__ }; \
            unsigned tst_local_nb_assertions = 0u; \
            unsigned tst_local_nb_failed = 0u; \
            \
            tst_scenario_function_ ## identifier_scenario (&data, #identifier_case, &__tst_local_nb_assertions, &__tst_local_nb_failed); \
            \
            tstprivate_print_test_case_report(#identifier_case, tst_local_nb_assertions, tst_local_nb_failed, FILE__, LINE__); \
        }

/**
 * @brief Runs a test case.
 *
 */
#define tst_run_test_case(identifier_case) do { tst_case_function_ ## identifier_case(); } while(0)


/**
 * @brief prints a message to stdout.
 *
 * @warning do not call directly ! This is meant to be used by a tst_assert* macro.
 * @param message format string
 * @param ... format string arguments
 */
void tstprivate_print(char *message, ...);

/**
 * @brief Compare two memory regions of the same size.
 *
 * @warning do not call directly ! This is meant to be used by a tst_assert* macro.
 * @param addr1
 * @param addr2
 * @param size_bytes
 */
int tstprivate_compare_mem(void *addr1, void *addr2, unsigned long size_bytes);

/**
 * @brief Prints a unit test report.
 *
 * @warning do not call directly ! This is meant to be used by a tst_assert* macro.
 * @param case_name
 * @param nb_assertions
 * @param nb_failed
 * @return int
 */
int tstprivate_print_test_case_report(char *case_name, unsigned nb_assertions, unsigned nb_failed, const char *file, int line);

#endif