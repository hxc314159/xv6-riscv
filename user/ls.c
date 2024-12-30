#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


// 提取路径中的文件名，并将其填充到固定大小的缓冲区中。
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int) st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}



// 在文件系统中，`.` 和 `..` 是特殊的目录项，用来表示当前目录和父目录：

// - `.` 指向当前目录本身。
// - `..` 指向当前目录的父目录。

// 当递归遍历目录时，如果不排除这两个目录项，会导致程序进入无限递归，因为：

// ### **递归行为分析**

// #### 1. **`.` 的问题**
// - 如果访问 `.`，程序会再次调用递归函数，并继续处理当前目录。
// - 每次都会重复处理当前目录的所有内容，导致死循环。

// #### 2. **`..` 的问题**
// - 如果访问 `..`，程序会切换到父目录。
// - 在父目录中，可能会再次进入当前目录（通过父目录的 `.` 或父目录的其他路径），导致循环回到原来的目录，从而形成无限递归。

// ---

// ### **示例**

// 假设目录结构如下：
// ```
// /test
//   ├── file1
//   └── dir1
//       ├── file2
//       └── .
//       └── ..
// ```

// #### 递归逻辑：
// - 对 `/test` 进行递归：
//   - 遍历 `file1`，没有问题。
//   - 遇到 `dir1`，进入 `dir1`：
//     - 遍历 `file2`，没有问题。
//     - 遇到 `.`，递归进入 `dir1`，重复处理 `file2`、`.` 和 `..`，导致死循环。
//     - 遇到 `..`，递归进入 `/test`，又重新开始递归 `/test`。

// #### **无限递归图解**
// ```
// /test
//   -> /test/dir1
//     -> /test/dir1/. (再进入 /test/dir1)
//       -> /test/dir1/..
//         -> /test
//           -> /test/dir1
//             -> 无限重复...
// ```

// ---

// ### **解决方法**

// 1. **跳过 `.` 和 `..`：**
//    - 在递归逻辑中判断目录项名称：
//      ```c
//      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
//        continue; // 跳过当前目录和父目录
//      ```

// 2. **结果：**
//    - 程序只会处理实际的子目录和文件，不会因为 `.` 和 `..` 导致无限递归。

// ---

// ### **总结**

// `.` 和 `..` 是文件系统中的循环引用，直接递归会导致程序陷入死循环。通过显式跳过它们，可以避免无限递归，同时正确遍历目录结构。
