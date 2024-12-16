#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    fd_set readfds;

    // 建立 Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "Socket creation error\n";
        return -1;
    }

    // 設定伺服器地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address\n";
        return -1;
    }

    // 連接伺服器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection failed\n";
        return -1;
    }
    std::cout << "已連線到聊天室伺服器\n";

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);  // 監聽來自伺服器的訊息
        FD_SET(STDIN_FILENO, &readfds);  // 監聽標準輸入

        // 使用 select
        if (select(sock + 1, &readfds, NULL, NULL, NULL) < 0) {
            std::cout << "select error\n";
            return -1;
        }

        // 接收伺服器訊息
        if (FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            valread = read(sock, buffer, BUFFER_SIZE);
            if (valread == 0) {
                std::cout << "伺服器已關閉\n";
                break;
            }
            std::cout << "訊息: " << buffer;
        }

        // 發送訊息到伺服器
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            read(STDIN_FILENO, buffer, BUFFER_SIZE);
            send(sock, buffer, strlen(buffer), 0);
        }
    }
    close(sock);
    return 0;
}
