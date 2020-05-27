#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*These headeer files are used to invoke socket API(s)*/
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#define MAX_STRING_LIMIT 1024

typedef struct
{
    char *chr;
    int length;
}String;

void getString(String *ptr)
{
    char temp[MAX_STRING_LIMIT];
    int index =0;

    fgets(temp,MAX_STRING_LIMIT,stdin);
    int len = strlen(temp);

    if (0 < len )
    {
        ptr->chr = (char *)malloc(len);
    }
    else if ((0 >len) || (NULL == ptr->chr))
    {
        fprintf(stderr,"ERROR:Memory Allocation Failed\n");
        exit (1);
    }

    for (index =0;index<len;index++)
    {
        ptr->chr[index] = temp[index];
    }
    *(ptr->chr +len -1) = '\0';
    ptr->length = strlen(ptr->chr);
}

char *DisplayString(String *ptr)
{
    if (0 > ptr->length)
    {
        fprintf(stderr,"ERROR:string length is 0 no string found ");
        exit (1);
    }
    else
        return ptr->chr;
}


int main()
{
    fprintf(stdout,"main:\n");
    struct sockaddr_in server_address;
    int network_socket,client_socket;
    String *string     = NULL; 
    int choice         = 0;
    char data[1024];

    string = (String *)malloc(sizeof(String));
    if (NULL == string)
    {
        fprintf(stderr,"ERROR:Memory Allocation Failed\n");
        exit (1);
    }
    memset(string,0,sizeof(String));
    memset(data,0,sizeof(data));


    /*creating a socket*/
    network_socket = socket(AF_INET, SOCK_STREAM, 0); //tcp protocol socket
    fprintf(stdout,"create socket:\n");

    /*specify an address for the socket*/
    server_address.sin_family      = AF_INET;
    server_address.sin_port        = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;
    fprintf(stdout,"specify address:\n");

    /*binding*/
    bind(network_socket, 
            (struct sockaddr *)&server_address,
            sizeof(server_address));
    fprintf(stdout,"binding:\n");
    /*Listen*/
    listen(network_socket,3);
    fprintf(stdout,"Listen:\n");

    /*Accept*/
    client_socket = accept(network_socket, 
            NULL,
            NULL);
    fprintf(stdout,"Accept:\n");
    do
    {
        memset(string,0,sizeof(String));
        fprintf(stdout,"Enter the String :");
        fprintf(stdout,"Type exit to stop sending:");

        getString(string);

        memcpy(data,string->chr,string->length+1);


        /*Send data*/
        send(client_socket, 
                &data,
                sizeof(data),0);

#if 0
        if(0 == data )
        {
            break;
        }
#endif
    }while(strcmp("exit",data));

    /*close the socket*/
    close(network_socket);

    if ((NULL != string->chr) &&
            (NULL != string))
    {
        free(string->chr);
        string->chr = NULL;
        free(string);
        string = NULL;
    }

    return 0;
}
