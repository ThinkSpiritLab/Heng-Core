#include <signal.h>

int main(void)
{
    kill(1, SIGKILL);
    kill(3, SIGKILL);
    return 0;
}