#include <arpa/inet.h> // included <netinet/in.h> -> <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

struct SockInfo
{
    struct sockaddr_in addr;
    int fd;
};

struct SockInfo infos[512];

void *working(void *arg);

int main(int argc, const char *argv[])
{
    // create listening socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }

    // init listening socket address
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY; // 0 => 0.0.0.0

    // bind local IP and port
    int ret = bind(lfd, (const struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    // set up listening and set queue size
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }

    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(lfd, &readset);

    int maxfd = lfd;
    while (1)
    {
        int ret = select(maxfd + 1, &readset, NULL, NULL, NULL);
        // whether is the listening file descriptor
        int cfd = accept(lfd, NULL, NULL);
    }

    int max = sizeof(infos) / sizeof(infos[0]);
    for (int i = 0; i < max; ++i)
    {
        memset(&infos[i], 0, sizeof(infos[i]));
        infos[i].fd = -1; // mark to indicate: has not been used
    }

    // block and wait for client to connect
    unsigned int addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        struct SockInfo *pinfo = NULL;
        for (int i = 0; i < max; ++i)
        {
            if (infos[i].fd == -1)
            {
                pinfo = &infos[i];
                break;
            }
        }
        int cfd = accept(lfd, (struct sockaddr *)&pinfo->addr, &addrlen);
        pinfo->fd = cfd;
        if (pinfo->fd == -1)
        {
            perror("accept");
            break;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, working, pinfo);
        pthread_detach(tid);
    }

    close(lfd);
    return 0;
}

void *working(void *arg)
{
    struct SockInfo *pinfo = (struct SockInfo *)arg;

    // print client ip info
    char ip[32];
    printf("Client IP %s, port %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    while (1)
    {
        char buf[1024] = {0};
        long len = recv(pinfo->fd, buf, sizeof(buf), 0);
        if (len > 0)
        {
            printf("Client says: %s\n", buf);
            for (int i = 0; i < len; ++i)
            {
                buf[i] = toupper(buf[i]);
            }
            send(pinfo->fd, buf, len, 0);
        }
        else if (len == 0)
        {
            printf("The Client has disconnected...\n");
            break;
        }
        else
        { // -1
            perror("recv");
            break;
        }
    }

    close(pinfo->fd);
    pinfo->fd = -1;

    return 0;
}
