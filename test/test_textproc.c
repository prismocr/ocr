#include <unity.h>
#include "textproc/dict.h"
#include "textproc/text.h"
#include "string.h"

Matrix image;
Matrix expected;

void setUp() {}

void tearDown() {}

void test_textproc() {
    Dict dict;
    dict_load("./res/dictionaries/words_en.txt", &dict);
    char *r = calloc(100, sizeof(char));
    find_closest_word(&dict, "three", r, 0);
    TEST_ASSERT_EQUAL_INT(0, strcmp("three", r));

    find_closest_word(&dict, "thonk", r, 0);
    TEST_ASSERT_EQUAL_INT(0, strcmp("thank/think/thong/thunk/tronk", r));

    find_closest_word(&dict, "a", r, 0);
    TEST_ASSERT_EQUAL_INT(0, strcmp("a", r));

    find_closest_word(&dict, "is", r, 0);
    TEST_ASSERT_EQUAL_INT(0, strcmp("is", r));

    find_closest_word(&dict, "log", r, 0);
    TEST_ASSERT_EQUAL_INT(0, strcmp("log", r));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_textproc);

    return UNITY_END();
}
