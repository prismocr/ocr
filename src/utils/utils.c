#include "utils/utils.h"

inline int min3(int a, int b, int c) {
    return a < b ? min2(a, c) : min2(b, c);
}

inline int min2(int a, int b) {
    return a < b ? a : b;
}

inline int max2(int a, int b) {
    return a > b ? a : b;
}
