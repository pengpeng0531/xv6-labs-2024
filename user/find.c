#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
/*
    1.利用路径打开文件
    2.利用文件描述符获取文件元数据
*/
void find(char *path,const char *filename){
    char buf[512], *p;
    int fd;
    struct dirent de;//表示目录项的结构体，存储在目录文件中T_DIR
    struct stat st;  //表示文件元数据的结构体，通过fstat获取,以文件描述符为参数
    if((fd = open(path, 0))< 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if((fstat(fd, &st)) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    //参数错误
    if(st.type != T_DIR){
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }  
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)//文件为无效数据
            continue;
        memmove(p, de.name, DIRSIZ);//添加路径名
        p[DIRSIZ] = 0;              //添加结束符
        if(stat(buf, &st) < 0){//获取文件元数据，判断文件类型，文件存在，有访问权限，路径正确返回0
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        //不在.和..目录中递归 为什么？需要避免循环递归
        if(st.type ==T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0){
            find(buf, filename);//递归调用
        }else if(strcmp(filename,p)==0){
            printf("%s\n",buf);
        }
    }
    close (fd);
}
int main(int argc,char* argv[]){
    if(argc != 3){
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}