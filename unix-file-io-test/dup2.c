#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

int main() {
  int fd = open("../../unix-file-io-test//abc.txt", O_RDWR | O_CREAT, 0664);
  if (fd == -1) {
    perror("open");
    exit(0);
  }
  printf("fd: %d\n", fd);

  const char *pt = "Hello World...";
  write(fd, pt, strlen(pt)); // file cursor is at end of file

  int fd1 = 1023; // not assign to any file
  dup2(fd, fd1);
  printf("newfd: %d\n", fd1);

  const char *newpt = "holy gosh!!!!!!";
  write(fd1, newpt, strlen(newpt));
  close(fd1);

  return 0;
}
