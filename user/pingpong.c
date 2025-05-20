#include"kernel/types.h"
#include"user/user.h"
#define RD 0
#define WR 1
int main(int argc,char const *argv[]){
    char buf = 'P';//buf:buffer

    int fd_c2p[2];//子->父
    int fd_p2c[2];//父->子
    //创建管道
    if(pipe(fd_c2p) < 0 || pipe(fd_p2c) < 0){
        fprintf(2,"pipe error\n");
        exit(1);
    }
    //创建子进程
    //管道需要进行关闭回收资源
    int pid = fork();
    int exit_status = 0;
    if(pid<0){
        fprintf(2,"fork error\n");
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        exit(1);
    }

    else if(pid == 0){//子进程程序:读取数据显示ping，并写入内容
        close(fd_p2c[WR]);//关闭写端
        close(fd_c2p[RD]);//关闭读端
        if(read(fd_p2c[RD],&buf,sizeof(char)) != sizeof(char)){
            fprintf(2,"read error\n");
            exit_status = 1;//读取错误
        }else{
            fprintf(1,"%d: received ping\n",getpid());
        }
        close(fd_p2c[RD]);//关闭读端 
        if(write(fd_c2p[WR],&buf,sizeof(char)) != sizeof(char)){
                fprintf(2,"write error\n");             
                exit_status = 1;//写入错误
        }
        close(fd_c2p[WR]);//关闭写端
        exit(exit_status);
    }
    //父进程程序:显示pong并写入内容
    else{

        close(fd_p2c[RD]);//关闭读端
        close(fd_c2p[WR]);//关闭写端
        if(write(fd_p2c[WR],&buf,sizeof(char)) != sizeof(char)){
            fprintf(2,"write error\n");
            exit_status = 1;//写入错误
        }else{
            close(fd_p2c[WR]);//关闭写端
            if(read(fd_c2p[RD],&buf,sizeof(char)) != sizeof(char)){
                fprintf(2,"read error\n");
                exit_status = 1;//读取错误
            }else{
                fprintf(1,"%d: received pong\n",getpid());
            }
            close(fd_c2p[RD]);//关闭读端
            
        }
        exit(exit_status);

    }
}

