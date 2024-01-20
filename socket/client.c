#include <arpa/inet.h> // included <netinet/in.h> -> <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    // create communication socket on client (random port num)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("communicate socket (client)");
        return -1;
    }

    // init socket address refer to server
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);
    // connect to server
    int ret = connect(fd, (const struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connect");
        return -1;
    }

    int num = 0;
    while (1)
    {
        // send data to server
        char buf[1024];
        sprintf(buf, "hello %d\n", num++);
        send(fd, buf, strlen(buf) + 1, 0);
        // receive data
        memset(buf, 0, sizeof(buf));
        long len = recv(fd, buf, sizeof(buf), 0);
        if (len > 0)
        {
            printf("Server says: %s\n", buf);
        }
        else if (len == 0)
        {
            printf("The Server has disconnected...\n");
            break;
        }
        else
        {
            perror("recv (client side)");
            break;
        }
        sleep(1);
    }

    close(fd);

    return 0;
}