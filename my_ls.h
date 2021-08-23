//
// Created by sudo-li on 2021/8/23.
//

#ifndef MY_LS_MY_LS_H
#define MY_LS_MY_LS_H

#define MAX_NAME_LEN 1024

typedef struct File file;
typedef struct Dir dir;
typedef struct Columns columns;

struct Columns {
    int cnt; // 栏数
    int row;//行数
    int *lsp; // 每栏占用的最大宽度
};

struct Dir {
    int cnt; //目录下的文件总数
    //int inode_max_wid; // iNode 最大宽度
    int links_max_wid;
    int user_max_wid;
    int group_max_wid;
    int size_max_wid;
    file *filep; // 各个文件的详细信息数组
};

struct File {
    dir *dirp; // 如果该文件不是目录此项为NULL
    int length; // 文件名长度
    int uid;
    int gid;
    int dev;
    int mode;
    int links;
    int ino;
    int size;
    int atime;
    int ctime;
    int mtime;
    char name[MAX_NAME_LEN + 1];
};


#endif //MY_LS_MY_LS_H
