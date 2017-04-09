#ifndef __PACKET_H__
#define __PACKET_H__

#include "arp.h"
#include "ipv4.h"

class Packet{
  public:
  unsigned char destination_MAC[6];
  unsigned char source_MAC[6];
  enum EType_List{ARP, RARP, IPX, IPv4, NONE} eType;

  unsigned char packet[500];
  unsigned int packet_length;

  ARP_PACKET arp;
  IPV4_PACKET ipv4;

  void set_destination_mac(unsigned char mac_addr[6]);
  void set_source_mac(unsigned char mac_addr[6]);
  void swap_mac();

  void packet_analyze();
  void packet_construction();
};

#endif
