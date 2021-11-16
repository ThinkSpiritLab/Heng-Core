#include <signal.h>
#include <sys/types.h>

int main(void)
{
    kill(1, SIGKILL);
    return 0;
}