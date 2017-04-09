#ifndef __ETHERCARD_H__
#define __ETHERCARD_H__

#include "enc624j600.h"
#include "packet.h"

class Ethercard{
  public:
    bool insert_ethercard(int ad_pin[8], int a_pin[15], int cs_pin, int rd_pin, int wr_pin);

    void begin(unsigned char ip[4]);
    void begin(unsigned char ip[4], unsigned char mac[6]);

    void set_ip(unsigned char ip[4]);
    void set_mac_address(unsigned char mac_addr[6]);
    void get_mac_address(unsigned char mac_addr[6]);

    void send_packet(Packet* packet);
    void get_packet(Packet* packet);

    const unsigned int PACKET_RECEIVED = 0x0040;
    const unsigned int PACKET_TRANSMITTED = 0x0008;

    enum ecState{
      NONE,
      // ARP
      ARP_REQUEST, ARP_REPLY
    };

    unsigned int get_macphy_state();
    ecState get_ethercard_state();

    Ethercard(){
        _isInserted = false;
    }

  private:
    bool _isInserted;
    unsigned char _IP[4];
    unsigned char _MAC_ADDR[6];
    Enc624j600 macphy;
};

#endif

