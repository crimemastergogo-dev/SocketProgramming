#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

int main()
{
    WSADATA     WSAData;
    SOCKET      S_socket; /*Used to create server*/
    struct sockaddr_in C_add; /*Used in accept */
    char        buffer[20] = "satvik";

    WSAStartup(MAKEWORD(2,0),&WSAData);
    S_socket = socket(AF_INET,SOCK_STREAM,0);

    C_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    C_add.sin_family      = AF_INET;
    C_add.sin_port        = htons(5555);

    connect(S_socket,(struct sockaddr_in*)&C_add,sizeof(C_add));

    printf("Connecting to server....\n");

    send(S_socket,buffer,sizeof(buffer),0);
    printf("Message Sent...!!!\n");

    WSACleanup();

    return 0;
}
