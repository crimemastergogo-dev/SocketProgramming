#include <stdio.h>          
#include <stdlib.h>        
#include <string.h>
#include <sys/socket.h>       
#include <sys/types.h>       
#include <netinet/in.h>        
#include <arpa/inet.h>          
#include <unistd.h>           
#include <errno.h>             


int main()
{
    struct sockaddr_in Server_Address;
    int     server = 0;
    int     client = 0;
    char    buffer[9999];

    strcpy(buffer,"HTTP/1.1 200 OK\nContent-length: 47\nContent-Type: text/html\n\n<html><body><H1>I love Food</H1></body></html>");

    server = socket(AF_INET,SOCK_STREAM,0);

    Server_Address.sin_addr.s_addr = INADDR_ANY;
    Server_Address.sin_family      = AF_INET;
    Server_Address.sin_port        = htons(5555);

    bind(server,(struct sockaddr *)&Server_Address,sizeof(Server_Address));
    listen(server,0);

    printf("Listening for incomming connections...!!!\n");

    client = accept(server, NULL,NULL);
    printf("Client Connected...!!!\n");
    send(client,buffer,sizeof(buffer),0);

    close(client);
    close(server);
}
