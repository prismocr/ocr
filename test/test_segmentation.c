#include <unity.h>
#include "utils/matrix.h"
#include "utils/bitmap.h"
#include "segmentation/segmentation.h"

Matrix image;

void setUp() {}

void tearDown() {
    matrix_free(&image);
}

void test_segmentation() {
    Matrix image;

    TEST_ASSERT_EQUAL_INT(0, bitmap_load("./examples/text.bmp", &image));

    Page *page = NULL;
    TEST_ASSERT_EQUAL_INT(0, segment(image, &page));

    page_free(&page);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_segmentation);

    return UNITY_END();
}
