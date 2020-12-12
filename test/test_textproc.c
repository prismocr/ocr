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
    find_closest_word(&dict, "three", r);
    TEST_ASSERT_EQUAL_INT(0, strcmp("three", r));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_textproc);

    return UNITY_END();
}
