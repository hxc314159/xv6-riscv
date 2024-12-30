#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}


// read() 函数是 Unix 和类 Unix 系统（如 Linux）中的一个系统调用，用于从文件描述符读取数据。在 C/C++ 中，read() 函数的基本用法如下：

// 函数原型：

// c
// ssize_t read(int fd, void *buf, size_t count);
// 参数：

// fd：文件描述符，表示要从中读取数据的文件。0 通常表示标准输入（stdin）。
// buf：指向存储读取数据的缓冲区的指针。
// count：要读取的最大字节数。
// 返回值：

// 成功时，返回实际读取的字节数。
// 如果到达文件末尾，返回 0。
// 如果发生错误，返回 -1，并设置 errno 以指示错误类型。
