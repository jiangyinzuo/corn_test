// Copyright (c) 2020, Jiang Yinzuo. All rights reserved.

// The latest version of this head file is available on
// https://github.com/ChiangYintso/corn_test

#ifndef CORN_TEST_H
#define CORN_TEST_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#define NUMARGS(...) (sizeof((int[]){ 0, ##__VA_ARGS__ }) / sizeof(int) - 1)

#define FMT(val)                                   \
	_Generic((val), signed char                \
		 : "%d", unsigned char             \
		 : "%u", short                     \
		 : "%d", unsigned short            \
		 : "%u", int                       \
		 : "%d", long                      \
		 : "%ld", long long                \
		 : "%lld", unsigned                \
		 : "%u", unsigned long             \
		 : "%lu", unsigned long long       \
		 : "%llu", float                   \
		 : "%f", double                    \
		 : "%f", long double               \
		 : "%Lf", default                  \
		 : _Generic((val - val), ptrdiff_t \
			    : "%p", default        \
			    : "undef"))

typedef void(test_case_func_t)(void);

struct test_case_node_t {
	char *name;
	test_case_func_t *test_case_func;
	struct test_case_node_t *next;
};

struct test_case_list_t {
	unsigned int length;
	struct test_case_node_t *head, *tail;
} test_case_list = { .length = 0, .head = NULL, .tail = NULL };

static int num_test_failed = 0;
enum colors { RESET, GREEN, RED };
static const char *colors[] = { "\033[0m", "\033[32m", "\033[31m" };

static struct test_case_node_t *
test_case_node_init(char *name, test_case_func_t test_case_func,
		    struct test_case_node_t *next)
{
	struct test_case_node_t *node = malloc(sizeof(struct test_case_node_t));
	node->name = name;
	node->test_case_func = test_case_func;
	node->next = next;
	return node;
}

static void __test_case_add(char *name, test_case_func_t test_case_func)
{
	if (test_case_list.head == NULL) {
		test_case_list.head = test_case_list.tail =
			test_case_node_init(name, test_case_func, NULL);
	} else {
		struct test_case_node_t *new_node =
			test_case_node_init(name, test_case_func, NULL);
		test_case_list.tail->next = new_node;
		test_case_list.tail = new_node;
	}
	++test_case_list.length;
}

static void test_case_list_free()
{
	struct test_case_node_t *node = test_case_list.head;
	while (node != NULL) {
		struct test_case_node_t *to_free = node;
		node = node->next;
		free(to_free);
	}
}

#define TEST(name)                                                    \
	static void test_##name();                                    \
	static void __attribute__((constructor)) test_##name##_init() \
	{                                                             \
		char *test_name = "test_" #name;                      \
		__test_case_add(test_name, test_##name);              \
	}                                                             \
	static void test_##name()

#define PRINT_FAILURE                                                      \
	fprintf(stderr, "%sFailure in %s, %d:\n%s", colors[RED], __FILE__, \
		__LINE__, colors[RESET]);

#define ASSERT(x, ...)                                             \
	if (!(x)) {                                                \
		PRINT_FAILURE;                                     \
		fprintf(stderr, "Expected: true\n");               \
		fprintf(stderr, "Actual: `%s` is false\n", #x);    \
		corn_printfn(NUMARGS(__VA_ARGS__), ##__VA_ARGS__); \
		++num_test_failed;                                 \
	}

static void corn_printfn(int numargs, ...)
{
	if (numargs > 0) {
		va_list args;
		va_start(args, numargs);
		const char *fmt = va_arg(args, const char *);
		vfprintf(stderr, fmt, args);
	}
}

#define ASSERT_EQ(x, y)                        \
	if (x != y) {                          \
		PRINT_FAILURE;                 \
		fprintf(stderr, "Expected: "); \
		fprintf(stderr, FMT(x), x);    \
		fprintf(stderr, "\nActual: "); \
		fprintf(stderr, FMT(y), y);    \
		putchar('\n');                 \
		++num_test_failed;             \
	}

static int corn_test_main(int argc, char *argv[])
{
	int use_color = isatty(STDOUT_FILENO);
	if (use_color != 1) {
		colors[0] = colors[1] = colors[2] = "";
	}
	puts("Test start");
	struct test_case_node_t *idx = test_case_list.head;
	for (unsigned int i = 1; idx != NULL; ++i) {
		printf("Running Test %s [%u/%u]\n", idx->name + 5, i,
		       test_case_list.length);
		idx->test_case_func();
		idx = idx->next;
	}
	if (num_test_failed > 0) {
		printf("%s%d tests failed!%s\n", colors[RED], num_test_failed,
		       colors[RESET]);
	} else {
		printf("%sAll tests PASSED!%s\n", colors[GREEN], colors[RESET]);
	}
	test_case_list_free();
	return num_test_failed != 0;
}

#define TEST_MAIN()                                \
	int main(int argc, char *argv[])           \
	{                                          \
		return corn_test_main(argc, argv); \
	}

#endif // CORN_TEST_H
