#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int connect_to_server(const char *server_ip, int server_port)
{
    int client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &(server_address.sin_addr)) <= 0)
    {
        perror("Invalid address or address not supported");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error connecting to server");
        exit(1);
    }

    return client_socket;
}

void send_command(int client_socket, const char *command)
{
    send(client_socket, command, strlen(command), 0);
    send(client_socket, "\r\n", 2, 0);
}

void receive_response(int client_socket, char *response)
{
    memset(response, 0, BUFFER_SIZE);
    recv(client_socket, response, BUFFER_SIZE - 1, 0);
}

int get_pasv_port(const char *response)
{
    char *start = strchr(response, '(');
    char *end = strchr(response, ')');
    if (start && end)
    {
        int a, b, c, d, port_high, port_low;
        sscanf(start, "(%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &port_high, &port_low);
        return (port_high << 8) + port_low;
    }
    return -1;
}

int get_epsv_port(const char *response)
{
    char *start = strchr(response, '|');
    char *end = strrchr(response, '|');
    if (start && end)
    {
        int port;
        sscanf(start + 1, "%d", &port);
        return port;
    }
    return -1;
}

void login(int client_socket, const char *username, const char *password)
{
    char response[BUFFER_SIZE];
    receive_response(client_socket, response);

    send_command(client_socket, "USER");
    receive_response(client_socket, response);

    char command[BUFFER_SIZE];
    snprintf(command, BUFFER_SIZE, "USER %s", username);
    send_command(client_socket, command);
    receive_response(client_socket, response);

    send_command(client_socket, "PASS");
    receive_response(client_socket, response);

    snprintf(command, BUFFER_SIZE, "PASS %s", password);
    send_command(client_socket, command);
    receive_response(client_socket, response);
}

void upload_file(int client_socket, const char *file_path, const char *file_name)
{
    char response[BUFFER_SIZE];

    send_command(client_socket, "PASV");
    receive_response(client_socket, response);
    int pasv_port = get_pasv_port(response);

    send_command(client_socket, "EPSV");
    receive_response(client_socket, response);
    int epsv_port = get_epsv_port(response);

    int data_socket = -1;
    if (epsv_port != -1)
    {
        data_socket = connect_to_server("localhost", epsv_port);
    }
    else if (pasv_port != -1)
    {
        char server_ip[BUFFER_SIZE];
        snprintf(server_ip, BUFFER_SIZE, "%s.%s.%s.%s", "127", "0", "0", "1");
        data_socket = connect_to_server(server_ip, pasv_port);
    }

    if (data_socket != -1)
    {
        send_command(client_socket, "TYPE I");
        receive_response(client_socket, response);

        FILE *file = fopen(file_path, "rb");
        if (file)
        {
            send_command(client_socket, "STOR");
            receive_response(client_socket, response);

            char command[BUFFER_SIZE];
            snprintf(command, BUFFER_SIZE, "STOR %s", file_name);
            send_command(client_socket, command);
            receive_response(client_socket, response);

            char buffer[BUFFER_SIZE];
            int bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
            {
                send(data_socket, buffer, bytes_read, 0);
            }

            fclose(file);
        }
        else
        {
            printf("Error opening file: %s\n", file_path);
        }

        close(data_socket);
    }
    else
    {
        printf("Data connection failed.\n");
    }
}

int main()
{
    const char *server_ip = "127.0.0.1"; // Thay đổi địa chỉ IP máy chủ FTP
    int server_port = 21;                // Thay đổi cổng máy chủ FTP

    const char *username = "your-username"; // Thay đổi tên đăng nhập của bạn
    const char *password = "your-password"; // Thay đổi mật khẩu của bạn

    const char *file_path = "/path/to/local/file.txt"; // Thay đổi đường dẫn tệp tin cần tải lên
    const char *file_name = "file.txt";                // Thay đổi tên tệp tin trên máy chủ FTP

    int client_socket = connect_to_server(server_ip, server_port);

    char response[BUFFER_SIZE];
    receive_response(client_socket, response);
    printf("%s", response);

    login(client_socket, username, password);

    upload_file(client_socket, file_path, file_name);

    send_command(client_socket, "QUIT");
    receive_response(client_socket, response);
    printf("%s", response);

    close(client_socket);

    return 0;
}