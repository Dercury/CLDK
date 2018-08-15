# callstack, 调用栈

程序发生异常时，将函数的调用栈打印出来，可以大大提高定位效率。
Linux的C库函数已经给我们提供了实现此功能的基本函数。

## 1. Linux中提供了三个函数用来获取调用栈：

```c
#include <execinfo.h>

/* 获取函数调用栈，
 * 结果存储在buffer中，返回值为栈的深度，
 * 参数size限制栈的最大深度，即最大取size步的栈信息 */
int backtrace(void **buffer, int size);

/* 将调用栈中的函数地址转化为函数名称，
 * 返回一个字符串数组（需用户释放返回的内存）
 * 参数size限定转换的深度，一般用backtrace调用的返回值 */
char **backtrace_symbols(void *const *buffer, int size);
 
/* 将调用栈中的函数地址转化为函数名称，并将其写入fd指定的文件描述符，
 * 功能和backtrace_symbols差不多，只不过它不把转换结果返回给调用方 */
void backtrace_symbols_fd(void *const *buffer, int size, int fd);
```

### 编译时需要加上-rdynamic参数，以得到符号名称.

## 2. 注册程序退出执行的用户函数

```c
#include <stdlib.h>

/* register a function to be called at normal process termination */
int atexit(void (*function)(void));
```

因此为了获取程序退出时的调用栈，只需在注册的退出函数里面，使用调用调用栈的相关函数获取程序的调用栈，并将其打印出来就可以了。

