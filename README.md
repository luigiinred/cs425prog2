cs425prog2
==========

# To Do


- [ ] Correctly handle ARP requests and responses.
	- [ ] A dynamically-allocated cache is used to store ARP responses (and failures).
	- [ ] Individual cache entries time-out (are invalidated) after 15 seconds.
	- [ ] A miss in the ARP cache causes an ARP request to be broadcast and the incoming packet queued.
	- [ ] Additional packets to the same IP address are queued while an ARP request for that IP address is pending.
	- [ ] An ARP request is transmitted up to 5 times at 1-second intervals before the ARP request is considered failed.
	- [ ] When an ARP request fails an ICMP "Host unreachable" response is sent to the sender of all packets queued for that ARP request.
- [ ] Respond correctly to ICMP echo requests
- [ ] Correctly forward packets between the gateway and the servers.
