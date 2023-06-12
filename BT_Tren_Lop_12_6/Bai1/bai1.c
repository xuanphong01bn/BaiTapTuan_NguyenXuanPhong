#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

void send_file_list(int client_socket, const char* folder_path) {
    DIR* dir;
    struct dirent* entry;

    dir = opendir(folder_path);
    if (dir == NULL) {
        char* error_message = "ERROR No files to download\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
        return;
    }

    int file_count = 0;
    char file_list[BUFFER_SIZE] = "";

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
            strcat(file_list, entry->d_name);
            strcat(file_list, "\r\n");
        }
    }

    if (file_count == 0) {
        char* error_message = "ERROR No files to download\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
    } else {
        char response[BUFFER_SIZE];
        sprintf(response, "OK %d\r\n%s\r\n\r\n", file_count, file_list);
        send(client_socket, response, strlen(response), 0);
    }

    closedir(dir);
}

void send_file(int client_socket, const char* folder_path, const char* file_name) {
    char file_path[BUFFER_SIZE];
    sprintf(file_path, "%s/%s", folder_path, file_name);

    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        char* error_message = "ERROR File does not exist\r\n";
        send(client_socket, error_message, strlen(error_message), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char response[BUFFER_SIZE];
    sprintf(response, "OK %d\r\n", file_size);
    send(client_socket, response, strlen(response), 0);

    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

void handle_client(int client_socket, const char* folder_path) {
    char buffer[BUFFER_SIZE];

    // Nhận tên file từ client
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }
    buffer[bytes_received] = '\0';

    // Gửi danh sách file hoặc thông báo lỗi
    if (strcmp(buffer, "GET_LIST") == 0) {
        send_file_list(client_socket, folder_path);
    } else {
        send_file(client_socket, folder_path, buffer);
    }

    close(client_socket);
}

int main() {
    const char* folder_path = "D:\\"; // Đường dẫn thư mục chứa các file

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Cấu hình địa chỉ server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(12345); // Port để lắng nghe

    // Ràng buộc socket với địa chỉ server
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Binding failed");
        return 1;
    }

    // Lắng nghe kết nối từ client
    if (listen(server_socket, 5) < 0) {
        perror("Listening failed");
        return 1;
    }

    printf("Server listening on port 12345\n");

    while (1) {
        // Chấp nhận kết nối từ client
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Accepting connection failed");
            return 1;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Xử lý yêu cầu từ client trong một tiến trình con
        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("Fork failed");
            return 1;
        } else if (child_pid == 0) {
            close(server_socket); // Đóng socket của tiến trình cha trong tiến trình con

            handle_client(client_socket, folder_path);

            exit(0);
        } else {
            close(client_socket); // Đóng socket của tiến trình con trong tiến trình cha
        }
    }

    close(server_socket);

    return 0;
}