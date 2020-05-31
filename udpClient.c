#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*These header files are used to invoke socket API(s)*/
#include<sys/types.h>
#include<sys/socket.h>
#define MAX_STRING_LIMIT 1024

#include<netinet/in.h>

int main()
{
    fprintf(stdout,"Main:\n");
    int network_socket,connect_status;
    char data[1024];

    /*creating a socket*/
    network_socket = socket(AF_INET, SOCK_DGRAM, 0); //udp protocol socket
    fprintf(stdout,"create :\n");
    
    /*specify an address for the socket*/
    struct sockaddr_in server_address;
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;
    fprintf(stdout,"specify add :\n");

    /*Receive*/
    fprintf(stdout,"Receive:\n");
    do
    {
        recv(network_socket, 
                &data,
                sizeof(data),0);
        fprintf(stdout,"Received:\n");
        /*printing Received Data*/
        fprintf(stdout,"printing:\n");
        fprintf(stdout,"Data Received: %s\n",data);
        if (0 == data)
        {
            break;
        }
    }while(strcmp("exit",data));

    /*close socket*/
    close(network_socket);

    return 0;
}
