// Use fork and exec to invoke the command on each line of input. Use wait in the parent to wait for the child to complete the command.
// To read individual lines of input, read a character at a time until a newline ('\n') appears.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

// 读取pipe中的参数
void
readline(char* buf)
{
    int i = 0;
    char c;
    while(read(0, &c, 1) != 0){
        //读到换行符 不退出，使用' '替换
        if(c == '\n'){
            buf[i++] = ' ';
        }
        else if(c==' '&&buf[i-1]==' ')
            continue;
        else buf[i++] = c;
    }
    buf[i] = '\0';

}

//返回长度
int
get_arg(char* buf, char* argv[])
{
    int argc = 0;
    char* p = buf;
    int in_word = 0;
    while (*p != '\0') {
        if (*p == ' ') {
            *p = '\0';
            in_word = 0;
        } else if (!in_word) {
            argv[argc++] = p;
            in_word = 1;
        }
        p++;
    }
    argv[argc] = 0;
    return argc;
}

void xargs_exec(char *argv[]);

void
xargs(char* init_arg[], int argc)
{
    //从管道中读取一行
    char buf[1024];
    readline(buf);

    char* pipe_argv[MAXARG];
    int len = get_arg(buf, pipe_argv);

    char* argvs[MAXARG];//存储命令行参数
    for (int i = 0; i < argc; ++i)
        argvs[i] = init_arg[i];

    char* n_option = "-n";
    if(strcmp(init_arg[0], n_option) == 0){
        int i = 0;
        //第二个参数设置为1
        while(i < len){
            argvs[argc] = pipe_argv[i++];
            argvs[argc+1] = 0;
            xargs_exec(argvs+2);
        }
    }
    else{
        int i = 0;
        while(i < len){
            argvs[argc++] = pipe_argv[i++];
        }
        argvs[argc] = 0;
        xargs_exec(argvs);
    }

}



void
xargs_exec(char *argv[]){
    int pid = fork();
    if(pid == 0){
    //打印参数
    for (int i = 0; argv[i] != 0; ++i)
         //printf("xargs:%s ", argv[i]);
        if(exec(argv[0], argv)==-1){
            exit(0);
        }
    }
    else
        wait(0);
    
}



int
main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(2, "xargs: no args\n");
        exit(0);
    }

    xargs(argv+1, argc-1);
    exit(0);
}
//echo 11 2 3 44| xargs -n 1 echo ii j
///echo ss m| xargs echo ii jls
//sh < xargstest.sh
//find . b | xargs grep hello