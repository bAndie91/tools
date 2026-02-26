#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Simple test runner (acutest-like minimal) so no external deps required.
 */

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(expr) do { \
    tests_run++; \
    if(!(expr)) { \
        fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(a,b) do { \
    tests_run++; \
    const char *_a = (a), *_b = (b); \
    int _cmp = (_a == NULL && _b == NULL) ? 0 : ((_a == NULL || _b == NULL) ? 1 : strcmp(_a, _b)); \
    if(_cmp != 0) { \
        fprintf(stderr, "String assertion failed: '%s' vs '%s' (%s:%d)\n", _a?_a:"(null)",_b?_b:"(null)", __FILE__, __LINE__); \
        tests_failed++; \
        return; \
    } \
} while(0)

/* Include implementation to allow testing static/internal helpers */
#include "../libarray.c"

/* Tests */

static void test_init_free()
{
    Array *a = NULL;
    array_init(&a, 0);
    ASSERT(a != NULL);
    ASSERT(array_length(&a) == 0);
    ASSERT(a->item != NULL);
    array_free(&a);
}

static void test_append_getitem_length()
{
    Array *a = NULL;
    array_init(&a, 1);
    array_append(&a, "foo");
    ASSERT(array_length(&a) == 1);
    ASSERT_STR_EQ(array_getitem(&a, 0), "foo");
    array_append(&a, NULL);
    ASSERT(array_length(&a) == 2);
    ASSERT(array_getitem(&a, 1) == NULL);
    array_free(&a);
}

static void test_setitem_and_insert_gap()
{
    Array *a = NULL;
    array_init(&a, 2);
    array_setitem(&a, 0, "zero");
    array_setitem(&a, 2, "two");
    /* index 1 should be created as NULL */
    ASSERT(array_length(&a) == 3);
    ASSERT(array_getitem(&a, 1) == NULL);
    ASSERT_STR_EQ(array_getitem(&a, 2), "two");
    /* insert at 1 shifts 2 -> 3 */
    array_insert(&a, 1, "one");
    ASSERT_STR_EQ(array_getitem(&a, 1), "one");
    ASSERT_STR_EQ(array_getitem(&a, 2), NULL);
    ASSERT_STR_EQ(array_getitem(&a, 3), "two");
    array_free(&a);
}

static void test_pop_shift_remove()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "a");
    array_append(&a, "b");
    array_append(&a, "c");
    char *p = array_pop(&a, 1);
    ASSERT_STR_EQ(p, "b");
    free(p);
    ASSERT(array_length(&a) == 2);
    char *s = array_pop(&a, 0);
    ASSERT_STR_EQ(s, "a");
    free(s);
    ASSERT_STR_EQ(array_getitem(&a, 0), "c");
    array_append(&a, "x");
    array_append(&a, "y");
    array_remove(&a, "x");
    /* removed first 'x' */
    for(size_t i=0;i<array_length(&a);i++){
        if(array_getitem(&a, i) && strcmp(array_getitem(&a,i), "x")==0) {
            ASSERT(0);
        }
    }
    array_free(&a);
}

static void test_condense_and_empty()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "a");
    array_append(&a, NULL);
    array_append(&a, "b");
    array_append(&a, NULL);
    ASSERT(array_length(&a) == 4);
    array_condense(&a);
    ASSERT(array_length(&a) == 2);
    ASSERT_STR_EQ(array_getitem(&a, 0), "a");
    ASSERT_STR_EQ(array_getitem(&a, 1), "b");
    array_empty(&a);
    ASSERT(array_length(&a) == 0);
    array_free(&a);
}

static array_loop_control foreach_cb(array_index_t idx, char * item, void * data)
{
    (void)idx;
    int *counter = data;
    if(item != NULL) (*counter)++;
    if(*counter > 1) return ARRAY_LOOP_STOP;
    return ARRAY_LOOP_CONTINUE;
}

static void test_foreach()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "one");
    array_append(&a, "two");
    int c = 0;
    array_foreach(&a, 0, foreach_cb, &c);
    ASSERT(c == 2 || c == 2); /* should have counted two before stopping */
    array_free(&a);
}

static void test_growth_and_contract()
{
    Array *a = NULL;
    array_init(&a, 1);
    for(int i=0;i<100;i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "v%03d", i);
        array_append(&a, buf);
    }
    ASSERT(a->length == 100);
    ASSERT(a->size >= a->length);
    /* remove many items to trigger contraction */
    array_delete(&a, 0, 90);
    ASSERT(a->length == 10);
    ASSERT(a->size >= a->length);
    array_free(&a);
}

int main(void)
{
    printf("Running libarray unit tests...\n");

    test_init_free();
    test_append_getitem_length();
    test_setitem_and_insert_gap();
    test_pop_shift_remove();
    test_condense_and_empty();
    test_foreach();
    test_growth_and_contract();

    if(tests_failed) {
        fprintf(stderr, "%d tests failed (ran %d assertions)\n", tests_failed, tests_run);
        return 1;
    }
    printf("All tests passed (%d assertions)\n", tests_run);
    return 0;
}
