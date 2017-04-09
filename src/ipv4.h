#ifndef __IPV4_H__
#define __IPV4_H__
#include "icmp.h"

class IPV4_PACKET{
  public:
    unsigned char version;
    unsigned char hlen;
    unsigned char tos;
    unsigned int total_length;
    unsigned int identifier;
    unsigned char flags;
    unsigned int fragment_offset;
    unsigned char time_to_live;
    unsigned char protocol;
    unsigned int header_checksum;
    unsigned char source_address[4];
    unsigned char destination_address[4];

    enum Protocol_List {ICMP, TCP, UDP, NONE} pType;
    unsigned char ipv4_packet[100];
    unsigned int packet_length;

    ICMP_PACKET icmp;

    void swap_ip();

    void ipv4_analyze(unsigned char* data);
    void ipv4_construction();
};

#endif
