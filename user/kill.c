#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char **argv)
{
  int i;

  if(argc < 2){
    fprintf(2, "usage: kill pid...\n");
    exit(1);
  }
  for(i=1; i<argc; i++)
    kill(atoi(argv[i]));
  exit(0);
}


// atoi() 是 C 和 C++ 标准库中的一个函数，用于将字符串转换为整数。以下是关于 atoi() 的详细说明：

// 函数原型
// c
// int atoi(const char *str);
// 参数
// str：指向要转换的字符串的指针。
// 返回值
// 成功时，返回转换后的整数值。
// 如果字符串不能被转换为有效的整数，返回 0。
// 如果转换的值超出了 int 类型的范围，行为未定义（在某些实现中可能会返回 INT_MAX 或 INT_MIN）。
// 注意事项
// atoi() 不提供错误处理机制，无法区分无效输入和值为 0 的情况。
// 对于更复杂的转换需求，建议使用 strtol() 或 std::stoi（C++11 及以上）。
