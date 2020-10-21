#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
int main(int argc, char *argv[])
{
    int n = std::stoi(argv[argc - 1]);
    while(n--)
    {
        fork();
        std::cout << getpid() << std::endl;
    }
    while(1)
        sleep(1);
    return 0;
}