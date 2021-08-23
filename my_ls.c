/*************************************************************************
    > File Name: 2.ls.c
    > Author: LYX
    > Mail: 
    > Created Time: Sat 03 Jul 2021 06:37:40 PM CST
 ************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include "my_ls.h"

static int a_flag;
static int l_flag;
static int file_flag; //文件还是目录
static int col; // 终端的宽度 列数

static int (*printls)(dir *); //便于程序扩展 采用函数指针实现调用不同的打印函数

void get_file_mode(int mode, char *buff) {
    // 第一字段
    char tmp_mode[3] = {'r', 'w', 'x'};
    switch (mode & S_IFMT) {
        case S_IFBLK: 
            strcat(buff, "b");
            break;
        case S_IFCHR:
            strcat(buff, "c");
            break;
        case S_IFDIR:
            strcat(buff, "d");
            break;
        case S_IFIFO:
            strcat(buff, "p");
            break;
        case S_IFLNK:
            strcat(buff, "l");
            break;
        case S_IFSOCK:
            strcat(buff, "s");
            break;
        default:
            strcat(buff, "-");
    }
    int cnt = -1;
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            cnt++;
            if ((mode & (0x0100 >> cnt))) {
                strncat(buff, &tmp_mode[i], 1);    
            } else {
                strcat(buff, "-");
            }
        }
    }
}

int get_digit_len (long int n) {
    char buf[BUFSIZ];
    snprintf(buf, BUFSIZ, "%ld", n);
    return (int)strlen(buf);
}

int get_string_len(char *str) {
    char buf[BUFSIZ];
    return sprintf(buf, "%s", str);
}

int cmp_name(const void *s1, const void *s2) {
    char *name1 = ((file*)s1)->name;
    char *name2 = ((file*)s2)->name;
    if (strcmp(name1, ".") == 0 || strcmp(name2, ".") == 0) {
        return strcmp(name1, name2);
    }
    if (name1[0] == '.' && name2[0] == '.') {
        return strcasecmp(&name1[1], &name2[1]);
    } 
    if (name1[0] == '.') {
        return strcasecmp(&name1[1], name2);
    }
    if (name2[0] == '.') {
        return strcasecmp(name1, &name2[1]);
    }
    return strcasecmp(name1, name2);
}

int sort_dir(dir *dp, int (*cmp)(const void *, const void *)) {
    if (dp == NULL || dp->filep == NULL) return -1;
    for (int i = 0; i < dp->cnt; i++) { // 寻找可能存在的子目录
        if (dp->filep[i].dirp) {
            sort_dir(dp->filep[i].dirp, cmp);
        }
    }
    qsort(dp->filep, dp->cnt, sizeof(file), cmp);
    return 0;
}

int free_dirs(dir *dp) {
    if (dp == NULL || dp->filep == NULL) return -1;
    for (int i = 0; i < dp->cnt; i++) {
        free_dirs(dp->filep[i].dirp);
    }
    free(dp->filep);
    free(dp);
    return 0;
}

void copyinfo(file *fp, const struct stat *sp, const char *pathname) {
    fp->dirp = NULL;
    fp->ino = (int)sp->st_ino;
    fp->uid = (int)sp->st_uid;
    fp->gid = (int)sp->st_gid;
    fp->dev = (int)sp->st_rdev;
    fp->mode = (int)sp->st_mode;
    fp->size = (int)sp->st_size;
    fp->atime = (int)sp->st_atime;
    fp->mtime = (int)sp->st_mtime;
    fp->ctime = (int)sp->st_ctime;
    fp->links = (int)sp->st_nlink;
    fp->length = (int)strlen(pathname);
    strcpy(fp->name, pathname);
}

void print_file_name(int mode, char *name) {
    switch (mode & S_IFMT) {
        case S_IFBLK: 
        case S_IFCHR:
            printf("\033[1m");
            printf("\033[40m");
            printf("\033[33m%s\033[0m", name);
            printf("\033[0m");
            break;
        case S_IFDIR:
            printf("\033[34m%s\033[0m", name);
            break;
        case S_IFLNK:
            printf("\033[1m");
            printf("\033[36m%s\033[0m", name);
            printf("\033[0m");
            break;
        default:
            if (access(name, X_OK) == 0) {
                printf("\033[32m%s\033[0m", name);
            } else {
                printf("%s", name);
            }
    }
    printf("\n");
}

void print_file_time(int atime) {
    char time[40] = {0};
    char *sub = NULL;
    strcpy(time, ctime((time_t*)(&atime)));

    strtok(time, " ");
    sub = strtok(NULL, " ");
    printf("%4s", sub);
    sub = strtok(NULL, " ");
    printf("%3s", sub);
    sub = strtok(NULL, " ");
    printf("%9s", sub);
    strtok(NULL, ":");
    printf(" ");
}

int print_long(dir *dp) {
    char buf[15] = {0};
    char tmp[BUFSIZ] = {0};
    
    if (dp == NULL || dp->filep == NULL) {
        return -1;
    }
    if (dp->cnt == 0) {
        printf("\n");
        return 0;
    }
    for (int i = 0; i < dp->cnt; i++) {
        memset(buf, 0, sizeof(buf));
        get_file_mode(dp->filep[i].mode, buf);
        char *user_name = getpwuid(dp->filep[i].uid)->pw_name;
        char *group_name = getgrgid(dp->filep[i].gid)->gr_name;
        printf("%10s %-*u %-*s %-*s ", \
        buf, dp->links_max_wid, dp->filep[i].links, dp->user_max_wid, user_name, \
        dp->group_max_wid, group_name);
        if (S_ISCHR(dp->filep[i].mode) || S_ISBLK(dp->filep[i].mode)) {
            snprintf(tmp, BUFSIZ, "%d,%d", major(dp->filep[i].dev),\
            minor(dp->filep[i].dev));
            printf("%-*s ", dp->size_max_wid, tmp);
        } else {
            printf("%-*ld ", dp->size_max_wid, (long int)dp->filep[i].size);
        }
        print_file_time(dp->filep[i].atime);
        print_file_name(dp->filep[i].mode, dp->filep[i].name);
    }
    return 0; 
}

/**
 * 分栏算法， 尽可能的采用最少的行数
 * 失败返回NULL
 */
columns *divcol(dir *dp) {
    columns *cp;
    file *fp;
    int r, c;//max_filename = 0; //r 为行数c 为列数
    
    if (dp == NULL || (fp = dp->filep) == NULL) return NULL;

    for (r = 1; ; r++) { //行数不断增加
        int l = 0; // 行长
        c = dp->cnt / r;
        for (int i = 0; i < c; i++) {
            int w = 0;
            for (int j = i; j < dp->cnt; j += c) { //得到该列的最长文件名长度
                if (fp[j].length > w) {
                    w = fp[j].length;
                }
            }
            l += w + 1; // 计算出最大的行长
        }
        if (l < col) { // 最大行长满足要求
            break;    
        }
    }
    if ((cp = malloc(sizeof(columns))) == NULL) return NULL;
    cp->cnt = c;
    cp->row = r;
    if ((cp->lsp = malloc(c * sizeof(int))) == NULL) {
        free(cp);
        return NULL;
    }
    for (int i = 0; i < c; i++) {
        int w = 0;
        for (int j = i; j < dp->cnt; j+= c) {
            if (fp[j].length > w) {
                w = fp[j].length;
            }
        }
        cp->lsp[i] = w;
    }
    return cp;
}

int free_col(columns *cp) {
    if (cp == NULL) return -1;
    if (cp->lsp) free(cp->lsp);
    free(cp);
    return 0;
}

int print_all(dir *dp) {
    columns *cp;
    if (dp == NULL || dp->filep == NULL) return -1;
    if (dp->cnt == 0) {
        printf("\n");
        return 0;
    }
    if ((cp = divcol(dp)) == NULL) return -1;
    //printf("%d\n", cp->row);
    for (int i = 0; i < dp->cnt; i++) {
        switch (dp->filep[i].mode & S_IFMT) {
            case S_IFBLK: 
            case S_IFCHR:
                printf("\033[1m");
                //printf("\033[40m");
                printf("\033[33m%-*s\033[0m", cp->lsp[i % cp->cnt], dp->filep[i].name);
                printf("\033[0m");
                break;
            case S_IFDIR:
                printf("\033[34m%-*s\033[0m", cp->lsp[i % cp->cnt], dp->filep[i].name);
                break;
            case S_IFLNK:
                printf("\033[1m");
                printf("\033[36m%-*s\033[0m", cp->lsp[i % cp->cnt], dp->filep[i].name);
                printf("\033[0m");
                break;
            default:
                if (access(dp->filep[i].name, X_OK) == 0) {
                    printf("\033[32m%-*s\033[0m", cp->lsp[i % cp->cnt], dp->filep[i].name);
                } else {
                    printf("%-*s", cp->lsp[i % cp->cnt], dp->filep[i].name);
                }
        }
        if ((i == 0 && dp->cnt > 1) || (i + 1) % cp->cnt) {
            putchar(' ');
        } else {
            if (!file_flag) putchar('\n');
            else putchar(' ');
        }
    }
    if (dp->cnt % cp->cnt) {
        putchar('\n');
    }
    free_col(cp);
    return 0;
}

dir *open_dir(const char *pathname) {
    int w = 0;
    dir *dirp;
    file *filep;
    DIR *dp;

    struct stat stbuf;
    struct stat lstbuf;
    struct dirent *direntp;
    char name[MAX_NAME_LEN + 1] = {0};
    char abspath[500] = {0};
    char link_path[50] = {0};

    if ((lstat(pathname, &stbuf)) == -1) {
        perror("stat");
        return NULL;
    }
    if ((dirp = malloc(sizeof(dir))) == NULL) {
        perror("malloc");
        return NULL;
    }
    memset(dirp, 0, sizeof(dir));
    if ((dirp->filep = malloc(sizeof(file))) == NULL) {
        perror("malloc");
        free(dirp);
        return NULL;
    }
    if (!S_ISDIR(stbuf.st_mode)) { //如果是文件
        file_flag = 1;
        dirp->cnt = 1;
        copyinfo(dirp->filep, &stbuf, pathname);
        return dirp;
    }
    
    if ((dp = opendir(pathname)) == NULL) {
        free(dirp->filep);
        free(dirp);
        perror("opendir");
        return NULL;
    }
    for (dirp->cnt = 0; (direntp = readdir(dp)) != NULL; dirp->cnt++) {
        if (!a_flag) {
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
                dirp->cnt--;
                continue;
            }
        }
        if ((filep = realloc(dirp->filep, (dirp->cnt + 1) * sizeof(file))) == NULL) {
            perror("realloc");
            free_dirs(dirp);
            closedir(dp);
            return NULL;
        }
        dirp->filep = filep;
        sprintf(abspath, "%s/%s", pathname, direntp->d_name);
        if (lstat(abspath, &stbuf) == -1) {
            perror("lstat");
            free_dirs(dirp);
            closedir(dp);
            return NULL;
        }
        copyinfo(dirp->filep + dirp->cnt, &stbuf, direntp->d_name);
        if (S_ISLNK(stbuf.st_mode) && l_flag) {
            int n = (int)readlink(abspath, link_path, sizeof(link_path) - 1);
            strcat((dirp->filep + (dirp->cnt))->name, " -> ");
            strncat((dirp->filep + (dirp->cnt))->name, link_path, n);
        }
        if (printls == print_long) {
            /* 获取最长的links*/
            if ((w = get_digit_len(dirp->filep[dirp->cnt].links)) > dirp->links_max_wid) {
                dirp->links_max_wid = w; 
            } 
            /* 获取最长的用户名 */
            if ((w = get_string_len(getpwuid(dirp->filep[dirp->cnt].uid)->pw_name))\
 > dirp->user_max_wid) {
                dirp->user_max_wid = w;
            }
            /* 获取最长的组名 */
            if ((w = get_string_len(getgrgid(dirp->filep[dirp->cnt].gid)->gr_name))\
 > dirp->group_max_wid) {
                dirp->group_max_wid = w;
            }
            if (S_ISCHR(stbuf.st_mode) || S_ISBLK(stbuf.st_mode)) {
                w = get_digit_len(major(dirp->filep[dirp->cnt].dev));
                w += get_digit_len(minor(dirp->filep[dirp->cnt].dev)) + 1; //+1因为中间有,
                if (w > dirp->size_max_wid) dirp->size_max_wid = w;
            } else {
                if ((w = get_digit_len(dirp->filep[dirp->cnt].size)) > dirp->size_max_wid) {
                    dirp->size_max_wid = w;
                }
            }
        }
    }
    closedir(dp);
    return dirp;
}

int main(int argc, char **argv) {
    int opt, nargs;
    dir *dp;
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&size) == -1) {
        perror("ioctl");
        exit(1);
    }
    col = size.ws_col; // 终端宽度
    //printf("%d\n", col);
    l_flag = 0, a_flag = 0;
    printls = print_all;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
            case 'l':
                l_flag = 1;
                printls = print_long;
                break;
            case 'a':
                a_flag = 1;
                printls = print_all;
                break;
            default :
                fprintf(stderr, "Usage : %s [al] args\n", argv[0]);
                exit(1);
        }
    }
    if (l_flag) printls = print_long;
   // nargs 剩余参数的数量
    nargs = argc - optind;
    if (nargs == 0) { //当前目录
        if ((dp = open_dir(".")) != NULL) {
            sort_dir(dp, cmp_name);
            printls(dp);
            free_dirs(dp);
            return 0;
        }
    }
    for (int i = 0; i < nargs; i++) {
        file_flag = 0;
        //printf("args[%d] = %s\n", i, argv[i + optind]);
        if ((dp = open_dir(argv[i + optind])) != NULL) {
            if (!file_flag && nargs > 1 && i == 0) printf("%s:\n", argv[i + optind]);
            if (!file_flag && nargs > 1 && i != 0) printf("\n%s:\n", argv[i + optind]);
            sort_dir(dp, cmp_name);
            printls(dp);
            free_dirs(dp);
        }
        if (!file_flag && nargs > 1 && l_flag) putchar('\n');
        if (file_flag && i == nargs - 1) printf("\n");
    }
    // printf("optind = %d\n, argc = %d\n, argc - optind = %d\n", optind, argc, argc - optind);
    return 0;
}

