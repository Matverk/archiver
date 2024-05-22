#include "utils.h"

double mtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    double mt = t.tv_sec + t.tv_usec / 1.e6;
    return mt;
}