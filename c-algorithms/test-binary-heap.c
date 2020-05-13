/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "src/alloc-testing.h"
#include "src/framework.h"

#include "src/binary-heap.h"
#include "src/compare-int.h"

#define NUM_TEST_VALUES 10000

int test_array[NUM_TEST_VALUES];

void test_binary_heap_new_free(void)
{
	BinaryHeap *heap;
	int i;

	for (i=0; i<NUM_TEST_VALUES; ++i) {
		heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);
		binary_heap_free(heap);
	}

	/* Test low memory scenario */

	alloc_test_set_limit(0);
	heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);
	binary_heap_free(heap);
	// assert(heap == NULL);

	alloc_test_set_limit(1);
	heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);
	// assert(heap == NULL);
	binary_heap_free(heap);
}

void test_binary_heap_insert(int argc, char *argv[])
{
	BinaryHeap *heap;
	int i;

	heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);

	int len = strlen(argv[1]);

	for (i=0; i<len; ++i) {
		test_array[i] = (int)argv[1][i];
		assert(binary_heap_insert(heap, &test_array[i]) != 0);
	}

	// printf("num1 %d num2 %d \n", binary_heap_num_entries(heap), len);

	assert(binary_heap_num_entries(heap) == len);

	binary_heap_free(heap);
}

void test_min_heap(int argc, char *argv[])
{
	BinaryHeap *heap;
	int *val;
	int i;

	heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);

	/* Push a load of values onto the heap */

	int len = strlen(argv[1]);

	for (i=0; i<len; ++i) {
		test_array[i] = (int)argv[1][i];
		assert(binary_heap_insert(heap, &test_array[i]) != 0);
	}

	/* Pop values off the heap and check they are in order */

	i = -1;
	while (binary_heap_num_entries(heap) > 0) {
		val = (int *) binary_heap_pop(heap);

		// printf("val %d, i %d \n", *val, i);
		assert(*val >= i);
		i = *val;
	}

	/* Test popping from an empty heap */

	assert(binary_heap_num_entries(heap) == 0);
	assert(binary_heap_pop(heap) == BINARY_HEAP_NULL);

	binary_heap_free(heap);
}

void test_max_heap(int argc, char *argv[])
{
	BinaryHeap *heap;
	int *val;
	int i;

	heap = binary_heap_new(BINARY_HEAP_TYPE_MAX, int_compare);

	/* Push a load of values onto the heap */

	int len = strlen(argv[1]);

	for (i=0; i<len; ++i) {
		test_array[i] = (int)argv[1][i];
		assert(binary_heap_insert(heap, &test_array[i]) != 0);
	}

	/* Pop values off the heap and check they are in order */

	i = NUM_TEST_VALUES * (len + 1);
	while (binary_heap_num_entries(heap) > 0) {
		val = (int *) binary_heap_pop(heap);

		assert(*val <= i);
		i = *val;
	}

	binary_heap_free(heap);
}

/* Test out of memory scenario when adding items */

void test_out_of_memory(void)
{
	BinaryHeap *heap;
	int *value;
	int values[] = {
		15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
	};
	int i;

	/* Allocate a heap and fill to the default limit */

	heap = binary_heap_new(BINARY_HEAP_TYPE_MIN, int_compare);

	alloc_test_set_limit(0);

	for (i=0; i<16; ++i) {
		assert(binary_heap_insert(heap, &values[i]) != 0);
	}

	assert(binary_heap_num_entries(heap) == 16);

	/* Check that we cannot add new values */

	for (i=0; i<16; ++i) {
		assert(binary_heap_insert(heap, &values[i]) == 0);
		assert(binary_heap_num_entries(heap) == i);
	}

	/* Check that we can read the values back out again and they
	 * are in the right order. */

	for (i=0; i<16; ++i) {
		value = binary_heap_pop(heap);
		assert(*value == i);
	}

	assert(binary_heap_num_entries(heap) == 0);

	binary_heap_free(heap);
}

static UnitTestFunction tests[] = {
	// test_binary_heap_new_free,
	// test_binary_heap_insert,
	// test_min_heap,
	// test_max_heap,
	// test_out_of_memory,
	NULL
};

int main(int argc, char *argv[])
{
	test_binary_heap_new_free();
	
	test_binary_heap_insert(argc, argv);
	
	test_min_heap(argc, argv);

	test_max_heap(argc, argv);

	// run_tests(tests);

	return 0;
}

