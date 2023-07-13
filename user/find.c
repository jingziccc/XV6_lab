#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

// 格式化文件名，返回一个长度为 DIRSIZ+1 的静态缓冲区
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // 找到最后一个斜杠后面的第一个字符
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // 返回填充空格的文件名
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}


void
find(char *path, const char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){ //获取文件状态
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    fprintf(2, "find: %s is a file\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);  //将de.name的内容复制到p中
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      //不要在“.”和“..”目录中递归
      if(st.type == T_DIR && strcmp(de.name, ".")!=0 && strcmp(de.name, "..")!=0){
        find(buf, filename);
      }
      else if(strcmp(filename, de.name) == 0)
        printf("%s\n", buf);
    }
    break;
  }
  close(fd);
}



int
main(int argc, char *argv[])
{
  //第一个参数是路径，第二个参数是文件名
  if (argc == 2){
    //默认在当前目录下查找
    find(".",argv[1]);
  }
  else if(argc != 3 ){
    // 参数错误输出用法
    fprintf(2, "usage: find path filename\n");
    exit(1);
  }
  else{
    // 指定路径下查找
    find(argv[1],argv[2]);
  }
  
  exit(0);
}
