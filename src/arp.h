#ifndef __ARP_H__
#define __ARP_H__

class ARP_PACKET{
  public:
    unsigned int hardware_type;
    unsigned int protocol_type;
    unsigned char hard_add_len;
    unsigned char proto_add_len;
    unsigned int op_code;
    unsigned char source_MAC[6];
    unsigned char source_IP[4];
    unsigned char destination_MAC[6];
    unsigned char destination_IP[4];

    unsigned char arp_packet[42];
    unsigned int packet_length;

    void swap_ip();
    void swap_mac();

    void set_destination_mac(unsigned char mac_addr[6]);
    void set_destination_ip(unsigned char ip[4]);
    void set_source_mac(unsigned char mac_addr[6]);
    void set_source_ip(unsigned char ip[4]);

    void arp_analyze(unsigned char* data);
    void arp_construction();

    bool destination_ip_is(unsigned char ip[4]);
    bool source_ip_is(unsigned char ip[4]);

};

#endif
