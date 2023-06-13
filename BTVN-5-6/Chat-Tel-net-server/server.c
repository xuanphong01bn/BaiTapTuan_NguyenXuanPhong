#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int client_count = 0;
int paired_count = 0;
int paired_clients[2];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
    
    pthread_mutex_lock(&mutex);
    
    client_count++;
    
    if (client_count % 2 == 1) {
        // Chờ đợi cặp khác
        pthread_cond_wait(&condition, &mutex);
    } else {
        // Ghép cặp client
        paired_clients[paired_count++] = client_socket;
        pthread_cond_broadcast(&condition);
    }
    
    pthread_mutex_unlock(&mutex);
    
    // Giao tiếp giữa hai client đã được ghép cặp
    char buffer[BUFFER_SIZE];
    int paired_socket = paired_clients[paired_count - 1];
    int read_size;
    
    while ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        send(paired_socket, buffer, read_size, 0);
        memset(buffer, 0, sizeof(buffer));
    }
    
    // Đóng kết nối và giải phóng resource
    close(client_socket);
    
    pthread_mutex_lock(&mutex);
    
    client_count--;
    paired_count--;
    
    // Nếu chỉ còn lại 1 client, đánh thức nó để kết thúc
    if (client_count == 1) {
        pthread_cond_signal(&condition);
    }
    
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    pthread_t thread_id;
    
    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Thiết lập địa chỉ và cổng cho server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Gắn địa chỉ và cổng cho socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
    
    // Lắng nghe kết nối từ client
    if (listen(server_fd, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is running on port %d\n", PORT);
    
    while (1) {
        // Chấp nhận kết nối từ client
        client_addr_len = sizeof(client_addr);
        if ((client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }
        
        // Tạo một thread để xử lý client mới
        if (pthread_create(&thread_id, NULL, client_handler, (void *)&client_socket) < 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        
        printf("Client connected\n");
    }
    
    // Đóng socket của server
    close(server_fd);
    
    return 0;
}