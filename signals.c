#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

volatile struct {
    int fd;
    int interrupt_count;
    char inputBuffer[128];
} IOData;

void sigpoll_callback(int signal) {
    int bytesRead = read(IOData.fd, (void *) IOData.inputBuffer, sizeof(IOData.inputBuffer));
    if (bytesRead > 0)
        printf("%d %s\n", IOData.interrupt_count, IOData.inputBuffer);

    memset((void *) IOData.inputBuffer, 0, sizeof(IOData.inputBuffer));
    IOData.interrupt_count++;
}

int main()
{
    IOData.fd = open("/dev/ttyS0", O_RDWR);

    fcntl(IOData.fd, F_SETOWN, getpid());
    fcntl(IOData.fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    struct sigaction sa = {0};
    sa.sa_handler = sigpoll_callback;
    sa.sa_flags = 0;
    sigaction(SIGPOLL, &sa, NULL);

    // Put the main thread to sleep and wake for inputs
    while (true)
    {
        sleep(1);
    }

    return 0;
}
