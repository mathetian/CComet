#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAX_PORTS 10

int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1) 
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}
int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    const char *ip = "0.0.0.0";
    int opt = 1;
    int bufsize;
    socklen_t optlen;
    int connections = 0;
    int base_port = 7000;
    if(argc > 2){
        base_port = atoi(argv[1]);
    }

    setlimit(100000);

    int server_socks[MAX_PORTS];

    for(int i=0; i<MAX_PORTS; i++){
        int port = base_port + i;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((short)port);
        inet_pton(AF_INET, ip, &addr.sin_addr);

        int serv_sock;
        if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            goto sock_err;
        }
        if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
            goto sock_err;
        }
        if(bind(serv_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
            goto sock_err;
        }
        if(listen(serv_sock, 1024) == -1){
            goto sock_err;
        }

        server_socks[i] = serv_sock;
        printf("server listen on port: %d\n", port);
    }

    //optlen = sizeof(bufsize);
    //getsockopt(serv_sock, SOL_SOCKET, SO_RCVBUF, &bufsize, &optlen);
    //printf("default send/recv buf size: %d\n", bufsize);

    while(1){
        fd_set readset;
        FD_ZERO(&readset);
        int maxfd = 0;
        for(int i=0; i<MAX_PORTS; i++){
            FD_SET(server_socks[i], &readset);
            if(server_socks[i] > maxfd){
                maxfd = server_socks[i];
            }
        }
        int ret = select(maxfd + 1, &readset, NULL, NULL, NULL);
        if(ret < 0){
            if(errno == EINTR){
                continue;
            }else{
                printf("select error! %s\n", strerror(errno));
                exit(0);
            }
        }

        if(ret > 0){
            for(int i=0; i<MAX_PORTS; i++){
                if(!FD_ISSET(server_socks[i], &readset)){
                    continue;
                }
                socklen_t addrlen = sizeof(addr);
                int sock = accept(server_socks[i], (struct sockaddr *)&addr, &addrlen);
                if(sock == -1){
                    goto sock_err;
                }
                connections ++;
                printf("connections: %d, fd: %d\n", connections, sock);
            }
        }
    }

    return 0;
sock_err:
    printf("error: %s\n", strerror(errno));
    return 0;
}