#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc,int *argv[])
{
    int PtoC[2];
    int CtoP[2];
    pipe(PtoC);
    pipe(CtoP);
    char buf[1];
    int pid = fork();
    if(pid == 0){
        // child
        close(PtoC[1]); // close write end
        close(CtoP[0]); // close read end
        read(PtoC[0],buf,1);    // read from pipe
        printf("%d: received ping\n",getpid());
        write(CtoP[1],buf,1);   // write to pipe
        close(PtoC[0]); // close read end
        close(CtoP[1]); // close write end
    }else{
        // parent
        close(PtoC[0]); // close read end
        close(CtoP[1]); // close write end
        write(PtoC[1],"a",1);
        read(CtoP[0],buf,1);
        printf("%d: received pong\n",getpid());
        close(PtoC[1]); // close write end
        close(CtoP[0]); // close read end
    }
    exit(0);

}