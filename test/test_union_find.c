#include <unity.h>
#include "utils/union_find.h"

UnionFind testee;

void setUp() {
    uf_new(&testee);
}

void tearDown() {
    uf_free(&testee);
}

void test_new_union_find() {
    TEST_ASSERT_EQUAL_INT(0, testee.num_nodes);
}

void test_add_node() {
    int a = uf_add_node(&testee);
    TEST_ASSERT_EQUAL_INT(1, testee.num_nodes);
    TEST_ASSERT_EQUAL_INT(a, uf_find(a, &testee));

    int b = uf_add_node(&testee);
    TEST_ASSERT_EQUAL_INT(2, testee.num_nodes);
    TEST_ASSERT_EQUAL_INT(b, uf_find(b, &testee));
}

void test_union() {
    int a = uf_add_node(&testee);
    int b = uf_add_node(&testee);
    int c = uf_add_node(&testee);

    TEST_ASSERT_EQUAL_INT(3, testee.num_nodes);

    int d = uf_union(a, b, &testee);

    TEST_ASSERT_EQUAL_INT(d, uf_find(a, &testee));
    TEST_ASSERT_EQUAL_INT(d, uf_find(b, &testee));
    TEST_ASSERT_EQUAL_INT(c, uf_find(c, &testee));
    TEST_ASSERT_EQUAL_INT(d, uf_find(d, &testee));
}

void big_test() {
    for (int i = 0; i < 32; i++) {
        uf_add_node(&testee);
    }

    TEST_ASSERT_EQUAL_INT(32, testee.num_nodes);
    uf_union(1, 5, &testee);
    uf_union(17, 5, &testee);
    uf_union(17, 24, &testee);

    TEST_ASSERT_EQUAL_INT(uf_find(24, &testee), uf_find(1, &testee));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_new_union_find);
    RUN_TEST(test_add_node);
    RUN_TEST(test_union);
    RUN_TEST(big_test);

    return UNITY_END();
}
