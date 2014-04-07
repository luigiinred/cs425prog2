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

    printf("*** -> Received packet of length %d \n",len);
    uint32_t thisIP = 0xAC1D0C08;

    printf("*** -> Received packet of length %d \n",len);  //print everything out

    struct sr_ethernet_hdr *ethernet_hdr = packet;
    uint16_t ether_type = ntohs(ethernet_hdr->ether_type);

    if (ether_type == 0x0806){
        printf("This is an arp packet\n");
        struct sr_arphdr *arphdr = ethernet_hdr + 1;
        uint16_t ar_op = ntohs(arphdr->ar_op);

        if (ar_op == 0x0001){
            uint32_t ar_tip = ntohl(arphdr->ar_tip);

            if(ar_tip == thisIP){
                uint8_t *new_packet = calloc(1, sizeof(packet)*len);
                memcpy(new_packet, packet, sizeof(packet)*len);
                struct sr_ethernet_hdr *new_ethernet_hdr = new_packet;
                struct sr_arphdr *new_arphdr = new_ethernet_hdr + 1;
                    //change the ethernet headers
                new_ethernet_hdr->ether_dhost[0] = ethernet_hdr->ether_shost[0];     //make the new target address the old sender address
                new_ethernet_hdr->ether_dhost[1] = ethernet_hdr->ether_shost[1];
                new_ethernet_hdr->ether_dhost[2] = ethernet_hdr->ether_shost[2];
                new_ethernet_hdr->ether_dhost[3] = ethernet_hdr->ether_shost[3];
                new_ethernet_hdr->ether_dhost[4] = ethernet_hdr->ether_shost[4];
                new_ethernet_hdr->ether_dhost[5] = ethernet_hdr->ether_shost[5];
                new_ethernet_hdr->ether_shost[0] = 0x22;                //make the new sender address my MAC address
                new_ethernet_hdr->ether_shost[1] = 0x10;
                new_ethernet_hdr->ether_shost[2] = 0xd8;
                new_ethernet_hdr->ether_shost[3] = 0x83;
                new_ethernet_hdr->ether_shost[4] = 0x54;
                new_ethernet_hdr->ether_shost[5] = 0x6c;
                    //change the arp headers
                new_arphdr->ar_tip = arphdr->ar_sip;
                new_arphdr->ar_sip = arphdr->ar_tip;
                new_arphdr->ar_op = htons(0x0002);
                new_arphdr->ar_tha[0] = arphdr->ar_sha[0];     //make the new target address the old sender address
                new_arphdr->ar_tha[1] = arphdr->ar_sha[1];
                new_arphdr->ar_tha[2] = arphdr->ar_sha[2];
                new_arphdr->ar_tha[3] = arphdr->ar_sha[3];
                new_arphdr->ar_tha[4] = arphdr->ar_sha[4];
                new_arphdr->ar_tha[5] = arphdr->ar_sha[5];
                new_arphdr->ar_sha[0] = 0x22;                //make the new sender address my MAC address
                new_arphdr->ar_sha[1] = 0x10;
                new_arphdr->ar_sha[2] = 0xd8;
                new_arphdr->ar_sha[3] = 0x83;
                new_arphdr->ar_sha[4] = 0x54;
                new_arphdr->ar_sha[5] = 0x6c;

                sr_send_packet(sr,new_packet,len,"eth0");
                free(new_packet);
            }

        } else if (ar_op == 0x0002){
            printf("now you need to update the routing table\n");

        } else;
    }


    if (ether_type == 0x0800){
        printf("This is an IP packet\n");
        struct ip *ip_packet = ethernet_hdr + 1;
    }




    printf("%s\n", interface);

}/* end sr_ForwardPacket */


/*---------------------------------------------------------------------
 * Method:
 *
 *---------------------------------------------------------------------*/