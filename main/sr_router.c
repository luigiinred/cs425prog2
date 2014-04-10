/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing. 11
 *
 **********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h> /* memset */


#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

u_short cksum(u_short *buf, int count){     //checksum algorithm
	register u_long sum = 0;
	while (count--){
		sum += *buf++;
		if (sum & 0xFFFF0000){
			/* carry occurred,
			so wrap around */
			sum &= 0xFFFF;
			sum++;
		}
	}
	return ~(sum & 0xFFFF);
}

	struct icmp_hdr{                //icmp headers not in the header files
		uint8_t    icmp_type;
		uint8_t    icmp_code;
		uint16_t   icmp_cksum;
	};

	void sr_init(struct sr_instance* sr)
	{
	/* REQUIRES */
		assert(sr);
	/* Add initialization code here! */

} /* -- sr_init -- */



/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

 void sr_handlepacket(struct sr_instance* sr,
		uint8_t * packet/* lent */,
 	unsigned int len,
		char* interface/* lent */)
 	{
	/* REQUIRES */
 		assert(sr);
 		assert(packet);
 		assert(interface);

/************************THE LINE BELOW IS TOPOLOGY SPECIFIC*****************************/
	uint32_t thisIP = 0xAC1D0C08;  //Chris's topo IP for eth0: 0xAC1D0C08; Timmy's:
/****************************************************************************************/


	char *to_interface = "eth0";
	// printf("Routing Table: %x\n",sr->routing_table->gw.s_addr);

	if (sr->topo_id == 314){  // Timmy
		thisIP = 0xAC1D09C8;
	}else if(sr->topo_id == 0){  //Chris
		thisIP = 0xAC1D0C08;
	}

	printf("*** -> Received packet of length %d \n",len);                           //print everything out

	struct sr_ethernet_hdr *ethernet_hdr = packet;                                  //interpret the packet as an ethernet header
	uint16_t ether_type = ntohs(ethernet_hdr->ether_type);

	if (ether_type == 0x0806){                                                      //This is an ARP packet
		printf("This is an arp packet\n");                                          //**************** For testing only
		struct sr_arphdr *arphdr = ethernet_hdr + 1;                                //interpret the packet as an ethernet header plus arp packet
		uint16_t ar_op = ntohs(arphdr->ar_op);                                      //change the op code from network to host byte order

		if (ar_op == 0x0001){                                                       //if opcode is 1, it is an arp request
			uint32_t ar_tip = ntohl(arphdr->ar_tip);                                //change the target ip address from network to host byte order
			//printf("%X\n", ar_tip);
			if(ar_tip == thisIP){                                                   //if arp request is for me... build and send a response
				uint8_t *new_packet = calloc(1, sizeof(packet)*len);                //create a copy of the packet
				memcpy(new_packet, packet, sizeof(packet)*len);

				struct sr_ethernet_hdr *new_ethernet_hdr = new_packet;              //and interpret the copied packet as an ethernet and arp header
				struct sr_arphdr *new_arphdr = new_ethernet_hdr + 1;

				//change the ethernet headers
				new_ethernet_hdr->ether_dhost[0] = ethernet_hdr->ether_shost[0];     //make the new target address the old sender address
				new_ethernet_hdr->ether_dhost[1] = ethernet_hdr->ether_shost[1];
				new_ethernet_hdr->ether_dhost[2] = ethernet_hdr->ether_shost[2];
				new_ethernet_hdr->ether_dhost[3] = ethernet_hdr->ether_shost[3];
				new_ethernet_hdr->ether_dhost[4] = ethernet_hdr->ether_shost[4];
				new_ethernet_hdr->ether_dhost[5] = ethernet_hdr->ether_shost[5];

				//make the new sender address my MAC address
				if (sr->topo_id == 314){ // Timmy

					to_interface = "eth0";
					new_ethernet_hdr->ether_shost[0] = 0x32;
					new_ethernet_hdr->ether_shost[1] = 0x4e;
					new_ethernet_hdr->ether_shost[2] = 0xf1;
					new_ethernet_hdr->ether_shost[3] = 0xe4;
					new_ethernet_hdr->ether_shost[4] = 0xf1;
					new_ethernet_hdr->ether_shost[5] = 0x0d;

					to_interface = "eth1";
					new_ethernet_hdr->ether_shost[0] = 0x42;
					new_ethernet_hdr->ether_shost[1] = 0x70;
					new_ethernet_hdr->ether_shost[2] = 0xcd;
					new_ethernet_hdr->ether_shost[3] = 0x52;
					new_ethernet_hdr->ether_shost[4] = 0x29;
					new_ethernet_hdr->ether_shost[5] = 0x69;

					to_interface = "eth2";
					new_ethernet_hdr->ether_shost[0] = 0x92;
					new_ethernet_hdr->ether_shost[1] = 0xa3;
					new_ethernet_hdr->ether_shost[2] = 0x5e;
					new_ethernet_hdr->ether_shost[3] = 0xe7;
					new_ethernet_hdr->ether_shost[4] = 0xd9;
					new_ethernet_hdr->ether_shost[5] = 0x79;


				}else if(sr->topo_id == 0){  //Chris's MAC address topology for eth0: 22.10.d8.83.54.6c
					new_ethernet_hdr->ether_shost[0] = 0x22;
					new_ethernet_hdr->ether_shost[1] = 0x10;
					new_ethernet_hdr->ether_shost[2] = 0xd8;
					new_ethernet_hdr->ether_shost[3] = 0x83;
					new_ethernet_hdr->ether_shost[4] = 0x54;
					new_ethernet_hdr->ether_shost[5] = 0x6c;
				}


				//change the arp headers
				new_arphdr->ar_tip = arphdr->ar_sip;                                //flip flop the old IP addresses and assign them to the new
				new_arphdr->ar_sip = arphdr->ar_tip;
				new_arphdr->ar_op = htons(0x0002);                                  //set the opcode to 2, for reply
				new_arphdr->ar_tha[0] = arphdr->ar_sha[0];                          //make the new target address the old sender address
				new_arphdr->ar_tha[1] = arphdr->ar_sha[1];
				new_arphdr->ar_tha[2] = arphdr->ar_sha[2];
				new_arphdr->ar_tha[3] = arphdr->ar_sha[3];
				new_arphdr->ar_tha[4] = arphdr->ar_sha[4];
				new_arphdr->ar_tha[5] = arphdr->ar_sha[5];

				//make the new sender address my MAC address
				if (sr->topo_id == 314){  // Timmy

					to_interface = "eth0";
					new_arphdr->ar_sha[0] = 0x32;
					new_arphdr->ar_sha[1] = 0x4e;
					new_arphdr->ar_sha[2] = 0xf1;
					new_arphdr->ar_sha[3] = 0xe4;
					new_arphdr->ar_sha[4] = 0xf1;
					new_arphdr->ar_sha[5] = 0x0d;

					to_interface = "eth1";
					new_arphdr->ar_sha[0] = 0x22;
					new_arphdr->ar_sha[1] = 0x10;
					new_arphdr->ar_sha[2] = 0xd8;
					new_arphdr->ar_sha[3] = 0x83;
					new_arphdr->ar_sha[4] = 0x54;
					new_arphdr->ar_sha[5] = 0x6c;

					to_interface = "eth2";
					new_arphdr->ar_sha[0] = 0x92;
					new_arphdr->ar_sha[1] = 0xa3;
					new_arphdr->ar_sha[2] = 0x5e;
					new_arphdr->ar_sha[3] = 0xe7;
					new_arphdr->ar_sha[4] = 0xd9;
					new_arphdr->ar_sha[5] = 0x79;

				}else if(sr->topo_id == 0){  //Chris's MAC address topology for eth0: 22.10.d8.83.54.6c
					new_arphdr->ar_sha[0] = 0x22;
					new_arphdr->ar_sha[1] = 0x10;
					new_arphdr->ar_sha[2] = 0xd8;
					new_arphdr->ar_sha[3] = 0x83;
					new_arphdr->ar_sha[4] = 0x54;
					new_arphdr->ar_sha[5] = 0x6c;
				}


				sr_send_packet(sr,new_packet,len,to_interface);                           //send the response
				printf("package Sent to %s\n",to_interface);
				free(new_packet);                                                   //free the allocated memory
			}

		} else if (ar_op == 0x0002){                                                //if the ARP op code is 2, a reply...
			printf("now you need to update the routing table\n");

		} else;
	}

	if (ether_type == 0x0800){                                                      //This is an IP packet
		printf("This is an IP packet\n");                                           //**************** For testing only

		uint8_t *new_packet = calloc(1, sizeof(packet)*len);                        //have to make a new packet so that I can set checksum to zero before calculating it
		memcpy(new_packet, packet, sizeof(packet)*len);
		struct sr_ethernet_hdr *new_ethernet_hdr = new_packet;                      //interpret this copied packet as an ethernet header
		struct ip *new_ip_packet = new_ethernet_hdr + 1;                            //and an IP packet

		uint16_t givenChecksum = new_ip_packet->ip_sum;                             //store the provided checksum
		new_ip_packet->ip_sum = 0x0000;                                             //set the checksum field to zero, then calculate the checksum for verification
		uint16_t calculatedChecksum = cksum((uint16_t *) new_ip_packet, new_ip_packet->ip_hl*2);  //the count is header length * 2 because the header length is given in 32 bits, where the count is in 16 bits

		if (givenChecksum != calculatedChecksum){                                   //if the checksum is invalid
			printf("IP Checksum is invalid\n");                                     //**************** For testing only
			return;                                                                 //drop the packet

		}else if(new_ip_packet->ip_p == 1 && ntohl(new_ip_packet->ip_dst.s_addr) == thisIP){ //if the ip packet is an ICMP and it's meant for my IP address...
				printf("This is an ICMP packet for me\n");                          //**************** For testing only
				struct icmp_hdr *icmpHeader = new_ip_packet + 1;                    //interpret it as an ICMP header

				if(icmpHeader->icmp_type == 8){                                     //This is an ICMP request
					printf("This is an icmp request\n");                            //**************** For testing only

					uint16_t gChecksum = icmpHeader->icmp_cksum;                    //store the given ICMP checksum
					icmpHeader->icmp_cksum = 0x0000;                                //set the ICMP checksum field to zero, then calculate the checksum for verification
					uint16_t cChecksum = cksum((uint16_t *) icmpHeader, 32);        //I determined the count is 32 by logging the wire traffic and counting the total number of bytes for an ICMP message, then dividing by 2

					if(gChecksum != cChecksum){                                     //if the checksum is invalid
						printf("The icmp checksum is invalid\n");                   //**************** For testing only
						return;                                                     //drop the packet

					}else{                                                          //otherwise, send a response
						//change ICMP headers
						icmpHeader->icmp_type = 0;                                  //set type to 0 for reply
						uint16_t newChecksum1 = cksum((uint16_t *) icmpHeader, 32); //calculate new checksum of ICMP packet
						icmpHeader->icmp_cksum = newChecksum1;                      //set new checksum
						//change the ip headers
						new_ip_packet->ip_dst.s_addr = new_ip_packet->ip_src.s_addr;//switch the sender/receiver IP address fields
						new_ip_packet->ip_src.s_addr = htonl(thisIP);
						new_ip_packet->ip_ttl = 0x04;                               //this is 64 in hex, but switched from host to network byte order. 64 was recommended by the FAQ
						uint16_t newChecksum2 = cksum((uint16_t *) new_ip_packet, new_ip_packet->ip_hl*2); //calculate new checksum of IP packet
						new_ip_packet->ip_sum = newChecksum2;                       //set the new checksum
						//change the ethernet headers
						new_ethernet_hdr->ether_dhost[0] = ethernet_hdr->ether_shost[0]; //make the new target address the old sender address
						new_ethernet_hdr->ether_dhost[1] = ethernet_hdr->ether_shost[1];
						new_ethernet_hdr->ether_dhost[2] = ethernet_hdr->ether_shost[2];
						new_ethernet_hdr->ether_dhost[3] = ethernet_hdr->ether_shost[3];
						new_ethernet_hdr->ether_dhost[4] = ethernet_hdr->ether_shost[4];
						new_ethernet_hdr->ether_dhost[5] = ethernet_hdr->ether_shost[5];

						if (sr->topo_id == 314){  // Timmy
							new_ethernet_hdr->ether_shost[0] = 0x32;
							new_ethernet_hdr->ether_shost[1] = 0x4e;
							new_ethernet_hdr->ether_shost[2] = 0xf1;
							new_ethernet_hdr->ether_shost[3] = 0xe4;
							new_ethernet_hdr->ether_shost[4] = 0xf1;
							new_ethernet_hdr->ether_shost[5] = 0x0d;
						}else if(sr->topo_id == 0){  //Chris's MAC address topology for eth0: 22.10.d8.83.54.6c
							new_ethernet_hdr->ether_shost[0] = 0x22;
							new_ethernet_hdr->ether_shost[1] = 0x10;
							new_ethernet_hdr->ether_shost[2] = 0xd8;
							new_ethernet_hdr->ether_shost[3] = 0x83;
							new_ethernet_hdr->ether_shost[4] = 0x54;
							new_ethernet_hdr->ether_shost[5] = 0x6c;
						}

						sr_send_packet(sr,new_packet,len,"eth0");                   //send the ICMP echo reply

					}
				}
			}else{
			uint8_t ttl = new_ip_packet->ip_ttl - 1;                                //decrement ttl by 1
			uint16_t recalculatedChecksum = cksum((uint16_t *) new_ip_packet, new_ip_packet->ip_hl*2);  //recalculate checksum
			new_ip_packet->ip_sum = recalculatedChecksum;                           //set the new checksum
			printf("ip address: %X\n", new_ip_packet->ip_dst.s_addr);
			//if the ip address is in our cache, then forward the packet to the appropriate router
			//where in the world do we store a cache???
			//if the ip address is not in our cache, send out an ARP request for this IP
		}
		free(new_packet);                                                           //free the allocated memory
	}








/********I used the stuff below to make IP's readable. I'll leave them here until they're no longer needed*****************/

	// unsigned int srcInt = ip_packet->ip_src.s_addr;
	// unsigned int dstInt = ip_packet->ip_dst.s_addr;   // use network to host long to interpret the destination IP

	// struct readableIP{                       //this struct will parse an IP address
	//     unsigned int byte1: 8;
	//     unsigned int byte2: 8;
	//     unsigned int byte3: 8;
	//     unsigned int byte4: 8;
	//  };
	// union readTheIP{                        //this union will combine the IP address integer and the struct
	//     unsigned int joinHere;
	//     struct readableIP legibleRepresentation;
	// };

	// union readTheIP makeIpReadable1;        //create an instance of each union
	// makeIpReadable1.joinHere = dstInt;      // and join it with the appropriate integer or pointer
	// union readTheIP makeIpReadable2;
	// makeIpReadable2.joinHere = srcInt;
	// union readTheIP makeIpReadable3;
	// makeIpReadable3.joinHere = arphdr->ar_sip;
	// union readTheIP makeIpReadable4;
	// makeIpReadable4.joinHere = arphdr->ar_tip;


	// printf("srcMAC: %X:%X:%X:%X:%X:%X\n", makeMACReadable2.legibleRepresentation.byte1, makeMACReadable2.legibleRepresentation.byte2, makeMACReadable2.legibleRepresentation.byte3, makeMACReadable2.legibleRepresentation.byte4, makeMACReadable2.legibleRepresentation.byte5, makeMACReadable2.legibleRepresentation.byte6);
	// printf("dstMAC: %X:%X:%X:%X:%X:%X\n", makeMACReadable1.legibleRepresentation.byte1, makeMACReadable1.legibleRepresentation.byte2, makeMACReadable1.legibleRepresentation.byte3, makeMACReadable1.legibleRepresentation.byte4, makeMACReadable1.legibleRepresentation.byte5, makeMACReadable1.legibleRepresentation.byte6);

	// printf("ar_sip: %X\n", arphdr->ar_sip);
	// printf("ar_tip: %X\n", arphdr->ar_tip);

	// printf("ip_src: %d.%d.%d.%d\n", makeIpReadable2.legibleRepresentation.byte1, makeIpReadable2.legibleRepresentation.byte2, makeIpReadable2.legibleRepresentation.byte3, makeIpReadable2.legibleRepresentation.byte4);
	// printf("ip_dst: %d.%d.%d.%d\n", makeIpReadable1.legibleRepresentation.byte1, makeIpReadable1.legibleRepresentation.byte2, makeIpReadable1.legibleRepresentation.byte3, makeIpReadable1.legibleRepresentation.byte4);
	// printf("ar_sip: %d.%d.%d.%d\n", makeIpReadable3.legibleRepresentation.byte1, makeIpReadable3.legibleRepresentation.byte2, makeIpReadable3.legibleRepresentation.byte3, makeIpReadable3.legibleRepresentation.byte4);
	// printf("ar_tip: %d.%d.%d.%d\n", makeIpReadable4.legibleRepresentation.byte1, makeIpReadable4.legibleRepresentation.byte2, makeIpReadable4.legibleRepresentation.byte3, makeIpReadable4.legibleRepresentation.byte4);


}     /* end sr_ForwardPacket */