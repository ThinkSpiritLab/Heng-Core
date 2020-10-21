#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
int main()
{
    int                              pid = fork();
    if(pid == 0)
    {
        while(1)
            ;
    }
    return 0;
}