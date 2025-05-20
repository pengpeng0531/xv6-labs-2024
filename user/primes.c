#include"kernel/types.h"
#include"user/user.h"
#define RD 0
#define WR 1

//1.将初始数据输入到管道中
//2.读取第一个数据，进行数据筛选
//3.将不能被第一个数据整除的数据写入右邻居的管道中
//此时右管道的输出数据为输入数据

const uint INT_LEN = sizeof(int);
/**
 * * @brief 读取左邻居的第一个数
 * * @param lpipe 左邻居的管道
 * * @param dst 用于存储第一个数据的地址
 * * @return 如果没有数据返回-1，有数据返回0
 */
int lpipe_first_data(int lpip[2],int*dst){
    if(read(lpip[RD],dst,INT_LEN) == INT_LEN){
        printf("prime %d\n",*dst);
        return 0;
    }
    return -1;
}
/**
 * * @brief 读取左邻居的数据，将不能被first整除的数据写入右邻居
 * * @param lpipe 左邻居的管道符
 * * @param rpipe 右邻居的管道符
 * * @param first 左邻居的第一个数据
 */
void transmit_data(int lpipe[2],int rpipe[2],int first){
    int num;
    //左管道读取数据
    while(read(lpipe[RD],&num,INT_LEN) == INT_LEN){
        if(num % first != 0){
            write(rpipe[WR],&num,INT_LEN);
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}
/**
 * * @brief 寻找素数
 * * @param lpipe 左邻居的管道符
 */
void prime(int lpipe[2]){
    close(lpipe[WR]);
    int first;
    //读取左邻居的第一个数
    if(lpipe_first_data(lpipe,&first) == -1){
        close(lpipe[RD]);
        return;
    }
    //创建右邻居的管道
    int rpipe[2];
    if(pipe(rpipe) < 0){
        fprintf(2,"pipe error\n");
        exit(1);
    }
    //传输数据
    transmit_data(lpipe,rpipe,first);

    //创建子进程
    int pid = fork();
    if(pid < 0){
        fprintf(2,"fork error\n");
        exit(1);
    }else if(pid == 0){
        close(rpipe[WR]);
        prime(rpipe);  //递归调用?此时rpipe是左邻居的管道
        exit(0);
    }else{
        close(rpipe[WR]);
        close(rpipe[RD]);
        wait(0);
    }
    exit(0);
}
int main(int argc,char const *argv[]){
    int p[2];
    if(pipe(p)<0){
        fprintf(2,"pipe error\n");
        exit(1);
    }
    for(int i=2;i<=35;i++)
        write(p[WR],&i,INT_LEN);   
    int pid = fork();
    if(pid < 0){ 
        fprintf(2,"fork error\n");
        exit(1);
    }
    else if (pid == 0){
        prime(p);
    }
    else{
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);   
}