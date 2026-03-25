#include "file_workers.h"

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
        if (!(!strcmp(dirent_p->d_name, "..") || !strcmp(dirent_p->d_name, "."))) { // игнорируем ссылки на себя и родителя
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

FILE* create_directory_archfile(char* path, char* tmp_path) {
    char* path_in = strdup(path);
    if (path_in == NULL) {
        perror("Err at pathname allocation");
        return NULL;
    }
    char* lastchar; // указатель на последний символ строки
    char* bslash;   // самый правый обратный слэш
    char* slash;
    while (1) {
        lastchar = path_in + strlen(path_in) - 1;
        bslash = strrchr(path_in, '\\');
        slash = strrchr(path_in, '/');
        if (bslash == lastchar) *bslash = 0;
        else if (slash == lastchar) *slash = 0;
        else break;
    }
    char* last_path;
    if (bslash != NULL) last_path = strdup(bslash + 1);
    else if (slash != NULL) last_path = strdup(slash + 1);
    else last_path = strdup(path);
    if (last_path == NULL) {
        perror("Err at last pathname allocation");
        return NULL;
    }
    free(path_in);
    FILE* sumfile = fopen(tmp_path, "wb");
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
    archive_directory(last_path, &toc, &toc_len, &toc_alloc_len);
    free(last_path);

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
    sumfile = fopen(tmp_path, "rb");
    if (sumfile == NULL) {
        perror("Err at opening directory archive file for reading");
        fprintf(stderr, "You can manually compress \"%s\"\n", tmp_path);
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
        if (toc_len >= toc_alloc_len) {
            toc_alloc_len += TOC_BUFSIZE_STEP;
            toc = (toc_entry**)realloc(toc, sizeof(toc_entry*) * toc_alloc_len);
            if (toc == NULL) {
                perror("Err at table of contents buffer reallocation");
                return -25;
            }
        }
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
        // printf("%4d: %70s | %10d\n", i, toc[i]->path, toc[i]->size);
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
    return 0;
}

int directory_handler(enum mode mode, enum info_display tinfo, char* argv[]) {
    int ret_code = 0, ret_c_fhandle;    // коды возвратов
    int is_archdir = 0;     // признак архива директории
    FILE* f_in, * f_out;
    char* tmp_path = tmppath();
    if (tmp_path == NULL) {
        perror("Err at temporary path string allocation");
        return -16;
    }
    snprintf(tmp_path, PATH_MAX, "%s%s", tmp_path, TMP_ARCHDIR_FNAME);
    double t0, t1, t2, t3;
    switch (mode) {
    case COMPRESS:
        f_out = fopen(argv[3], "wb");
        if (f_out == NULL) {
            perror("Err at output file");
            return 2;
        }
        t0 = mtime();
        FILE* archfile = create_directory_archfile(argv[2], tmp_path);
        t1 = mtime();
        if (archfile == NULL) {
            free(tmp_path);
            return -21;
        }
        if (tinfo != NONE) {
            printf("Create directory archive file: %f s\n", t1 - t0);
            printf("Compressing this file...\n");
        }
        t2 = mtime();
        fputc(1, f_out);    // это архив директории
        ret_code = file_handler(archfile, f_out, mode, tinfo, argv);
        t3 = mtime();
        fclose(archfile);
        if (remove(tmp_path)) {
            perror("Can't delete arch directory file");
            free(tmp_path);
            return -12;
        }
        if (tinfo != NONE) {
            printf("Directory archived & compressed in %f s\n", t3 - t2 + t1 - t0);
        }
        break;
    case EXTRACT:
        f_in = fopen(argv[2], "rb");
        if (f_in == NULL) {
            perror("Err at input file");
            free(tmp_path);
            return 1;
        }
        f_out = fopen(tmp_path, "wb");
        if (f_out == NULL) {
            perror("Err at temp archive file");
            free(tmp_path);
            return -15;
        }
        t0 = mtime();
        is_archdir = fgetc(f_in);
        ret_c_fhandle = file_handler(f_in, f_out, mode, tinfo, argv);
        t1 = mtime();
        fclose(f_out);
        fclose(f_in);
        if (ret_c_fhandle) {
            free(tmp_path);
            return ret_c_fhandle;
        }
        if (is_archdir) {
            t2 = mtime();
            ret_code = extract_directory_archfile(tmp_path, argv);
            t3 = mtime();
        }
        if (remove(tmp_path)) {
            perror("Can't delete arch directory file");
            free(tmp_path);
            return -12;
        }
        if (tinfo != NONE) {
            printf("Extract directory archive file: %f s\n", t3 - t2);
            printf("Directory decompressed & extracted in %f s\n", t3 - t2 + t1 - t0);
        }
        break;
    default: break;
    }
    free(tmp_path);
    return ret_code;
}