#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "libmallocab.h"

static void fail_if(bool condition, const char *message)
{
    if (condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        abort();
    }
}

static void run_abort_child(void (*child_func)(void), const char *name)
{
    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        child_func();
        _exit(EXIT_FAILURE);
    }

    int status = 0;
    assert(waitpid(pid, &status, 0) == pid);
    fail_if(!WIFSIGNALED(status), "child did not terminate by signal");
    fail_if(WTERMSIG(status) != SIGABRT, "child did not abort with SIGABRT");
    printf("PASS: %s abort path\n", name);
}

static void test_mallocab_success(void)
{
    void *ptr = mallocab(16);
    assert(ptr != NULL);
    memset(ptr, 0x5a, 16);
    free(ptr);
    printf("PASS: mallocab success\n");
}

static void test_reallocab_success(void)
{
    char *ptr = mallocab(16);
    assert(ptr != NULL);
    strcpy(ptr, "hello");
    ptr = reallocab(ptr, 32);
    assert(ptr != NULL);
    assert(strcmp(ptr, "hello") == 0);
    ptr = reallocab(ptr, 8);
    assert(ptr != NULL);
    assert(strcmp(ptr, "hello") == 0);
    free(ptr);
    printf("PASS: reallocab success\n");
}

static void test_strdupab_success(void)
{
    const char *source = "libmallocab test";
    char *copy = strdupab(source);
    assert(copy != NULL);
    assert(strcmp(copy, source) == 0);
    free(copy);
    printf("PASS: strdupab success\n");
}

static void test_strndupab_success(void)
{
    const char *source = "libmallocab";
    char *copy = strndupab(source, 8);
    assert(copy != NULL);
    assert(strcmp(copy, "libmallo") == 0);
    assert(strlen(copy) == 8);
    free(copy);
    printf("PASS: strndupab success\n");
}

static void test_snprintfab_success(void)
{
    char buffer[32];
    int result = snprintfab(buffer, sizeof(buffer), "a=%d, b=%s", 123, "ok");
    assert(result == 11);
    assert(strcmp(buffer, "a=123, b=ok") == 0);
    printf("PASS: snprintfab success\n");
}

static void abort_mallocab_child(void)
{
#ifdef RLIMIT_AS
    struct rlimit limit = { .rlim_cur = 1 << 20, .rlim_max = 1 << 20 };
    if (setrlimit(RLIMIT_AS, &limit) != 0) {
        perror("setrlimit");
        _exit(EXIT_FAILURE);
    }
#endif
    mallocab(64 * 1024 * 1024);
}

static void abort_snprintfab_child(void)
{
    char buffer[8];
    snprintfab(buffer, sizeof(buffer), "this-is-long");
}

int main(void)
{
    test_mallocab_success();
    test_reallocab_success();
    test_strdupab_success();
    test_strndupab_success();
    test_snprintfab_success();

    run_abort_child(abort_mallocab_child, "mallocab");
    run_abort_child(abort_snprintfab_child, "snprintfab");

    puts("ALL TESTS PASSED");
    return EXIT_SUCCESS;
}
