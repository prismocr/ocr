#include <unity.h>
#include "utils/matrix.h"
#include "utils/bitmap.h"
#include "imgproc/image.h"

Matrix image;
Matrix expected;

void setUp() {}

void tearDown() {
    matrix_free(&image);
}

void test_trim() {
    TEST_ASSERT_EQUAL_INT(0, bitmap_load("./test/assets/trim/2.bmp", &image));

    TEST_ASSERT_EQUAL_INT(37, image_bound_left(&image));
    TEST_ASSERT_EQUAL_INT(51, image_bound_right(&image));
    TEST_ASSERT_EQUAL_INT(187, image_bound_top(&image));
    TEST_ASSERT_EQUAL_INT(206, image_bound_bottom(&image));

    TEST_ASSERT_EQUAL_INT(0,
                          bitmap_load("./test/assets/trim/Aseg.bmp", &image));

    TEST_ASSERT_EQUAL_INT(3, image_bound_left(&image));
    TEST_ASSERT_EQUAL_INT(image.w, image_bound_right(&image));
    TEST_ASSERT_EQUAL_INT(0, image_bound_top(&image));
    TEST_ASSERT_EQUAL_INT(image.h - 1, image_bound_bottom(&image));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_trim);

    return UNITY_END();
}
