#include "utils.h"

double mtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    double mt = t.tv_sec + t.tv_usec / 1.e6;
    return mt;
}

void recurse_mkdir(char* path) {
    char* sep = strrchr(path, '/'); // ищем последний слэш
    if (sep != NULL) {  // если нашли
        *sep = 0;       // ставим конец строки
        recurse_mkdir(path);
        *sep = '/';
    }
    if (mkdir(path) && errno != EEXIST) {
        fprintf(stderr, "Error while trying to create '%s' ", path);
        perror("");
    }
}

FILE* fopen_mkdir(char* path, char* mode) {
    char* sep = strrchr(path, '/');
    if (sep) {
        char* path0 = strdup(path);
        path0[sep - path] = 0;
        recurse_mkdir(path0);
        free(path0);
    }
    return fopen(path, mode);
}

char* tmppath() {
#ifdef _WIN32
    char* pb = (char*)malloc(PATH_MAX);
    int ret = GetTempPath(PATH_MAX, pb);
    if (ret > PATH_MAX || ret == 0) {
        fprintf(stderr, "GetTempPath failed\n");
        return NULL;
    }
    return pb;
#else
    char* pb = strdup(TMP_UNIX_PATH);
    return pb;
#endif
}