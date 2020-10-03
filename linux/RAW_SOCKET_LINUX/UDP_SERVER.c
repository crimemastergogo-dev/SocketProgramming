#include <stdio.h>          
#include <stdlib.h>        
#include <string.h>
#include <sys/socket.h>       
#include <sys/types.h>       
#include <netinet/in.h>        
#include <arpa/inet.h>          
#include <unistd.h>           
#include <errno.h>             
#include <netinet/if_ether.h>             
#include <netinet/ip.h>             
#include <netinet/udp.h>             
#include <sys/ioctl.h>
#include <net/if.h>


typedef struct
{
    int  num;
    char name[20];
}student;

student *Unpack(unsigned char *buffer)
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
    struct sockaddr raw_Addr;
    struct sockaddr_in source;
    struct sockaddr_in dest;
    struct ethhdr   *eth         = NULL;
    struct iphdr    *ip          = NULL;
    struct udphdr   *udp         = NULL;
    student         *std         = NULL;
    unsigned  char  *buffer      = NULL;
    unsigned  char  *DATA        = NULL;
    int             raw_server   = 0;
    int             raw_Addr_len = 0;
    int             buffer_len   = 0;
    int             ip_hdr_len   = 0;
    int             FData_len    = 0;
    int             idx          = 0;


    buffer =  (unsigned char *)malloc(65536);
    if (NULL == buffer)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }

    memset(buffer,0,65536);

    raw_server = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if (0 > raw_server)
    {
        perror("Failed to open raw socket\n");
        exit(1);
    }
    else
    {
        printf("RAW spcket created successfully\n");
    }

    memset(&raw_Addr_len,0,sizeof(struct sockaddr));
    buffer_len = recvfrom(raw_server,buffer,65536,0,&raw_Addr,(socklen_t *)&raw_Addr_len);
    if (0 > buffer_len)
    {
        perror("Failed to Recv \n");
        exit(1);
    }
    else
    {
        printf("Received successfully\n");
    }

    std = (student *)malloc(sizeof(student));
    if (NULL == std)
    {
        printf("Memory Allocation Failed ...!!!\n");
        exit(1);
    }
    memset(std,0,sizeof(student));

    eth = (struct ethhdr *)buffer;
    printf("\nEthernet Header\n");
    printf("\t|-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    printf("\t|-Protocol            : %d\n",eth->h_proto);

    ip  = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    memset(&source,0,sizeof(struct sockaddr));
    memset(&dest,0,sizeof(struct sockaddr));

    source.sin_addr.s_addr = ip->saddr;
    dest.sin_addr.s_addr   = ip->daddr;

    printf("\nIP Header\n");
    printf("\t|-Version                : %d\n",(unsigned int)ip->version);
    printf("\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
    printf("\t|-Type Of Service        : %d\n",(unsigned int)ip->tos);
    printf("\t|-Total Length           : %d Bytes\n",ntohs(ip->tot_len));
    printf("\t|-Identification         : %d\n",ntohs(ip->id));
    printf("\t|-Time To Live           : %d\n",(unsigned int)ip->ttl);
    printf("\t|-Protocol               : %d\n",(unsigned int)ip->protocol);
    printf("\t|-Header Checksum        : %d\n",ntohs(ip->check));
    printf("\t|-Source IP              : %s\n", inet_ntoa(source.sin_addr));
    printf("\t|-Destination IP         : %s\n",inet_ntoa(dest.sin_addr));

    ip_hdr_len = ip->ihl *4;

    udp = (struct udphdr*)(buffer + ip_hdr_len + sizeof(struct ethhdr));

    printf("\nudp Header\n");
    printf("\t|-Source Port      : %d\n ", ntohs(udp->source));
    printf("\t|-Destination Port : %d\n ", ntohs(udp->dest));
    printf("\t|-udp Length       : %d\n" , ntohs(udp->len));
    printf("\t|-udp CheckSum     : %d\n" , ntohs(udp->check));


    DATA = (buffer + ip_hdr_len + sizeof(struct ethhdr) + sizeof(struct udphdr));
    FData_len = buffer_len - (ip_hdr_len +sizeof(struct ethhdr)+ sizeof(struct udphdr));

    std = Unpack(DATA);

    printf("\nDATA RECEIVED\n");
    for (idx = 0 ; idx < FData_len;idx++)
        printf(" %0x",DATA[idx]);

    printf("\nClient Send Roll no [%d]\n",std->num);
    printf("Client Send Name    [%s]\n",std->name);


    close(raw_server);

    if (NULL !=std)
    {
        free(std);
        std = NULL;
    }
}
