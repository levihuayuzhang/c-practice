#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static inline void readfile()
{
    int fd = open("../../unix-file-io-test/abc.txt", O_RDWR); // cmake built binary is located at build/io-test (darwin completable)
    if (fd == -1)
    {
        printf("Open failed!\n");
    } else {
        printf("fd: %d\n", fd);
    }

    close(fd);
}

static inline void createfile()
{
    int fd = open("../../unix-file-io-test/defg.txt", O_CREAT|O_RDWR, 0664);
    if (fd == -1)
    {
        printf("Create failed!\n");
    } else {
        printf("fd: %d\n", fd);
    }

    close(fd);
}

static inline void existfile()
{
    int fd = open("../../unix-file-io-test/defg.txt", O_CREAT|O_EXCL|O_RDWR);
    if (fd == -1)
    {
        printf("Create failed, file exist already!\n");
    } else {
        printf("fd: %d\n", fd);
    }

    close(fd);
}

static inline int read_file_content()
{
    int fd = open("../../unix-file-io-test/abc.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("Read content failed.\n");
    }
    return fd;
}

static inline int write_file_content()
{
    int fd = open("../../unix-file-io-test/defg.txt", O_WRONLY|O_CREAT, 0664);
    if (fd == -1)
    {
        perror("Write content failed.\n");
    }
    return fd;
}

int main() 
{
    // readfile();
    // createfile();
    // existfile();
    int fd1 = read_file_content();
    int fd2 = write_file_content();

    char buf[4096];
    long len = -1;
    while ( (len = read(fd1, buf, sizeof(buf))) > 0 )
    {
        write(fd2, buf, len);
    }

    close(fd1);
    close(fd2);
    
    return 0;
}

