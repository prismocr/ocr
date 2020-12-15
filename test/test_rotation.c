#include <unity.h>
#include "utils/matrix.h"
#include "utils/bitmap.h"
#include "imgproc/rotation.h"

Matrix image;

void setUp() {}

void tearDown() {
    matrix_free(&image);
}

void detect_skew() {
    // TEST_ASSERT_EQUAL_INT(0,
    //                       bitmap_load("./res/images/text_skew_17.bmp",
    //                       &image));
    // float angle = image_detect_skew(&image, 0.01f);
    // // rad_to_deg(angle);
    // TEST_ASSERT_EQUAL_FLOAT(17.f, roundf(rad_to_deg(angle)));

    // TEST_ASSERT_EQUAL_INT(0,
    //                       bitmap_load("./res/images/text_skew_60.bmp",
    //                       &image));
    // angle = image_detect_skew(&image, 0.01f);
    // // rad_to_deg(angle);
    // TEST_ASSERT_EQUAL_FLOAT(-60.f, roundf(rad_to_deg(angle)));

    // TEST_ASSERT_EQUAL_INT(0, bitmap_load("./res/images/text.bmp", &image));
    // angle = image_detect_skew(&image, 0.01f);
    // // rad_to_deg(angle);
    // TEST_ASSERT_EQUAL_FLOAT(0.f, roundf(rad_to_deg(angle)));

    // TEST_ASSERT_EQUAL_INT(0, bitmap_load("./res/images/text_skew.bmp",
    // &image)); angle = image_detect_skew(&image, 0.01f);
    // // rad_to_deg(angle);
    // TEST_ASSERT_EQUAL_FLOAT(4.f, roundf(rad_to_deg(angle)));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(detect_skew);

    return UNITY_END();
}
