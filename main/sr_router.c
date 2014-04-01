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
#include <inttypes.h> // Needed to print uint8_t

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
    struct ip *new_packet = calloc(1, sizeof(struct ip));
     new_packet = (struct ip*) packet;

    printf("*** -> Received packet of length %d \n",len);
    printf("ip_tos: %" PRIu8 "\n", new_packet->ip_tos);

    printf("ip_src: %" PRIu8 "\n", new_packet->ip_src.s_addr);
    printf("ip_dst: %" PRIu8 "\n", new_packet->ip_dst.s_addr);

    printf("%s\n", interface);
    printf("%" PRIu8 "\n", sr->sr_addr.sin_addr.s_addr);


}/* end sr_ForwardPacket */


/*---------------------------------------------------------------------
 * Method:
 *
 *---------------------------------------------------------------------*/
