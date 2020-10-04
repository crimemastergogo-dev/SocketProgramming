#include <stdio.h>          
#include <stdlib.h>        
#include <string.h>
#include <sys/socket.h>       
#include <sys/types.h>       
#include <netinet/in.h>        
#include <netinet/sctp.h>        
#include <arpa/inet.h>          
#include <unistd.h>           
#include <errno.h>             


typedef struct
{
    int  num;
    char name[20];
}student;

student *Unpack(char buffer[sizeof(student)])
{
    student *std   = NULL;
    int idx = 0;
    typedef union
    {
        student temp;
        char str[sizeof(student)];
    }Expand;

    Expand E;
    std = (student *)malloc(sizeof(student));
    if (NULL == std)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }
    memset(std,0,sizeof(student));

    for (idx = 0 ; idx < sizeof(student); idx++)
        E.str[idx] = buffer[idx];

    std->num = E.temp.num;
    memcpy(std->name,E.temp.name,sizeof(char)*20);
    return std;
}

int main()
{
    struct sockaddr_in Server_Address;
    struct sctp_initmsg initmsg;
    student *std   = NULL;
    int     server = 0;
    int     client = 0;
    int     idx = 0;
    char buffer[sizeof(student)];

    std = (student *)malloc(sizeof(student));
    if (NULL == std)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }
    memset(std,0,sizeof(student));

    server = socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP);

    Server_Address.sin_addr.s_addr = INADDR_ANY;
    Server_Address.sin_family      = AF_INET;
    Server_Address.sin_port        = htons(5555);

    bind(server,(struct sockaddr *)&Server_Address,sizeof(Server_Address));

    memset(&initmsg,0,sizeof(initmsg));
    initmsg.sinit_num_ostreams  = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts  = 4;

    setsockopt(server,IPPROTO_SCTP,SCTP_INITMSG,&initmsg,sizeof(initmsg));

    listen(server,0);

    printf("Listening for incomming connections...!!!\n");

    client = accept(server, NULL,NULL);
    printf("Client Connected...!!!\n");
    recv(client,buffer,sizeof(buffer),0);
    printf("Serialized data Received :");
    for (idx = 0 ;idx < sizeof(student);idx++)
        printf(" %0x",buffer[idx]);
    printf("\n");

    std = Unpack(buffer);
    printf("Client Send Roll no [%d]\n",std->num);
    printf("Client Send Name    [%s]\n",std->name);
    close(client);
    close(server);
    if (NULL !=std)
    {
        free(std);
        std = NULL;
    }
}
