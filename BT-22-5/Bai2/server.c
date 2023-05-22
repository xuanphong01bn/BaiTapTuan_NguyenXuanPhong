#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fds[MAX_CLIENTS], max_clients = 0;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Khởi tạo server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // Liên kết socket với địa chỉ server
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Lỗi khi liên kết socket");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối từ client
    if (listen(server_fd, 5) == -1) {
        perror("Lỗi khi lắng nghe kết nối");
        exit(EXIT_FAILURE);
    }

    // Khởi tạo tập hợp socket và khởi động số lượng client đang kết nối
    fd_set read_fds, temp_fds;
    int max_fd, activity, i, j;
    char buffer[BUFFER_SIZE];

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    // Vòng lặp chính
    while (1) {
        temp_fds = read_fds;

        // Chờ sự kiện trên các socket
        activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Lỗi trong select");
            exit(EXIT_FAILURE);
        }

        // Kiểm tra sự kiện trên server socket
        if (FD_ISSET(server_fd, &temp_fds)) {
            int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_socket == -1) {
                perror("Lỗi khi chấp nhận kết nối");
                exit(EXIT_FAILURE);
            }

            // Thêm socket client mới vào tập hợp
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == 0) {
                    client_fds[i] = new_socket;
                    max_clients++;
                    break;
                }
            }

            // Gửi xâu chào với số lượng client đang kết nối
            sprintf(buffer, "Xin chào. Hiện có %d clients đang kết nối.\n", max_clients);
            send(new_socket, buffer, strlen(buffer), 0);

            // Cập nhật số lượng socket client tối đa
            if (new_socket > max_fd) {
                max_fd = new_socket;
            }

            // Xử lý nếu không còn sự kiện chờ
            if (--activity <= 0) {
                continue;
            }
        }

        // Kiểm tra sự kiện trên các socket client
        for (i = 0; i < MAX_CLIENTS; i++) {
            int client_socket = client_fds[i];

            if (FD_ISSET(client_socket, &temp_fds)) {
                // Đọc dữ liệu từ client
                memset(buffer, 0, sizeof(buffer));
                int read_bytes = read(client_socket, buffer, sizeof(buffer));
                if (read_bytes == 0) {
                    // Kết nối đã đóng
                    close(client_socket);
                    client_fds[i] = 0;
                    max_clients--;
                    break;
                }

                // Xử lý xâu ký tự từ client
                if (strcmp(buffer, "exit\n") == 0) {
                    // Client gửi "exit" -> gửi xâu tạm biệt và đóng kết nối
                    send(client_socket, "Tạm biệt!\n", 10, 0);
                    close(client_socket);
                    client_fds[i] = 0;
                    max_clients--;
                } else {
                    // Chuẩn hóa xâu ký tự và gửi kết quả cho client
                    char result[BUFFER_SIZE];
                    int result_length = 0;
                    int j = 0;
                    int is_space = 1;

                    for (j = 0; j < read_bytes; j++) {
                        char ch = buffer[j];
                        if (isalpha(ch)) {
                            if (is_space) {
                                result[result_length++] = toupper(ch);
                            } else {
                                result[result_length++] = tolower(ch);
                            }
                            is_space = 0;
                        } else if (isspace(ch)) {
                            is_space = 1;
                        }
                    }
                    result[result_length] = '\0';

                    send(client_socket, result, strlen(result), 0);
                }

                // Xử lý nếu không còn sự kiện chờ
                if (--activity <= 0) {
                    break;
                }
            }
        }
    }

    // Đóng server socket
    close(server_fd);

    return 0;
}