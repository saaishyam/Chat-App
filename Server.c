#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define port 8080

int main(){

    //Server Configurations

    int socketfd;

    char Buffer[1024];

    int client_socket1;
    int client_socket2;

    socketfd = socket(AF_INET,SOCK_STREAM,0);

    if(socketfd <0){
        perror("Socket");
        return 1;
    }

    struct sockaddr_in server;

    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port =htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
    bind(socketfd,(struct sockaddr*) &server,sizeof(server));

    listen(socketfd,5);


    client_socket1 = accept(socketfd,NULL, NULL);
    client_socket2 = accept(socketfd,NULL,NULL);

    if(client_socket1 < 0){
        printf("accept");
    }
    else{
        printf("Client 1 Connected\n");
    }

    if(client_socket2 < 0){
        printf("accept");
    }
    else{
        printf("Client 2 connected\n");
    }

    pid_t p = fork();

    while (1){

        if(p > 0){
            int client1 = recv(client_socket1,Buffer,sizeof(Buffer),0);
            
            if (client1 > 0){
                send(client_socket2,Buffer,client1,0);
            }
            else if(client1 == 0){
                printf("Client 1 disconnected\n");
                close(client_socket1);
                break;
            }
            else if(client1 < 0){
                perror("recv");
                close(client_socket1);
                break;
            }
        }
        else if (p == 0){
            int client2 = recv(client_socket2,Buffer,sizeof(Buffer),0);

            if (client2 > 0){
                send(client_socket1,Buffer,client2,0);
            }
            else if(client2 == 0){
                printf("Client2 disconnected\n");
                close(client_socket2);
                break;
            }
             else if(client2 < 0){
                perror("recv");
                close(client_socket2);
                break;
            }
        }
  
    }
}