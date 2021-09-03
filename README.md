# Heng-Core

这是用于众衡评测系统的测量核心。具有如下功能

1. 时间限制 ( `-t <ms>` / `-tl <ms>` )

2. 内存限制 ( `-m <byte>` / `-ml <byte>`)

3. 进程数限制 ( `-p <pid_limit>` / `-pidl <pid_limit>` )

4. 指定 uid 和/或 gid ( `-u <uid> -g <gid>` / `-uid <uid> -gid <gid>` )

5. 输入输出重定向 ( `-i / -o / -e / -stdin / -stdout / -stderr <path>` )

6. 指定 cwd ( `-c / -cwd <path>` )

7. 指定输出结果的fd ( `-f <fd>` )

8. 指定要执行文件 ( `-bin <path>` )

9. 指定参数 ( `-a / -args <...args>` )(必须放在最后)

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

为了避免误操作，忽略了程序终止信号（SIGINT）。想要提前终止任务，请手动 kill 主子进程。