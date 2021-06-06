#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>

#define DEST_MAC0	0x08
#define DEST_MAC1	0x00
#define DEST_MAC2	0x27
#define DEST_MAC3	0x2e
#define DEST_MAC4	0x73
#define DEST_MAC5	0x1d

#define DEFAULT_PORT	"enp0s8"
#define BUF_SIZ		1024

int main(int argc, char *argv[])
{

	int sockfd, ret, i, numbytes, sockopt;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct sockaddr_ll socket_address;
	unsigned char buf[BUF_SIZ];
	unsigned char *data_recvd=buf;
   struct ether_header *eh = buf;

	/* Open PF_PACKET socket */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
		perror("listener: socket");	
		return -1;
	}

	/* Set interface to promiscuous mode */
	strncpy(ifopts.ifr_name, DEFAULT_PORT, IFNAMSIZ-1);
	if(ioctl(sockfd, SIOCGIFFLAGS, &ifopts)<0) {perror("get flag");}
	ifopts.ifr_flags |= IFF_PROMISC;
	if(ioctl(sockfd, SIOCSIFFLAGS, &ifopts)<0) {perror("set flag");}
	
/* Allow the socket to be reused - incase connection is closed prematurely */
/*
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
*/

	printf("Waiting to recv.............!!!!!\n");
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
	
	/* Check the packet is for me */
	if (eh->ether_dhost[0] == DEST_MAC0 &&
			eh->ether_dhost[1] == DEST_MAC1 &&
			eh->ether_dhost[2] == DEST_MAC2 &&
			eh->ether_dhost[3] == DEST_MAC3 &&
			eh->ether_dhost[4] == DEST_MAC4 &&
			eh->ether_dhost[5] == DEST_MAC5) 
	{
		printf("Correct destination MAC address: %02X:%02X:%02X:%02x:%02x:%02x\n", eh->ether_dhost[0],eh->ether_dhost[1], 
		                                     eh->ether_dhost[2],eh->ether_dhost[3],eh->ether_dhost[4],eh->ether_dhost[5]);                                
	} 
	
	printf("got frame of %d bytes from MAC: %02X:%02X:%02X:%02x:%02x:%02x\n", numbytes, eh->ether_shost[0],eh->ether_shost[1],
	                                           eh->ether_shost[2], eh->ether_shost[3],eh->ether_shost[4],eh->ether_shost[5]);	

	data_recvd=data_recvd+14; // as ethernet frame header is 14bytes dmac+smac+type
	printf("data received=%s\n", data_recvd);
	close(sockfd);
	return 0;
}









