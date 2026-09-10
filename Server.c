#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int socketfd;
    char Buffer[BUFFER_SIZE];

    int client_socket1 = -1;
    int client_socket2 = -1;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0)
    {
        perror("Socket");
        return 1;
    }

    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;

    setsockopt(
        socketfd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    if (bind(socketfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind");
        close(socketfd);
        return 1;
    }

    if (listen(socketfd, 5) < 0)
    {
        perror("Listen");
        close(socketfd);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    fd_set readfds;

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(socketfd, &readfds);

        if (client_socket1 != -1)
            FD_SET(client_socket1, &readfds);

        if (client_socket2 != -1)
            FD_SET(client_socket2, &readfds);

        int max_fd = socketfd;

        if (client_socket1 > max_fd)
            max_fd = client_socket1;

        if (client_socket2 > max_fd)
            max_fd = client_socket2;

        int activity = select(
            max_fd + 1,
            &readfds,
            NULL,
            NULL,
            NULL
        );

        if (activity < 0)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(socketfd, &readfds))
        {
            int new_client = accept(socketfd, NULL, NULL);

            if (new_client < 0)
            {
                perror("accept");
            }
            else if (client_socket1 == -1)
            {
                client_socket1 = new_client;
                printf("Client 1 connected\n");
            }
            else if (client_socket2 == -1)
            {
                client_socket2 = new_client;
                printf("Client 2 connected\n");
            }
            else
            {
                close(new_client);
            }
        }

        if (client_socket1 != -1 &&
            FD_ISSET(client_socket1, &readfds))
        {
            int bytes = recv(
                client_socket1,
                Buffer,
                sizeof(Buffer),
                0
            );

            if (bytes > 0)
            {
                if (client_socket2 != -1)
                    send(client_socket2, Buffer, bytes, 0);
            }
            else if (bytes == 0)
            {
                printf("Client 1 disconnected\n");
                close(client_socket1);
                client_socket1 = -1;
            }
            else
            {
                perror("recv");
                close(client_socket1);
                client_socket1 = -1;
            }
        }

        if (client_socket2 != -1 &&
            FD_ISSET(client_socket2, &readfds))
        {
            int bytes = recv(
                client_socket2,
                Buffer,
                sizeof(Buffer),
                0
            );

            if (bytes > 0)
            {
                if (client_socket1 != -1)
                    send(client_socket1, Buffer, bytes, 0);
            }
            else if (bytes == 0)
            {
                printf("Client 2 disconnected\n");
                close(client_socket2);
                client_socket2 = -1;
            }
            else
            {
                perror("recv");
                close(client_socket2);
                client_socket2 = -1;
            }
        }
    }

    close(socketfd);

    return 0;
}