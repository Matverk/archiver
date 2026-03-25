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
        fprintf(stderr, "error while trying to create '%s'", path);
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

int file_handler(FILE* f_in, FILE* f_out, enum mode mode, enum info_display tinfo, char* argv[]) {
    int uniqk = 0;      // счётчик количества различных букв, уникальных символов
    symbol simbols[256] = { 0 };    // массив записей
    if (mode == COMPRESS) {
        int kk = 0;         // счётчик количества всех знаков в файле
        int kolvo[256] = { 0 };     // массив количеств уникальных символов
        symbol* psym[256];  // инициализируем массив указателей на записи
        int fsize2 = 0;     // счётчик количества бит в сжатом файле
        double r0 = mtime();
        reading_from_file(f_in, simbols, kolvo, &kk, &uniqk);
        double r1 = mtime();

        // Расчёт частоты встречаемости
        for (int i = 0; i < uniqk; ++i) simbols[i].freq = (float)kolvo[i] / kk;

        // В массив указателей psym заносим адреса записей
        for (int i = 0; i < uniqk; ++i) psym[i] = simbols + i;

        //Сортировка по убыванию по частоте
        descend_sort(psym, uniqk);

        double mt0 = mtime();
        symbol* root = makeTree(psym, uniqk);   //создание дерева Хаффмана
        if (root == NULL) return -10;
        double mt1 = mtime();
        makeCodes(root);    //получение кодов
        double mc2 = mtime();
        rewind(f_in);   //возвращаем указатель в файле в начало файла
        // double ts0 = mtime();
        // compress_to_file(f_in, f_out, simbols, uniqk, &fsize2);    // медленно
        // double ts1 = mtime();
        // rewind(f_in);
        double t0 = mtime();
        write_code_table(f_out, simbols, uniqk, kk);
        compress_to_file_simb(f_in, f_out, simbols, uniqk, &fsize2);    // более быстрый способ без промежуточного файла
        double t1 = mtime();
        if (tinfo != NONE) {
            if (tinfo == FULL) show_information(simbols, uniqk, kk, fsize2);
            printf("reading: %f s\n", r1 - r0);
            printf("make tree: %f s\nmake codes: %f s\n", mt1 - mt0, mc2 - mt1);
            // printf("1 write: %f s\n", ts1 - ts0);
            printf("fast write: %f s\n", t1 - t0);
            printf("File compressed to \"%s\" in %f s\n", argv[3], t1 - r0);
        }
    } else if (mode == EXTRACT) {
        double te0 = mtime();
        uniqk = read_code_table(f_in, simbols);
        if (extract_from_file(f_in, f_out, simbols, uniqk)) return -11;
        double te1 = mtime();
        if (tinfo != NONE) printf("File extracted in %f s\n", te1 - te0);
    }
    return 0;
}

void archive_directory(char* path, toc_entry** toc[], unsigned* toc_len, unsigned* toc_alloc_len) {
    DIR* dd = opendir(path);
    if (dd == NULL) {
        perror("Can't open directory");
        return;
    }
    struct dirent* dirent_p;
    while ((dirent_p = readdir(dd)) != NULL) {
        struct stat st;
        if (!(!strcmp(dirent_p->d_name, "..") || !strcmp(dirent_p->d_name, "."))) {
            char* child_fname = (char*)malloc(strlen(path) + 1 + dirent_p->d_namlen + 1);
            if (child_fname == NULL) {
                perror("Err at path name buff allocation");
                closedir(dd);
                return;
            }
            sprintf(child_fname, "%s/%s", path, dirent_p->d_name);
            if (stat(child_fname, &st)) {
                fprintf(stderr, "Unable to stat %s\n", child_fname);
                continue;
            }
            if (S_ISDIR(st.st_mode)) {
                archive_directory(child_fname, toc, toc_len, toc_alloc_len);
            } else if (st.st_size <= UINT_MAX) {
                if (*toc_len >= *toc_alloc_len) {
                    *toc_alloc_len += TOC_BUFSIZE_STEP;
                    *toc = (toc_entry**)realloc(*toc, sizeof(toc_entry*) * (*toc_alloc_len));
                    if (*toc == NULL) {
                        perror("Err at table of contents buffer reallocation");
                        closedir(dd);
                        return;
                    }
                }
                (*toc)[*toc_len] = (toc_entry*)malloc(sizeof(toc_entry));
                if ((*toc)[*toc_len] == NULL) {
                    perror("Err at allocating table of contents entry");
                    closedir(dd);
                    return;
                }
                (*toc)[*toc_len]->path = (char*)malloc(strlen(child_fname) + 1);
                if ((*toc)[*toc_len]->path == NULL) {
                    perror("Err at allocating table of contents entry path");
                    closedir(dd);
                    return;
                }
                strcpy((*toc)[*toc_len]->path, child_fname);
                (*toc)[*toc_len]->size = st.st_size;
                // printf("%3d file #%d: %s, size=%d B\n", *toc_alloc_len, *toc_len, child_fname, st.st_size);
                ++(*toc_len);
            } else printf("Skipped large file: %s\n", child_fname);
            free(child_fname);
        }
    }
    closedir(dd);
}

FILE* create_directory_archfile(char* path) {
    FILE* sumfile = fopen(TMP_ARCHDIR_FNAME, "wb");
    if (sumfile == NULL) {
        perror("Err at opening directory archive file for writing");
        return NULL;
    }

    toc_entry** toc = (toc_entry**)malloc(sizeof(toc_entry*) * TOC_BUFSIZE_STEP);   // массив записей оглавления
    if (toc == NULL) {
        perror("Err at table of contents buffer allocation");
        fclose(sumfile);
        return NULL;
    }
    unsigned toc_len = 0, toc_alloc_len = TOC_BUFSIZE_STEP;
    archive_directory(path, &toc, &toc_len, &toc_alloc_len);

    for (int i = 0; i < toc_len; ++i) {     // запись оглавления
        fwrite(toc[i]->path, 1, strlen(toc[i]->path), sumfile);
        fputc('"', sumfile);
        fwrite(&(toc[i]->size), sizeof(toc[i]->size), 1, sumfile);
        // printf("%4d: %70s | %10d\n", i, toc[i]->path, toc[i]->size);
    }
    fputc('"', sumfile);

    char* transfer_buf = (char*)malloc(TRANSFER_BUF_SIZE);
    if (transfer_buf == NULL) {
        perror("Err at transfer buffer allocation");
        fclose(sumfile);
        return NULL;
    }
    for (int i = 0; i < toc_len; ++i) {
        FILE* targetf = fopen(toc[i]->path, "rb");
        if (targetf == NULL) {
            fprintf(stderr, "Err at opening file: %s:", toc[i]->path);
            perror("");
            fclose(sumfile);
            return NULL;
        }
        while (1) {         // копирование содержимого файла
            int read_num = fread(transfer_buf, 1, TRANSFER_BUF_SIZE, targetf);
            fwrite(transfer_buf, 1, read_num, sumfile);
            if (feof(targetf)) break;
        }
        free(toc[i]->path);
        free(toc[i]);
    }
    free(toc);
    fclose(sumfile);
    sumfile = fopen(TMP_ARCHDIR_FNAME, "rb");
    if (sumfile == NULL) {
        perror("Err at opening directory archive file for reading");
        fprintf(stderr, "You can manually compress \"%s\"\n", TMP_ARCHDIR_FNAME);
        return NULL;
    }
    return sumfile;
}

int extract_directory_archfile(char* fname, char* argv[]) {
    toc_entry** toc = (toc_entry**)malloc(sizeof(toc_entry*) * TOC_BUFSIZE_STEP);
    if (toc == NULL) {
        perror("Err at table of contents buffer allocation");
        return -21;
    }
    unsigned toc_len = 0, toc_alloc_len = TOC_BUFSIZE_STEP;
    FILE* archf = fopen(fname, "rb");
    if (archf == NULL) {
        perror("Err at open directory archive file");
        return -22;
    }
    unsigned char c;
    char tocpath_buf[PATH_MAX];
    int tocpath_cnt = 0, fsize_read_flag = 0;
    enum state { FPATH, FSIZE } state = FPATH;
    while (1) {                 // чтение оглавления
        toc[toc_len] = (toc_entry*)malloc(sizeof(toc_entry));
        if (toc[toc_len] == NULL) {
            perror("Err at table of cont entry alloc");
            return -23;
        }
        if (state != FSIZE) c = fgetc(archf);
        if (state == FPATH) {
            if (c != '"') {
                tocpath_buf[tocpath_cnt] = c;
                ++tocpath_cnt;
            } else if (!tocpath_cnt) break;
            else state = FSIZE;
        } else if (state == FSIZE) {
            if (toc_len >= toc_alloc_len) {
                toc_alloc_len += TOC_BUFSIZE_STEP;
                toc = (toc_entry**)realloc(toc, sizeof(toc_entry*) * toc_alloc_len);
                if (toc == NULL) {
                    perror("Err at table of contents buffer reallocation");
                    return -25;
                }
            }
            tocpath_buf[tocpath_cnt] = 0;
            ++tocpath_cnt;
            toc[toc_len]->path = (char*)malloc(tocpath_cnt);
            if (toc[toc_len]->path == NULL) {
                perror("Err at toc fpath alloc");
                return -24;
            }
            memcpy(toc[toc_len]->path, tocpath_buf, tocpath_cnt);
            tocpath_cnt = 0;
            fread(&(toc[toc_len]->size), sizeof(toc[toc_len]->size), 1, archf);
            ++toc_len;
            state = FPATH;
        }
    }

    char* transfer_buf = (char*)malloc(TRANSFER_BUF_SIZE);
    if (transfer_buf == NULL) {
        perror("Err at transfer buffer allocation");
        fclose(archf);
        return -26;
    }
    for (int i = 0; i < toc_len; ++i) {
        printf("%4d: %70s | %10d\n", i, toc[i]->path, toc[i]->size);
        char out_path[PATH_MAX];
        sprintf(out_path, "%s/%s", argv[3], toc[i]->path);
        FILE* fout = fopen_mkdir(out_path, "wb");
        if (fout == NULL) {
            perror("Err at one of output target files");
            fprintf(stderr, "Skipped\n");
            continue;
        }
        unsigned transfer_left = toc[i]->size;
        while (1) {                     // копирование содержимого из архива по файлам
            int minsize = TRANSFER_BUF_SIZE < transfer_left ? TRANSFER_BUF_SIZE : transfer_left;
            int read_cnt = fread(transfer_buf, 1, minsize, archf);
            transfer_left -= read_cnt;
            fwrite(transfer_buf, 1, read_cnt, fout);
            if (transfer_left <= 0) break;
        }
        fclose(fout);
        free(toc[i]->path);
        free(toc[i]);
    }
    fclose(archf);
    free(toc);
}

int directory_handler(enum mode mode, enum info_display tinfo, char* argv[]) {
    int ret_code = 0, ret_c_fhandle;
    int is_archdir = 0;     // признак архива директории
    FILE* f_in, * f_out;
    switch (mode) {
    case COMPRESS:
        f_out = fopen(argv[3], "wb");
        if (f_out == NULL) {
            perror("Err at output file");
            return 2;
        }
        double dc0 = mtime();
        FILE* archfile = create_directory_archfile(argv[2]);
        double dc1 = mtime();
        if (archfile == NULL) return -21;
        if (tinfo != NONE) {
            printf("Create directory archive file: %f s\n", dc1 - dc0);
            printf("Compressing this file...\n");
        }
        double dc2 = mtime();
        fputc(1, f_out);    // это архив директории
        ret_code = file_handler(archfile, f_out, mode, tinfo, argv);
        double dc3 = mtime();
        fclose(archfile);
        if (remove(TMP_ARCHDIR_FNAME)) {
            perror("Can't delete arch directory file");
            return -12;
        }
        if (tinfo != NONE) {
            printf("Directory archived & compressed in %f s\n", dc3 - dc2 + dc1 - dc0);
        }
        break;
    case EXTRACT:
        f_in = fopen(argv[2], "rb");
        if (f_in == NULL) {
            perror("Err at input file");
            return 1;
        }
        f_out = fopen(TMP_ARCHDIR_FNAME, "wb");
        if (f_out == NULL) {
            perror("Err at temp archive file");
            return -15;
        }
        is_archdir = fgetc(f_in);
        ret_c_fhandle = file_handler(f_in, f_out, mode, tinfo, argv);
        fclose(f_out);
        fclose(f_in);
        if (ret_c_fhandle) return -14;
        if (is_archdir) ret_code = extract_directory_archfile(TMP_ARCHDIR_FNAME, argv);
        if (remove(TMP_ARCHDIR_FNAME)) {
            perror("Can't delete arch directory file");
            return -12;
        }
        break;
    default: break;
    }
    return ret_code;
}