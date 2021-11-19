# Heng-Core

这是用于众衡评测系统的测量核心。具有如下功能

1. 时间限制 ( `-t <ms>` / `-tl <ms>` )
2. 内存限制 ( `-m <byte>` / `-ml <byte>`)
3. 进程数限制 ( `-p <pid_limit>` / `-pidl <pid_limit>` )
4. 每秒可使用 cpu 时间限制 / 至多充分利用几个核心 ( `-cpu <second>` )
5. 指定 uid 和/或 gid ( `-u <uid> -g <gid>` / `-uid <uid> -gid <gid>` )
6. 输入输出重定向 ( `-i / -o / -e / -stdin / -stdout / -stderr <path>` )
7. 指定 cwd ( `-c / -cwd <path>` )
8. 指定输出结果的fd ( `-f <fd>` )
9. 指定要执行文件 ( `-bin <path>` )
10. 指定参数 ( `-a / -args <...args>` )(必须放在最后)

输出格式

```cpp
struct Result
    {
        long long memory;//byte
        struct TimeUsage
        {
            long long sys;//ms
            long long usr;//ms
            long long real;//ms
        } time;
        int returnCode;
        int signal;
    };
```
