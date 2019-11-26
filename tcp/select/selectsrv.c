#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>

#define MAX_CLIENTS 30

int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    struct sockaddr remote;
    struct timeval timeout = {0, 0};
    int res, master_socket, sd, addrlen, client_sockets[MAX_CLIENTS], max_sd;
    char buf[100];
    fd_set readfds;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;//IPv4 当为IPv6时为:AF_INET6
    hint.ai_socktype = SOCK_STREAM;//流模式
    hint.ai_protocol = 0;
    hint.ai_flags    = AI_PASSIVE;//被动的，用于bind，用于server socket
    //处理名字到地址以及服务到端口这两种转换
    res = getaddrinfo(NULL, "8088", &hint, &result);
    if (res != 0) {
        perror("error : cannot get socket address!\n");
        exit(1);
    }
    //获得句柄
    master_socket = socket(result->ai_family, result->ai_socktype,
                           result->ai_protocol);
    if (master_socket == -1) {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }
    //绑定到端口
    res = bind(master_socket, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("error : cannot bind the socket with the given address!\n");
        exit(1);
    }
    //开始监听
    res = listen(master_socket, SOMAXCONN);
    if (res == -1) {
        perror("error : cannot listen at the given socket!\n");
        exit(1);
    }

    memset(client_sockets, 0, sizeof(client_sockets));

    addrlen = sizeof(struct sockaddr);
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
          
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }
        
        res = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ( res < 0) {
            perror("error : func select error");
            exit(1);
        }

        if (FD_ISSET(master_socket, &readfds)) {
            
            sd = accept(master_socket, &remote, &addrlen);
            if (sd < 0) {
                perror("error : cannot accept client socket\n");
                exit(1);
            }
            
            //strcpy(buf, "Welcome Client");
            //write(sd, buf, strlen(buf));

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                     client_sockets[i] = sd;
                     break;
                }
            }
        }
        
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                res = read(sd, buf, sizeof(buf));
                
                if (res == 0) {
                    close(sd);
                    client_sockets[i] = 0;
                    printf("close socket\n");
                } else {
                    printf("receive client data : %s\n", buf);
                    strcpy(buf, "Hello Client\0");
                    res = write(sd, buf, strlen(buf));
                    printf("write <hello client> to client\n");
                }
            }
        }

    }
    return 0;
}
