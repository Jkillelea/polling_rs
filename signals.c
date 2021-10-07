#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

int fd = -1;
volatile int interrupt_count = 0;
char inputBuffer[128];

void sigpoll_callback(int signal) {
    int bytesRead = read(fd, inputBuffer, sizeof(inputBuffer));
    if (bytesRead > 0)
        printf("%d %s\n", interrupt_count, inputBuffer);

    memset(inputBuffer, 0, sizeof(inputBuffer));
    interrupt_count++;
}

int main()
{
    fd = open("/dev/ttyS0", O_RDWR);

    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    struct sigaction sa = {0};
    sa.sa_handler = sigpoll_callback;
    sa.sa_flags = 0;
    sigaction(SIGPOLL, &sa, NULL);

    while (true)
    {
        sleep(1);
    }

    return 0;
}
