#include <unity.h>
#include "linked_list.h"
#include "matrix.h"

MatrixLinkedList testee;

void setUp() {
    mll_new(&testee);
}

void tearDown() {
    mll_free(&testee);
}

void when_new_should_be_empty() {
    TEST_ASSERT_TRUE(mll_is_empty(testee));
    TEST_ASSERT_EQUAL_UINT8(0, testee.length);
}

void when_insert_at_the_end_should_be_present() {
    Matrix elem;
    matrix_new(1, 1, &elem);

    mll_insert(0, elem, &testee);

    TEST_ASSERT_TRUE(matrix_equal(elem, *mll_get(0, testee)));
    TEST_ASSERT_EQUAL_UINT8(1, testee.length);
}

void when_delete_first_should_shift() {
    Matrix first, second;
    matrix_new(1, 1, &first);
    matrix_new(1, 2, &second);

    mll_insert(0, first, &testee);
    mll_insert(1, second, &testee);

    TEST_ASSERT_EQUAL_UINT8(2, testee.length);

    mll_delete(0, &testee);
    matrix_free(&first);

    TEST_ASSERT_TRUE(matrix_equal(second, *mll_get(0, testee)));
    TEST_ASSERT_EQUAL_UINT8(1, testee.length);
}

void big_test() {
    Matrix elem[12];
    for (int i = 0; i < 12; i++) {
        matrix_new(1, i + 1, &elem[i]);
        mll_insert(i, elem[i], &testee);

        TEST_ASSERT_TRUE(matrix_equal(elem[i], *mll_get(i, testee)));
        TEST_ASSERT_EQUAL_UINT8(i + 1, testee.length);
    }

    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_TRUE(matrix_equal(elem[i], *mll_get(0, testee)));

        mll_delete(0, &testee);
        TEST_ASSERT_EQUAL_UINT8(12 - i - 1, testee.length);
    }
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(when_new_should_be_empty);
    RUN_TEST(when_insert_at_the_end_should_be_present);
    RUN_TEST(when_delete_first_should_shift);
    RUN_TEST(big_test);

    return UNITY_END();
}

