#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    char buffer[BUFFER_SIZE];

    // Tạo socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Không thể tạo socket");
        exit(1);
    }

    // Cấu hình địa chỉ server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Liên kết socket với địa chỉ server
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Không thể liên kết socket với địa chỉ");
        exit(1);
    }

    // Lắng nghe kết nối từ client
    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        perror("Lỗi trong quá trình lắng nghe");
        exit(1);
    }

    printf("Chat server đang chạy...\n");

    while (1)
    {
        // Chấp nhận kết nối từ client
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0)
        {
            perror("Lỗi trong quá trình chấp nhận kết nối");
            exit(1);
        }

        printf("Kết nối mới từ client %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Xử lý thông điệp từ client
        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead <= 0)
            {
                printf("Client %s:%d đã ngắt kết nối\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                break;
            }

            // In thông điệp từ client
            printf("Client %s:%d: %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);

            // Gửi thông điệp cho tất cả client khác
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clientSockets[i] != 0 && clientSockets[i] != clientSocket)
                {
                    send(clientSockets[i], buffer, strlen(buffer), 0);
                }
            }
        }

        // Đóng kết nối với client
        close(clientSocket);
    }

    // Đóng server socket
    close(serverSocket);

    return 0;
}
