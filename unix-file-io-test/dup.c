#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

int main()
{
    int fd = open("../../unix-file-io-test//abc.txt", O_RDWR | O_CREAT, 0664);
    if (fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    const char *pt = "Hello World...";
    write(fd, pt, strlen(pt)); // file cursor is at end of file

    int newfd = dup(fd);
    printf("newfd: %d\n", newfd);

    close(fd);

    const char *newpt = "holy shit!!!!!!";
    write(newfd, newpt, strlen(newpt));
    close(newfd);

    return 0;
}