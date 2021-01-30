#include <sys/types.h>
#include <sys/wait.h>
// #include <thread>
#include <iostream>
#include <unistd.h>
int main(void)
{
    std::cout << getpid() << std::endl;
}