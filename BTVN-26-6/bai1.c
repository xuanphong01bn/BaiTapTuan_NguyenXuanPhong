#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define ROOT_DIRECTORY "/path/to/root/directory" // Thay đổi thành đường dẫn thư mục gốc của bạn

void send_response(int client_socket, const char *message)
{
    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s", strlen(message), message);
    send(client_socket, response, strlen(response), 0);
}

void handle_directory_request(int client_socket, const char *directory)
{
    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL)
    {
        strcat(response, "<html><body>\n");
        while ((ent = readdir(dir)) != NULL)
        {
            char entry_path[BUFFER_SIZE];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", directory, ent->d_name);

            if (ent->d_type == DT_DIR)
            {
                char link[BUFFER_SIZE];
                snprintf(link, sizeof(link), "<a href=\"%s\">%s/</a><br>\n", ent->d_name, ent->d_name);
                strcat(response, link);
            }
            else
            {
                char link[BUFFER_SIZE];
                snprintf(link, sizeof(link), "<a href=\"%s\">%s</a><br>\n", ent->d_name, ent->d_name);
                strcat(response, link);
            }
        }
        strcat(response, "</body></html>\n");

        closedir(dir);
    }
    else
    {
        strcpy(response, "Error opening directory.");
    }

    send_response(client_socket, response);
}

void handle_file_request(int client_socket, const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *file_content = malloc(file_size + 1);
        fread(file_content, file_size, 1, file);
        fclose(file);

        send_response(client_socket, file_content);
        free(file_content);
    }
    else
    {
        char error_message[BUFFER_SIZE];
        sprintf(error_message, "Error opening file: %s", file_path);
        send_response(client_socket, error_message);
    }
}

void handle_request(int client_socket, const char *request)
{
    char method[BUFFER_SIZE], path[BUFFER_SIZE], protocol[BUFFER_SIZE];
    sscanf(request, "%s %s %s", method, path, protocol);

    if (strcmp(method, "GET") == 0)
    {
        char file_path[BUFFER_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", ROOT_DIRECTORY, path);

        struct stat file_info;
        if (stat(file_path, &file_info) == 0)
        {
            if (S_ISDIR(file_info.st_mode))
            {
                handle_directory_request(client_socket, file_path);
            }
            else if (S_ISREG(file_info.st_mode))
            {
                handle_file_request(client_socket, file_path);
            }
        }
        else
        {
            send_response(client_socket, "404 - File Not Found");
        }
    }
    else
    {
        send_response(client_socket, "400 - Bad Request");
    }
}

void start_server(int port)
{
    int server_socket;
    struct sockaddr_in server_address;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket");
        exit(1);
    }

    if (listen(server_socket, 10) < 0)
    {
        perror("Error listening on socket");
        exit(1);
    }

    printf("Server started on port %d\n", port);

    while (1)
    {
        int client_socket;
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket < 0)
        {
            perror("Error accepting connection");
            exit(1);
        }

        char request[BUFFER_SIZE];
        memset(request, 0, sizeof(request));
        recv(client_socket, request, sizeof(request) - 1, 0);

        printf("Received request:\n%s\n", request);

        handle_request(client_socket, request);

        close(client_socket);
    }

    close(server_socket);
}

int main()
{
    int port = 8080; // Thay đổi cổng nếu cần thiết
    start_server(port);
    return 0;
}