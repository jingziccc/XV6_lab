#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    if(argc != 2){  // 参数个数不对，argv[0]是程序名, argv[1]是sleep的tick数
        fprintf(2, "Usage: sleep ticks\n"); // fprintf(2, ...)是向标准错误输出
        exit(1);
    }

    int ticks = atoi(argv[1]);  // atoi是将字符串转换成整数, ascii to integer
    if (ticks <= 0){
        fprintf(2, "sleep: invalid ticks\n");
        exit(1);
    }
    
    sleep(ticks);
    exit(0);
}