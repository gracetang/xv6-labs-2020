//lab01 primes

#include "kernel/types.h"
#include "user/user.h"

enum ACTION{RD=0, WR};
const uint INT_LEN = sizeof(int);

/**
 * @brief 读取左邻居的第一个数据
 * 
 * @param lpipe 左邻居的管道符
 * @param dst 用于存储第一个数据的地址
 * @return int 如果没有数据返回-1,有数据返回0
 */
int lpipe_first_data(int lpipe[2], int *dst) {
    if (read(lpipe[RD], dst, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

/**
 * @brief 讀取lpipe數據 將不能被first整除的寫入rpipe
 * 
 * @param lpipe 左鄰居管道符
 * @param rpipe 右鄰居管道符
 * @param first 左鄰居的第一個數據
 */
void transmit_data(int lpipe[2], int rpipe[2], int first) {
    int data;
    while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) {
        if (data % first != 0) {
            write(rpipe[WR], &data, sizeof(int));
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

/**
 * @brief find prime number
 * 
 * @param lpipe 左鄰居管道
 */
void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;
    if (lpipe_first_data(lpipe, &first) == 0) {
        int p[2];
        pipe(p); //cur pipe
        transmit_data(lpipe, p, first);

        if (fork() == 0) {
            primes(p); //recursive
        } else {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(){
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; ++i) {
        write(p[WR], &i, INT_LEN);
    }
    // close(p[WR]); could it be here?
    if (fork() == 0) {
        primes(p);
    } else {
        close(p[WR]); //???
        close(p[RD]);
        wait(0);
    }
    exit(0);
}
