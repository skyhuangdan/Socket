#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    int res, sfd;
    char buf[20];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    //函数能够处理名字到地址以及服务到端口这两种转换
    res = getaddrinfo("127.0.0.1", "8088", &hint, &result);
    if (res == -1) {
        perror("error : cannot get socket address!\n");
        exit(1);
    }
    //获得句柄
    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }
    //客户端发起连接
    res = connect(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("error : cannot connect the server!\n");
        exit(1);
    }
    printf(" connected !!\n");
    while (1)
    {
        printf("pls input:\n");    
        char desc[1024] = {0};      
        scanf("%s", &desc);
        printf(" inputed: %s\n", desc);  
        write(sfd, desc, sizeof(desc));

        //等待服务器返回数据
        read(sfd, buf, sizeof(buf));
        printf("read < %s > from server\n", buf);
    }
    
/*
    strcpy(buf, "Hello Server!");
    //给服务器发送数据
    write(sfd, buf, sizeof(buf));
    printf("write < %s > to server\n", buf);
    //等待服务器返回数据
    read(sfd, buf, sizeof(buf));
    printf("read < %s > from server\n", buf);
*/
    printf("client closed\n");
    return 0;
}
