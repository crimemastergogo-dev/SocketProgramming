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

char pack(student *p,int idx)
{
    typedef union
    {
        student temp2;
        char str[sizeof(student)];
    }dump;

    dump D;
    D.temp2.num = p->num;
    memcpy(D.temp2.name,p->name,20*sizeof(char));

    return D.str[idx];
}

int main()
{
    struct sockaddr_in Client_Address;
    student *std   = NULL;
    int     client = 0;
    int     idx = 0;
    char    temp[20] = {'s','a','t','v','i','k'};
    char    buffer[sizeof(student)];

    std = (student *)malloc(sizeof(student));
    if (NULL == std)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }
    memset(std,0,sizeof(student));

    std->num = 50;
    memcpy(std->name,temp,20*sizeof(char));

    for (idx = 0 ; idx < sizeof(student);idx ++)
        buffer[idx] = pack(std,idx);

    client = socket(AF_INET,SOCK_STREAM,0);

    Client_Address.sin_addr.s_addr = inet_addr("127.0.0.1");
    Client_Address.sin_family      = AF_INET;
    Client_Address.sin_port        = htons(5555);

    connect(client,(struct sockaddr *)&Client_Address,sizeof(Client_Address));
    printf("Connected to server...!!!\n");
    printf("Sending serialized data : ");
    for (idx = 0 ; idx < sizeof(student);idx ++)
        printf(" %x",buffer[idx]);
    printf("\n");

    send(client,buffer,sizeof(buffer),0);


    close(client);
    if (NULL !=std)
    {
        free(std);
        std = NULL;
    }
}
