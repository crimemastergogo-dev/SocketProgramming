#include <stdio.h>          
#include <stdlib.h>        
#include <string.h>
#include <sys/socket.h>       
#include <sys/types.h>       
#include <netinet/in.h>        
#include <arpa/inet.h>          
#include <unistd.h>           
#include <errno.h>             


typedef struct
{
    int  num;
    char name[20];
}student;

void Unpack(char buffer[sizeof(student)],student *std)
{
    int idx = 0;
    typedef union
    {
        student temp;
        char str[sizeof(student)];
    }Expand;

    Expand E;

    for (idx = 0 ; idx < sizeof(student); idx++)
        E.str[idx] = buffer[idx];

    std->num = E.temp.num;
    memcpy(std->name,E.temp.name,sizeof(char)*20);

}

int main()
{
    struct sockaddr_in Server_Address;
    student *std   = NULL;
    int     server = 0;
    int     idx = 0;
    char buffer[sizeof(student)];

    std = (student *)malloc(sizeof(student));
    if (NULL == std)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }
    memset(std,0,sizeof(student));

    server = socket(AF_INET,SOCK_DGRAM,0);
    if (0 > server)
    {
        perror ("Server Socket creation failed...!!!\n");
        exit(1);
    }
    else
    {
        printf ("UDP Server Socket creation is SUCCESSFUL...!!!\n");
    }

    Server_Address.sin_addr.s_addr = INADDR_ANY;
    Server_Address.sin_family      = AF_INET;
    Server_Address.sin_port        = htons(5555);

    if (0 > bind(server,(struct sockaddr *)&Server_Address,sizeof(Server_Address)))
    {
        perror ("UDP BIND failed...!!!\n");
        exit(1);
    }
    else
    {
        printf ("UDP Server Socket Binding is SUCCESSFUL...!!!\n");
    }

    recv(server,buffer,sizeof(buffer),0);
    printf("Serialized data Received :");
    for (idx = 0 ;idx < sizeof(student);idx++)
        printf(" %0x",buffer[idx]);
    printf("\n");

    Unpack(buffer,std);
    printf("Client Send Roll no [%d]\n",std->num);
    printf("Client Send Name    [%s]\n",std->name);

    close(server);
    if (NULL !=std)
    {
        free(std);
        std = NULL;
    }
}
