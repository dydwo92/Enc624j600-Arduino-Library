#ifndef __ICMP_H__
#define __ICMP_H__

class ICMP_PACKET{
  public:
    unsigned char type;
    unsigned char code;
    unsigned int checksum;
    unsigned int identifier;
    unsigned int sequence_number;
    unsigned char payload[50];

    unsigned char icmp_packet[50];
    unsigned int packet_length;

    void icmp_analyze(unsigned char* data, unsigned int total_length);
    void icmp_contruction();
};

#endif
