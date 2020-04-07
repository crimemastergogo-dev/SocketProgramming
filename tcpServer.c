#include<stdio.h>
#include<stdlib.h>
/*These headeer files are used to invoke socket API(s)*/
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

int main()
{
    struct sockaddr_in server_address;
    int network_socket,client_socket;
    char server_message[256] = "you have reached the server";

    /*creating a socket*/
    network_socket = socket(AF_INET, SOCK_STREAM, 0); //tcp protocol socket
    
    /*specify an address for the socket*/
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /*binding*/
   bind(network_socket, 
        (struct sockaddr *)&server_address,
        sizeof(server_address));
    /*Listen*/
    listen(network_socket,3);
    
    /*Accept*/
    client_socket = accept(network_socket, 
                           NULL,
                           NULL);
    /*Send data*/
    send(client_socket, 
         server_message,
         sizeof(server_message),0);

    /*close the socket*/
    close(network_socket);
    return 0;
}
