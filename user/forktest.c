// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N  1000

void
print(const char *s)
{
  write(1, s, strlen(s));
}

void
forktest(void)
{
  int n, pid;

  print("fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit(0);
  }

  if(n == N){
    print("fork claimed to work N times!\n");
    exit(1);
  }

  for(; n > 0; n--){
    if(wait(0) < 0){
      print("wait stopped early\n");
      exit(1);
    }
  }

  if(wait(0) != -1){
    print("wait got too many\n");
    exit(1);
  }

  print("fork test OK\n");
}

int
main(void)
{
  forktest();
  exit(0);
}

// notes:...
// 理解子进程在父进程调用 `wait()` 之前结束的情况非常重要。让我们详细分析一下这种情况下 `wait()` 的行为。

// ### 子进程在父进程调用 `wait()` 之前结束

// 当子进程在父进程调用 `wait()` 之前结束时，父进程调用 `wait()` 仍然可以接收到该子进程的退出状态。这是因为子进程在结束时会进入僵尸状态（zombie state），直到父进程通过 `wait()` 或 `waitpid()` 获取其退出状态并清理资源。

// ### 详细解释

// 1. **子进程结束**：
//    - 当子进程调用 `exit()` 或者自然结束（如到达 `main` 函数的末尾）时，它会进入僵尸状态。
//    - 在僵尸状态下，子进程的进程控制块（PCB）仍然存在，但其资源已被释放，除了进程控制块本身。

// 2. **父进程调用 `wait()`**：
//    - 当父进程调用 `wait(0)` 时，它会检查是否有任何子进程处于僵尸状态。
//    - 如果有子进程处于僵尸状态，`wait(0)` 会立即返回该子进程的 PID，并清理其进程控制块。
//    - 如果没有子进程处于僵尸状态，`wait(0)` 会阻塞父进程，直到有一个子进程结束并进入僵尸状态。

// ### 示例流程

// 假设父进程创建了一个子进程，并且子进程在父进程调用 `wait()` 之前结束：

// 1. **创建子进程**：
//    ```c
//    pid_t pid = fork();
//    if (pid == 0) {
//        // 子进程
//        printf("子进程结束\n");
//        exit(0);
//    }
//    ```

// 2. **子进程结束**：
//    - 子进程调用 `exit(0)` 结束，并进入僵尸状态。

// 3. **父进程调用 `wait()`**：
//    ```c
//    int status;
//    pid_t wpid = wait(&status);
//    if (wpid == -1) {
//        perror("wait");
//        exit(1);
//    }
//    printf("父进程等待到子进程 %d\n", wpid);
//    ```

// 4. **`wait()` 的行为**：
//    - 父进程调用 `wait(&status)` 时，会立即返回子进程的 PID，因为子进程已经处于僵尸状态。
//    - `wait()` 清理子进程的进程控制块，并返回子进程的退出状态。

// ### 代码示例

// 以下是一个完整的示例代码，展示了子进程在父进程调用 `wait()` 之前结束的情况：

// ```c
// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// void print(const char *s) {
//   write(1, s, strlen(s));
// }

// void forktest(void) {
//   int n, pid;

//   print("fork test\n");

//   for(n = 0; n < 1; n++) { // 只创建一个子进程以简化示例
//     pid = fork();
//     if(pid < 0) {
//       print("fork failed\n");
//       exit(1);
//     } else if(pid == 0) {
//       // 子进程
//       print("子进程结束\n");
//       exit(0);
//     }
//   }

//   // 父进程等待子进程结束
//   int status;
//   pid_t wpid = wait(&status);
//   if(wpid == -1) {
//     print("wait stopped early\n");
//     exit(1);
//   }
//   print("父进程等待到子进程 ");
//   print(itoa(wpid));
//   print("\n");

//   // 再次调用 wait() 检查是否有多余的子进程
//   if(wait(0) != -1) {
//     print("wait got too many\n");
//     exit(1);
//   }

//   print("fork test OK\n");
// }

// int main(void) {
//   forktest();
//   exit(0);
// }
// ```

// ### 运行结果示例

// ```
// fork test
// 子进程结束
// 父进程等待到子进程 1234
// fork test OK
// ```

// ### 总结

// - **子进程在父进程调用 `wait()` 之前结束**：子进程会进入僵尸状态。
// - **父进程调用 `wait()`**：`wait()` 会立即返回子进程的 PID，并清理其进程控制块。
// - **`wait stopped early` 的条件**：`wait stopped early` 表示 `wait(0)` 提前停止，即在所有子进程结束之前返回 `-1`，这通常是因为 `wait` 被信号中断或其他错误导致的。
// - **确保所有子进程被等待**：通过第一个 `for` 循环等待所有子进程结束，并通过第二个 `if` 语句确保没有多余的子进程可以等待。

// 如果您有更多具体的问题或需要进一步的解释，请告诉我！
