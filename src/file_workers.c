#include "file_workers.h"

void file_handler(FILE* f_in, FILE* f_out, enum mode mode, enum info_display tinfo, char* argv[]) {
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
        if (tinfo != NONE) printf("reading: %f s\n", r1 - r0);

        // Расчёт частоты встречаемости
        for (int i = 0; i < uniqk; ++i) simbols[i].freq = (float)kolvo[i] / kk;

        // В массив указателей psym заносим адреса записей
        for (int i = 0; i < uniqk; ++i) psym[i] = simbols + i;

        //Сортировка по убыванию по частоте
        descend_sort(psym, uniqk);

        double mt0 = mtime();
        symbol* root = makeTree(psym, uniqk);   //создание дерева Хаффмана
        double mt1 = mtime();
        makeCodes(root);    //получение кодов
        double mc2 = mtime();
        rewind(f_in);   //возвращаем указатель в файле в начало файла
        if (tinfo != NONE) printf("make tree: %f s\nmake codes: %f s\n", mt1 - mt0, mc2 - mt1);
        // double ts0 = mtime();
        // compress_to_file(f_in, f_out, simbols, uniqk, &fsize2);    // медленно
        // double ts1 = mtime();
        // rewind(f_in);
        // if (tinfo != NONE) printf("1 write: %f s\n", ts1 - ts0);
        double t0 = mtime();
        write_code_table(f_out, simbols, uniqk, kk);
        compress_to_file_simb(f_in, f_out, simbols, uniqk, &fsize2);    // более быстрый способ без промежуточного файла
        double t1 = mtime();
        if (tinfo != NONE) {
            if (tinfo == FULL) show_information(simbols, uniqk, kk, fsize2);
            printf("fast write: %f s\n", t1 - t0);
            printf("File compressed to \"%s\" in %f s\n", argv[3], t1 - r0);
        }
    } else if (mode == EXTRACT) {
        double te0 = mtime();
        uniqk = read_code_table(f_in, simbols);
        extract_from_file(f_in, f_out, simbols, uniqk);
        double te1 = mtime();
        if (tinfo != NONE) printf("File extracted in %f s\n", te1 - te0);
    }
}

void archive_directory(char* path, char* toc_rootpath, toc_entry** toc[], unsigned* toc_len, unsigned* toc_alloc_len) {
    DIR* dd = opendir(path);
    check_log_err_exit(dd != NULL, "Directory opening", 1, 6);
    struct dirent* dirent_p;
    while ((dirent_p = readdir(dd)) != NULL) {
        struct stat st;
        if (!(!strcmp(dirent_p->d_name, "..") || !strcmp(dirent_p->d_name, "."))) { // игнорируем ссылки на себя и родителя
            char* child_fname = (char*)malloc(strlen(path) + 1 + dirent_p->d_namlen + 1);   // полный путь
            char* toc_child_fname = (char*)malloc(strlen(toc_rootpath) + 1 + dirent_p->d_namlen + 1);   // путь, начиная с общей директории
            check_log_err_exit(!(child_fname == NULL || toc_child_fname == NULL), "Path name buffers memory allocation", 1, 7);
            snprintf(child_fname, PATH_MAX, "%s/%s", path, dirent_p->d_name);
            snprintf(toc_child_fname, PATH_MAX, "%s/%s", toc_rootpath, dirent_p->d_name);
            if (stat(child_fname, &st)) {
                fprintf(stderr, "Unable to stat %s\n", child_fname);
                continue;
            }
            if (S_ISDIR(st.st_mode)) {
                archive_directory(child_fname, toc_child_fname, toc, toc_len, toc_alloc_len);
            } else if (st.st_size <= UINT_MAX) {
                if (*toc_len >= *toc_alloc_len) {
                    *toc_alloc_len += TOC_BUFSIZE_STEP;
                    *toc = (toc_entry**)realloc(*toc, sizeof(toc_entry*) * (*toc_alloc_len));
                    check_log_err_exit(*toc != NULL, "Table of contents buffer memory reallocation", 1, 8);
                }
                (*toc)[*toc_len] = (toc_entry*)malloc(sizeof(toc_entry));
                check_log_err_exit((*toc)[*toc_len] != NULL, "Table of contents entry memory allocation", 1, 9);
                (*toc)[*toc_len]->path = strdup(toc_child_fname);
                check_log_err_exit((*toc)[*toc_len]->path != NULL, "Table of contents entry path memory allocation", 1, 10);
                (*toc)[*toc_len]->size = st.st_size;
                ++(*toc_len);
            } else printf("Skipped large file: '%s'\n", child_fname);
            free(child_fname);
        }
    }
    closedir(dd);
}

FILE* create_directory_archfile(char* path, char* tmp_path, enum info_display tinfo) {
    char* path_in = strdup(path);   // копия входного пути, чтобы изменять его
    check_log_err_exit(path_in != NULL, "Input pathname memory allocation", 1, 11);
    char* lastchar; // указатель на последний символ строки
    char* bslash;   // самый правый обратный слэш
    char* slash;
    while (1) {     // отрезаем все последние \ и /
        lastchar = path_in + strlen(path_in) - 1;
        bslash = strrchr(path_in, '\\');
        slash = strrchr(path_in, '/');
        if (bslash == lastchar) *bslash = 0;
        else if (slash == lastchar) *slash = 0;
        else break;
    }
    char* last_path;
    if (bslash != NULL) last_path = strdup(bslash + 1); // отрезаем всё до самой вложенной директории
    else if (slash != NULL) last_path = strdup(slash + 1);
    else last_path = strdup(path_in);
    check_log_err_exit(last_path != NULL, "Last pathname memory allocation", 1, 1);
    FILE* sumfile = fopen(tmp_path, "wb");
    check_log_err_exit(sumfile != NULL, "Directory archive file opening for writing", 1, 13);

    toc_entry** toc = (toc_entry**)malloc(sizeof(toc_entry*) * TOC_BUFSIZE_STEP);   // массив записей оглавления
    check_log_err_exit(toc != NULL, "Table of contents buffer memory allocation", 1, 14);
    unsigned toc_len = 0, toc_alloc_len = TOC_BUFSIZE_STEP;
    archive_directory(path, last_path, &toc, &toc_len, &toc_alloc_len);
    free(last_path);

    for (int i = 0; i < toc_len; ++i) {     // запись оглавления
        fwrite(toc[i]->path, 1, strlen(toc[i]->path), sumfile);
        fputc('"', sumfile);
        fwrite(&(toc[i]->size), sizeof(toc[i]->size), 1, sumfile);
        if (tinfo == FULL) printf("%4d: %70s | %10d\n", i, toc[i]->path, toc[i]->size);
    }
    fputc('"', sumfile);
    if (tinfo != NONE) printf("Total number of files: %d\n", toc_len);

    char* transfer_buf = (char*)malloc(TRANSFER_BUF_SIZE);
    check_log_err_exit(transfer_buf != NULL, "Transfer buffer memory allocation", 1, 15);
    for (int i = 0; i < toc_len; ++i) {
        slash = strchr(toc[i]->path, '/');
        char pathb[PATH_MAX];
        snprintf(pathb, PATH_MAX, "%s%s", path_in, slash);  // получение настоящего пути из оглавления и аргументов
        FILE* targetf = fopen(pathb, "rb");
        if (targetf == NULL) {
            fprintf(stderr, "Err at opening file: '%s'\n", toc[i]->path);
            check_log_err_exit(targetf != NULL, "File opening", 1, 16);
        }
        while (1) {         // копирование содержимого файла
            int read_num = fread(transfer_buf, 1, TRANSFER_BUF_SIZE, targetf);
            fwrite(transfer_buf, 1, read_num, sumfile);
            if (feof(targetf)) break;
        }
        fclose(targetf);
        free(toc[i]->path);
        free(toc[i]);
    }
    free(toc);
    free(path_in);
    fclose(sumfile);
    sumfile = fopen(tmp_path, "rb");
    if (sumfile == NULL) {
        fprintf(stderr, "You can manually compress \"%s\"\n", tmp_path);
        check_log_err_exit(sumfile != NULL, "Directory archive file opening for reading", 1, 17);
    }
    return sumfile;
}

void extract_directory_archfile(char* fname, enum info_display tinfo, char* argv[]) {
    toc_entry** toc = (toc_entry**)malloc(sizeof(toc_entry*) * TOC_BUFSIZE_STEP);
    check_log_err_exit(toc != NULL, "Table of contents buffer memory allocation", 1, 18);
    unsigned toc_len = 0, toc_alloc_len = TOC_BUFSIZE_STEP;
    FILE* archf = fopen(fname, "rb");
    check_log_err_exit(archf != NULL, "Directory archive file opening", 1, 19);
    unsigned char c;
    char tocpath_buf[PATH_MAX];
    int tocpath_cnt = 0;
    enum state { FPATH, FSIZE } state = FPATH;
    while (1) {                 // чтение оглавления
        if (toc_len >= toc_alloc_len) {
            toc_alloc_len += TOC_BUFSIZE_STEP;
            toc = (toc_entry**)realloc(toc, sizeof(toc_entry*) * toc_alloc_len);
            check_log_err_exit(toc != NULL, "Table of contents buffer memory reallocation", 1, 20);
        }
        toc[toc_len] = (toc_entry*)malloc(sizeof(toc_entry));
        check_log_err_exit(toc[toc_len] != NULL, "Table of contents entry memory allocation", 1, 21);
        if (state != FSIZE) c = fgetc(archf);
        if (state == FPATH) {
            if (c != '"') {
                tocpath_buf[tocpath_cnt] = c;
                ++tocpath_cnt;
                check_log_err_exit(tocpath_cnt < PATH_MAX, "Read path is too long", 0, 22);
            } else if (!tocpath_cnt) break;     // конец оглавления
            else state = FSIZE;
        } else if (state == FSIZE) {
            tocpath_buf[tocpath_cnt] = 0;
            ++tocpath_cnt;
            toc[toc_len]->path = (char*)malloc(tocpath_cnt);
            check_log_err_exit(toc[toc_len]->path != NULL, "Table of contents file path memory allocation", 1, 23);
            memcpy(toc[toc_len]->path, tocpath_buf, tocpath_cnt);
            tocpath_cnt = 0;
            fread(&(toc[toc_len]->size), sizeof(toc[toc_len]->size), 1, archf);
            ++toc_len;
            state = FPATH;
        }
    }

    char* transfer_buf = (char*)malloc(TRANSFER_BUF_SIZE);
    check_log_err_exit(transfer_buf != NULL, "Transfer buffer memory allocation", 1, 24);
    for (int i = 0; i < toc_len; ++i) {
        char out_path[PATH_MAX];
        snprintf(out_path, PATH_MAX, "%s/%s", argv[3], toc[i]->path);   // есть ограничение на длину пути
        FILE* fout = fopen_mkdir(out_path, "wb");
        if (fout == NULL) {
            perror("Err at one of output target files");
            fprintf(stderr, "Skipped\n");
            continue;
        }
        if (tinfo == FULL) printf("%4d: %70s | %10d\n", i, toc[i]->path, toc[i]->size);
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

void directory_handler(enum mode mode, enum info_display tinfo, char* argv[]) {
    unsigned char arch_marker = 0;      // признак правильности с вероятностью 254/256 = 99,2%
    FILE* f_in, * f_out;
    char* tmp_path = tmppath();
    check_log_err_exit(tmp_path != NULL, "Temporary directory path string creation", 1, 25);
    snprintf(tmp_path, PATH_MAX, "%s/%s", tmp_path, TMP_ARCHDIR_FNAME);

    double t0, t1, t2, t3;
    switch (mode) {
    case COMPRESS:
        f_out = fopen(argv[3], "wb");
        check_log_err_exit(f_out != NULL, "Output file opening", 1, 26);
        t0 = mtime();
        FILE* archfile = create_directory_archfile(argv[2], tmp_path, tinfo);
        t1 = mtime();
        if (tinfo != NONE) {
            printf("Create directory archive file: %f s\n", t1 - t0);
            printf("Compressing this file...\n");
        }
        t2 = mtime();
        fputc(ARCHIVE_IS_DIR, f_out);   // это архив директории
        file_handler(archfile, f_out, mode, tinfo, argv);
        t3 = mtime();
        fclose(archfile);
        check_log_err_exit(!remove(tmp_path), "Temporary directory archive file deletion", 1, 27);
        if (tinfo != NONE) {
            printf("Directory archived & compressed in %f s\n", t3 - t2 + t1 - t0);
        }
        break;
    case EXTRACT:
        f_in = fopen(argv[2], "rb");
        check_log_err_exit(f_in != NULL, "Input file opening", 1, 28);
        arch_marker = fgetc(f_in);
        if ((arch_marker == ARCHIVE_IS_FILE || arch_marker == ARCHIVE_IS_DIR) && !feof(f_in)) { // правильный непустой входной файл
            f_out = fopen(arch_marker == ARCHIVE_IS_DIR ? tmp_path : argv[3], "wb");
            check_log_err_exit(f_out != NULL, "Temporary directory archive file opening", 1, 29);
            t0 = mtime();
            file_handler(f_in, f_out, mode, tinfo, argv);
            t1 = mtime();
            fclose(f_out);
            fclose(f_in);
            if (arch_marker == ARCHIVE_IS_DIR) {
                t2 = mtime();
                extract_directory_archfile(tmp_path, tinfo, argv);
                t3 = mtime();
                check_log_err_exit(!remove(tmp_path), "Temporary directory archive file deletion", 1, 30);
            }
            if (tinfo != NONE) {
                printf("Extract directory archive file: %f s\n", t3 - t2);
                printf("%s decompressed & extracted in %f s\n", arch_marker == ARCHIVE_IS_DIR ? "Directory" : "File", t3 - t2 + t1 - t0);
            }
        } else {
            fprintf(stderr, "File is incorrect\n");
        }
        break;
    default: break;
    }
    free(tmp_path);
}