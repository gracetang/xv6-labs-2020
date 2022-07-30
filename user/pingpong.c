//lab1-2 pingpong: pipe

#include "kernel/types.h"
#include "user/user.h"

#define RD 0 //pipe read
#define WR 1 //pipe write

enum EXIT_STATUS{SUCESS = 0, FAILED};

int main(int argc, char** argv) {
    char buf = 'P';
    int fd_p2c[2]; // parent to child
    int fd_c2p[2]; // child to parent

    pipe(fd_p2c);
    pipe(fd_c2p);

    int pid = fork();
    int exit_status = SUCESS;
    if (pid < 0) { 
        fprintf(2, "ERROR: Fork Error\n"); // 0, 1, 2 = 標準輸入 標準輸出 標準錯誤 (note: process table entry 的fd flags至少從3開始)
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        exit_status = FAILED;
        exit(exit_status);
    } else if(pid == 0) { // child process
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);
        if (read(fd_p2c[RD], &buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "child read error\n");
            exit_status = FAILED;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
            //fprintf(1, "size of char: %d\n", sizeof(char));
        }
        if (write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write error\n");
            exit_status = FAILED;
        }
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);
        exit(exit_status);
    } else{
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);
        if (write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write error\n");
            exit_status = FAILED;
        }
        if (read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read error\n");
            exit_status = FAILED;
        }
        fprintf(1, "%d: received pong\n", getpid());
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);
        exit(exit_status);
    }
}

// pipe, 2 process
// parent -(w a byte)-> child (print:pid)
// child  -(w a byte)-> parent, exit
