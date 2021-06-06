#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>

#define DEFAULT_PORT	"enp0s3"
#define BUF_SIZ		1024
#define ENP0S8  3


struct interfaces{
		
unsigned char name[IFNAMSIZ];
unsigned char hwaddr[ETH_ALEN];
int mtu;
int indx;

}ifc;

int get_info( unsigned char *port_name, int *fd);
void fill_info(char *sendbuf, struct sockaddr_ll *socket_address, int *tx_len);

int main(int argc, char *argv[])
{
	int sockfd, tx_len=0;
	unsigned char sendbuf[BUF_SIZ]={'\0'};
	struct sockaddr_ll socket_address;
   
   if(get_info(DEFAULT_PORT, &sockfd)) { printf("exiting main() function...!!!!!!!\n"); exit(EXIT_FAILURE);}	
      
   fill_info(sendbuf, &socket_address, &tx_len);
  
	/* Send packet */
   if(sendto(sockfd, sendbuf, tx_len, 0, &socket_address, sizeof(struct sockaddr_ll)) < 0)    //(struct sockaddr*) 
	   printf("Send failed\n");
/*    
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)    //(struct sockaddr*) 
	    printf("Send failed\n");
*/

   printf("Data sent successfully yayy...!!!\n\n");
   close(sockfd);
	return 0;
}

/***********************************************************************************************************/

 int get_info( unsigned char *port_name, int *fd)
 {	 
    struct ifreq ifr;
    
    if((*fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) 
    {
	    perror("socket");
	    return 1;
	 }

	 strncpy(ifr.ifr_name, port_name, IFNAMSIZ);
	 strncpy(ifc.name, port_name, IFNAMSIZ);
	   
	 if(ioctl(*fd, SIOCGIFINDEX, &ifr) < 0) { perror("EXITING the program..!!couldnt get index \t"); return 1; }
	 ifc.indx=ifr.ifr_ifindex;
	   	 
	 if(ioctl(*fd, SIOCGIFHWADDR, &ifr)<0) { perror("EXITING the program..!!couldnt get hardware address\t"); return 1; }
	 memcpy(ifc.hwaddr, ifr.ifr_hwaddr.sa_data, 6);   
			
	 if (ioctl(*fd, SIOCGIFMTU, &ifr)<0) { perror("EXITING the program..!!couldnt get MTU\t"); return 1; }  
	 ifc.mtu=ifr.ifr_mtu; 
/*	 
	 printf("indx=%d\t%s\t\t%d\t\t%02X:%02X:%02X:%02x:%02x:%02x\n",ifc.indx, ifc.name, ifc.mtu,
           ifc.hwaddr[0], ifc.hwaddr[1], ifc.hwaddr[2],ifc.hwaddr[3], ifc.hwaddr[4], ifc.hwaddr[5]);
*/
    return 0;
 }

/*************************************************************************************************************/


void fill_info(char *sendbuf, struct sockaddr_ll *socket_address, int *tx_len)
{
	 struct ether_header *eh = sendbuf; //= (struct ether_header *) sendbuf;
	 unsigned char dmac[6]= {0x08, 0x00, 0x27, 0x2e, 0x73, 0x1d}; //unicast MAC address
	 //unsigned char dmac[6]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //broadcast MAC address
	
    memcpy(eh->ether_shost, ifc.hwaddr, ETH_ALEN);  
	 memcpy(eh->ether_dhost, dmac, ETH_ALEN);  
	 eh->ether_type = 0x0000;

	 *tx_len += sizeof(struct ether_header);

	 sendbuf[*tx_len] = 'h'; (*tx_len)++;
	 sendbuf[*tx_len] = 'e'; (*tx_len)++;
	 sendbuf[*tx_len] = 'y'; (*tx_len)++;

	 socket_address->sll_ifindex = ifc.indx; //ENP0S8; 
	 socket_address->sll_family=AF_PACKET;
	 socket_address->sll_halen = ETH_ALEN;
	 socket_address->sll_protocol = ETH_P_ALL;
    socket_address->sll_hatype=0;
    socket_address->sll_pkttype=0;
    memcpy(socket_address->sll_addr, dmac, 6); 
 
}


