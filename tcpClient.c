#include<stdio.h>
#include<stdlib.h>
/*These headeer files are used to invoke socket API(s)*/
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>

int main()
{
    int network_socket,connect_status;
    char server_response[256];
    /*creating a socket*/
    network_socket = socket(AF_INET, SOCK_STREAM, 0); //tcp protocol socket
    
    /*specify an address for the socket*/
    struct sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /*connet*/
    connect_status = connect(network_socket, 
                             (struct sockaddr *)&server_address,
                             sizeof(server_address));
    if (connect_status == -1)
    {
        fprintf(stderr,"ERROR IN CONNECT\n");
    }
    /*Receive*/
    
    recv(network_socket, 
            &server_response,
            sizeof(server_response),0);
    /*printing Received Data*/
    fprintf(stdout,"data recieved : %s \n",server_response);
    
    /*close socket*/
    close(network_socket);
    return 0;
}
