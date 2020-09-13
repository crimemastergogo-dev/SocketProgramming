#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main()
{
    WSADATA     WSAData;
    SOCKET      S_socket;  /*Used to create server*/
    SOCKET      C_socket;  /*Used in accept  from client*/
    struct sockaddr_in S_add;
    struct sockaddr_in C_add; /*Used in accept */
    char        buffer[20];
    int         C_add_size = 0;

    WSAStartup(MAKEWORD(2,0),&WSAData);
    S_socket = socket(AF_INET,SOCK_STREAM,0);

    S_add.sin_addr.s_addr = INADDR_ANY;
    S_add.sin_family      = AF_INET;
    S_add.sin_port        = htons(5555);

    bind(S_socket,(SOCKADDR *)&S_add,sizeof(S_add));
    listen(S_socket,0); /*second argument represents no. of client connection in Queue*/

    printf("Listening Incommig messages....\n");
    C_add_size = sizeof(C_add);
    if ((C_socket = accept(S_socket,(SOCKADDR *)&C_add, &C_add_size))!= INVALID_SOCKET)
    {
        printf("Client Connected....!!!\n");
        recv(C_socket,buffer,sizeof(buffer),0);
        printf("Client says.... [%s]\n",buffer);

        closesocket(C_socket);
        printf("Socket Disconnected...\n");
    }

    WSACleanup();

    return 0;
}
