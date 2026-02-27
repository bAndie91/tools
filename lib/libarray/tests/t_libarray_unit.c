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
    array_append(&a, "three");
    array_append(&a, "four");
    int c = 0;
    array_foreach(&a, 0, foreach_cb, &c);
    ASSERT(c == 2); /* should have counted two before stopping */
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

static void test_getitem_out_of_bounds()
{
    Array *a = NULL;
    array_init(&a, 0);
    ASSERT(array_getitem(&a, 0) == NULL);
    ASSERT(array_getitem(&a, 5) == NULL);
    array_append(&a, "x");
    ASSERT(array_getitem(&a, 1) == NULL);
    array_free(&a);
}

static void test_insert_far_index()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_insert(&a, 5, "foo");
    ASSERT(array_length(&a) == 6);
    for(size_t i=0;i<5;i++) {
        ASSERT(array_getitem(&a, i) == NULL);
    }
    ASSERT_STR_EQ(array_getitem(&a, 5), "foo");
    array_free(&a);
}

static void test_pop_oob()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "a");
    char *p = array_pop(&a, 2);
    ASSERT(p == NULL);
    ASSERT(array_length(&a) == 1);
    array_free(&a);
}

static void test_delete_multiple_and_tail()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "1");
    array_append(&a, "2");
    array_append(&a, "3");
    array_append(&a, "4");
    array_append(&a, "5");
    array_delete(&a, 3, 2);
    ASSERT(array_length(&a) == 3);
    ASSERT_STR_EQ(array_getitem(&a, 0), "1");
    ASSERT_STR_EQ(array_getitem(&a, 1), "2");
    ASSERT_STR_EQ(array_getitem(&a, 2), "3");
    array_free(&a);
}

static void test_remove_not_found()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "foo");
    array_remove(&a, "bar");
    ASSERT(array_length(&a) == 1);
    ASSERT_STR_EQ(array_getitem(&a, 0), "foo");
    array_free(&a);
}

static void test_condense_empty()
{
    Array *a = NULL;
    array_init(&a, 0);
    ASSERT(array_condense(&a) == 0);
    array_free(&a);
}

static void test_empty_empty()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_empty(&a);
    ASSERT(array_length(&a) == 0);
    array_free(&a);
}

static void test_foreach_start_index()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "a");
    array_append(&a, "b");
    int cnt = 0;
    array_foreach(&a, 1, foreach_cb, &cnt);
    ASSERT(cnt == 1);
    array_free(&a);
}

static void test_internal_grow_contract()
{
    Array *a = NULL;
    array_init(&a, 1);
    _array_grow(&a, 10);
    ASSERT(a->size >= 10);
    ASSERT(a->length == 0);
    _array_contract(&a);
    /* after contracting with len 0, size should be 1 */
    ASSERT(a->size == 1);
    array_free(&a);
}

static void test_setitem_replaces()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "first");
    array_setitem(&a, 0, "second");
    char *new = array_getitem(&a, 0);
    /* pointer may or may not change; only string equality matters */
    ASSERT_STR_EQ(new, "second");
    /* length must not have been changed */
    ASSERT(array_length(&a) == 1);
    array_free(&a);
}

static void test_append_nulls()
{
    Array *a = NULL;
    array_init(&a, 0);
    for(int i=0;i<10;i++) array_append(&a, NULL);
    ASSERT(array_length(&a) == 10);
    array_condense(&a);
    ASSERT(array_length(&a) == 0);
    array_free(&a);
}

static void test_delete_gap_too_large()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "1");
    array_delete(&a, 0, 5);
    ASSERT(array_length(&a) == 0);
    array_free(&a);
}

static void test_foreach_oob_start()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "a");
    int cnt = 0;
    array_foreach(&a, 5, foreach_cb, &cnt);
    ASSERT(cnt == 0);
    array_free(&a);
}

static void test_setitem_null()
{
    Array *a = NULL;
    array_init(&a, 0);
    array_append(&a, "x");
    array_setitem(&a, 0, NULL);
    ASSERT(array_getitem(&a, 0) == NULL);
    array_free(&a);
}

static void test_insert_large_index()
{
    Array *a = NULL;
    array_init(&a, 1);
    array_insert(&a, 1000, "big");
    ASSERT(array_length(&a) == 1001);
    ASSERT(array_getitem(&a, 1000) && strcmp(array_getitem(&a,1000),"big")==0);
    array_free(&a);
}

static void test_condense_all_null()
{
    Array *a = NULL;
    array_init(&a, 5);
    for(int i=0;i<5;i++) array_append(&a, NULL);
    ASSERT(array_length(&a) == 5);
    array_condense(&a);
    ASSERT(array_length(&a) == 0);
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
    test_getitem_out_of_bounds();
    test_insert_far_index();
    test_pop_oob();
    test_delete_multiple_and_tail();
    test_remove_not_found();
    test_condense_empty();
    test_empty_empty();
    test_foreach_start_index();
    test_internal_grow_contract();
    test_setitem_replaces();
    test_append_nulls();
    test_delete_gap_too_large();
    test_foreach_oob_start();
    test_setitem_null();
    test_insert_large_index();
    test_condense_all_null();

    if(tests_failed) {
        fprintf(stderr, "%d tests failed (ran %d assertions)\n", tests_failed, tests_run);
        return 1;
    }
    printf("All tests passed (%d assertions)\n", tests_run);
    return 0;
}
