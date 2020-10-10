//chat 的客户端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "winsock.h"
#include "stdio.h"
#pragma comment(lib, "wsock32.lib")
#include <sys/types.h>

#define BUFLEN 1024
#define PORT 6666

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in ser_addr;
    int len;
    unsigned int port;
    char buf[BUFLEN];
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd;

    /*建立socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(errno);
    }
    else
        printf("socket create success!\n");

    /*设置服务器ip*/
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);

    /*开始连接服务器*/
    if (connect(sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(errno);
    }
    else
        printf("conncet success!\n");

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        maxfd = 0;
        FD_SET(sockfd, &rfds);
        if (maxfd < sockfd)
            maxfd = sockfd;
        tv.tv_sec = 6;
        tv.tv_usec = 0;
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            printf("select出错，客户端程序退出\n");
            break;
        }
        else if (retval == 0)
        {
            printf("waiting...\n");
            continue;
        }
        else
        {
            /*服务器发来了消息*/
            if (FD_ISSET(sockfd, &rfds))
            {
                /******接收消息*******/
                strcpy(buf, BUFLEN);
                len = recv(sockfd, buf, BUFLEN, 0);
                if (len > 0)
                    printf("服务器发来的消息是：%s\n", buf);
                else
                {
                    if (len < 0)
                        printf("接受消息失败！\n");
                    else
                        printf("服务器退出了，聊天终止！\n");
                    break;
                }
            }
            /*用户输入信息了,开始处理信息并发送*/
            if (FD_ISSET(0, &rfds))
            {
                /******发送消息*******/
                bzero(buf, BUFLEN);
                fgets(buf, BUFLEN, stdin);

                if (!strncasecmp(buf, "quit", 4))
                {
                    printf("client 请求终止聊天!\n");
                    break;
                }
                len = send(sockfd, buf, strlen(buf), 0);
                if (len > 0)
                    printf("\t消息发送成功：%s\n", buf);
                else
                {
                    printf("消息发送失败!\n");
                    break;
                }
            }
        }
    }
    /*关闭连接*/
    close(sockfd);

    return 0;
}