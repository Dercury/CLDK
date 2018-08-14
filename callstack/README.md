# callstack, 调用栈

程序发生异常时，将函数的调用栈打印出来，可以大大提高定位效率。
Linux的C库函数已经给我们提供了实现此功能的基本基本函数。

## 1. Linux中提供了三个函数用来获取调用栈：

/* 获取函数调用栈 */

int backtrace(void **buffer, int size);

/* 将调用栈中的函数地址转化为函数名称 并返回一个字符串数组 */

char **backtrace_symbols(void *const *buffer, int size);
 
/* 将调用栈中的函数地址转化为函数名称 并将其定入到文件中 */

void backtrace_symbols_fd(void *const *buffer, int size, int fd);

### 编译时需要加上-rdynamic参数，以得到符号名称.

## 2. 注册程序退出执行的用户函数

/* register a function to be called at normal process termination */

int atexit(void (*function)(void));

因此为了获取程序退出时的调用栈，只需在注册的退出函数里面，使用调用调用栈的相关函数获取程序的调用栈，并将其打印出来就可以了。

