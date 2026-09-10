#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define port 8080
#define SIZE 1024

int main(){

    int c_socket;

    char sendBuffer[SIZE];
    char RecieveBuffer[SIZE];
    

    c_socket = socket(AF_INET,SOCK_STREAM,0);

    if(c_socket < 0){
        perror("Socket");
        return 1;
    }

    struct sockaddr_in server;

    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port =htons(port);

    inet_pton(AF_INET,"127.0.0.1", &server.sin_addr);

    if (connect(c_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {

        perror("connect");
        return 1;

    }

    memset(sendBuffer,0,sizeof(sendBuffer));

    pid_t p = fork();

    while(1){

        if (p > 0)
        {
            //Send Message

            printf("Message->\n");
            fgets(sendBuffer,SIZE,stdin);

            sendBuffer[strcspn(sendBuffer,"\n")] = '\0';

            send(c_socket,sendBuffer,strlen(sendBuffer),0);
        }
        else if(p == 0){

            //Recieve the message

            int bytes = recv(c_socket,RecieveBuffer,sizeof(RecieveBuffer)-1 ,0);

            if (bytes< 0){
                perror("Recv\n");
                return 1;
            }

            if(bytes > 0){

                RecieveBuffer[bytes] = '\0';
                printf("Client Message->%s\n",RecieveBuffer);
            }
        }
    }
}