#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/if_ether.h>
#include<netinet/tcp.h>
#include<linux/if_packet.h>
#include<arpa/inet.h>
#include <unistd.h>

#define DESTMAC0	0xd0
#define DESTMAC1	0x67
#define DESTMAC2	0xe5
#define DESTMAC3	0x12
#define DESTMAC4	0x6f
#define DESTMAC5	0x8f

typedef struct
{
    int  num;
    char name[20];
}student;

unsigned short checksum(unsigned short* buff, int _16bitword)
{
   /*When the packet arrives at the router, it calculates the checksum, 
     and if the calculated checksum does not match with the checksum field of the header,
     the router will drop the packet; and if it matches, 
     the router will decrement the time to the live field by one, and forward it.
    */

   /*To calculate the checksum, sum up all the 16-bit words of the IP header and if there is any carry,
     add it again to get a 16-bit word. After this, find the complement of 1's and that is our checksum.
     To check whether our checksum is correct
    */

    unsigned long sum;
    for(sum=0;_16bitword>0;_16bitword--)
        sum+=htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum>>16);
    return (unsigned short)(~sum);
}

unsigned char pack(student *p,int idx)
{
    typedef union
    {
        student temp2;
        unsigned char str[sizeof(student)];
    }dump;

    dump D;
    D.temp2.num = p->num;
    memcpy(D.temp2.name,p->name,20*sizeof(char));

    return D.str[idx];
}

int main()
{
    struct sockaddr_ll Client_Address;
    struct ifreq     Iifreq;
    struct ifreq     Mifreq;
    struct ifreq     IPifreq;
    student          *std       = NULL;
    struct ethhdr    *eth       = NULL;
    struct iphdr     *ip        = NULL;
    struct tcphdr    *tcp       = NULL;
    int              raw_client = 0;
    int              send       = 0;
    int              idx        = 0;
    int              total_len  = 0;
    unsigned char    temp[20]   = {'s','a','t','v','i','k'};
    unsigned char    *sendbuff  = NULL;
    unsigned char    buffer[sizeof(student)];

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

    raw_client = socket(AF_INET,SOCK_STREAM,0);
    if (0 > raw_client)
    {
        perror("Socket Creation Failed\n");
        exit(1);
    }
    else
    {
        printf("Socket Creation Successful\n");
    }

    /* Getting Index to send packet */
    memset(&Iifreq,0,sizeof(struct ifreq));
    memcpy(Iifreq.ifr_name,"lo",IFNAMSIZ-1);

    if (0 > (ioctl(raw_client,SIOCGIFINDEX,&Iifreq)))
    {
        perror("Error in Fetching Index\n");
        exit(1);
    }
    else
    {
        printf("Index = %d\n",Iifreq.ifr_ifindex);
    }

    /*Fetching MAC address*/
    memset(&Mifreq,0,sizeof(struct ifreq));
    memcpy(Mifreq.ifr_name,"lo",IFNAMSIZ-1);

    if (0 > (ioctl(raw_client,SIOCGIFHWADDR,&Mifreq)))
    {
        perror("Error in SIGOCGIFHWADDR ioctl reading\n");
        exit(1);
    }
    else
    {
        printf("SIGOCGIFHWADDR ioctl reading SUCCESSFULL\n");
    }

    /* Fetching IP ADDRESS*/
    memset(&IPifreq,0,sizeof(struct ifreq));
    memcpy(IPifreq.ifr_name,"lo",IFNAMSIZ-1);

    if (0 > (ioctl(raw_client,SIOCGIFADDR,&IPifreq)))
    {
        perror("Error in SIGOCGIFADDR ioctl reading\n");
        exit(1);
    }
    else
    {
        printf("SIGOCGIFADDR ioctl reading SUCCESSFULL\n");
    }

    /* Construction Ethernet Header */
    sendbuff = (unsigned char *) malloc(78);   /* 14 + 20 + 20 + 24*/
    memset(sendbuff,0,78);

    eth = (struct ethhdr *)(sendbuff);

    eth->h_source[0] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[0]);
    eth->h_source[1] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[1]);
    eth->h_source[2] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[2]);
    eth->h_source[3] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[3]);
    eth->h_source[4] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[4]);
    eth->h_source[5] = (unsigned char)(Mifreq.ifr_hwaddr.sa_data[5]);

    /* filling destination mac. DESTMAC0 to DESTMAC5 are macro having octets of mac address. */
    eth->h_dest[0] = DESTMAC0;
    eth->h_dest[1] = DESTMAC1;
    eth->h_dest[2] = DESTMAC2;
    eth->h_dest[3] = DESTMAC3;
    eth->h_dest[4] = DESTMAC4;
    eth->h_dest[5] = DESTMAC5;

    eth->h_proto = htons(ETH_P_IP);

    total_len += sizeof(struct ethhdr);

    /* Constructing IP header */
    ip = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 16;
    ip->id = htons(10201);
    ip->ttl = 64;
    ip->protocol = 17;
    ip->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(IPifreq.ifr_addr))->sin_addr)));
    ip->daddr = inet_addr("127.0.0.1"); // put destination IP address

    total_len += sizeof(struct iphdr);

    /* Constructing TCP header*/
    tcp = (struct tcphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));

    tcp->source      = htons(5554);
    tcp->dest        = htons(5555);
    tcp->seq         = htonl(1);
    tcp->ack_seq     = 0;
    tcp->doff        = 5;
    tcp->syn         = 1;
    tcp->ack         = 0;
    tcp->window      = htons(32767);
    tcp->check       = 0; /* Done by kernel */
    tcp->urg_ptr     = 0;

    /*Remaining Field of IP header*/

    /*IP length*/
    ip->tot_len = htons(total_len - sizeof(struct ethhdr));

    /*IP Checksum for error corretcion*/
    ip->check = checksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));

    /*Adding data to packet*/
    for (idx = 0 ; idx < sizeof(student);idx ++)
    {
        sendbuff[total_len] = buffer[idx];
        total_len++;
    }

    /*Sending Packet*/
    Client_Address.sll_ifindex = Iifreq.ifr_ifindex; // index of interface
    Client_Address.sll_halen   = ETH_ALEN;           // length of destination mac address
    Client_Address.sll_addr[0] = DESTMAC0;
    Client_Address.sll_addr[1] = DESTMAC1;
    Client_Address.sll_addr[2] = DESTMAC2;
    Client_Address.sll_addr[3] = DESTMAC3;
    Client_Address.sll_addr[4] = DESTMAC4;
    Client_Address.sll_addr[5] = DESTMAC5;

    send = sendto(raw_client,sendbuff,78,0,(struct sockaddr*)&Client_Address,sizeof(struct sockaddr_ll));
    if (0 > send)
    {
        perror("Send Un-successfull..!!!\n");
        exit(1);
    }
    else
    {
        printf("Send successfull...!!!\n");
    }

    close(raw_client);
    if (NULL !=std)
    {
        free(std);
        std = NULL;
    }
}
