#include <unity.h>
#include "neuralnet/output.h"

// TODO: allocate pages on the heap in setup and free page in teardown
void setUp() {}

void tearDown() {}

int compareFile(FILE *f1, FILE *f2) {
    char ch1, ch2;

    int line = 1;
    int col = 0;

    while (ch1 != EOF && ch2 != EOF) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);

        if (ch1 == '\n') {
            line += 1;
            col = 0;
        }

        if (ch1 != ch2)
            return -1;

        col += 1;
    }
    return ch1 == ch2;
}

void save_pages_equal_to_file() {
    // PAGE 1
    Page p1;
    Region r11;
    Line l111;
    Word w1111;
    Word w1112;
    Word w1113;

    Line l112;
    Word w1121;

    Line l113;
    Word w1131;

    Region r12;
    Line l121;
    Word w1211;

    w1111.length = 3;
    w1112.length = 4;
    w1113.length = 7;
    w1121.length = 6;
    w1131.length = 3;
    w1211.length = 2;

    w1111.letters = "hey";
    w1112.letters = "haha";
    w1113.letters = "bonjour";
    w1121.letters = "coucou";
    w1131.letters = "ah!";
    w1211.letters = "hi";

    w1111.next = &w1112;
    w1112.next = &w1113;
    w1113.next = NULL;
    w1121.next = NULL;
    w1131.next = NULL;
    w1211.next = NULL;

    l111.next = &l112;
    l112.next = &l113;
    l113.next = NULL;
    l121.next = NULL;

    r11.next = &r12;
    r12.next = NULL;

    p1.regions = &r11;

    r11.lines = &l111;
    r12.lines = &l121;

    l111.words = &w1111;
    l112.words = &w1121;
    l113.words = &w1131;
    l121.words = &w1211;

    // PAGE 2
    Page p2;
    Region r21;
    Line l211;
    Word w2111;
    Word w2112;
    Word w2113;

    Line l212;
    Word w2121;

    Line l213;
    Word w2131;

    Region r22;
    Line l221;
    Word w2211;

    w2111.length = 3;
    w2112.length = 4;
    w2113.length = 7;
    w2121.length = 6;
    w2131.length = 3;
    w2211.length = 2;

    w2111.letters = "boo";
    w2112.letters = "adam";
    w2113.letters = "yolebro";
    w2121.letters = "cucucu";
    w2131.letters = "euh";
    w2211.letters = ":D";

    w2111.next = &w2112;
    w2112.next = &w2113;
    w2113.next = NULL;
    w2121.next = NULL;
    w2131.next = NULL;
    w2211.next = NULL;

    l211.next = &l212;
    l212.next = &l213;
    l213.next = NULL;
    l221.next = NULL;

    r21.next = &r22;
    r22.next = NULL;

    p2.regions = &r21;

    r21.lines = &l211;
    r22.lines = &l221;

    l211.words = &w2111;
    l212.words = &w2121;
    l213.words = &w2131;
    l221.words = &w2211;

    p1.next = &p2;
    p2.next = NULL;
    output_save_default(&p1, "tmppages.txt");
    FILE *f1;
    FILE *f2;

    f1 = fopen("tmppages.txt", "r");
    f2 = fopen("test/testpages.txt", "r");

    if (f2 == NULL || f2 == NULL) {
        TEST_ASSERT_EQUAL_INT(0, 1);
    }
    TEST_ASSERT_EQUAL_INT(compareFile(f1, f2), 1);
    fclose(f1);
    fclose(f2);
    remove("tmppages.txt");
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(save_pages_equal_to_file);

    return UNITY_END();
}
