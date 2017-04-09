#include "ethercard.h"

bool Ethercard::insert_ethercard(int ad_pin[8], int a_pin[15], int cs_pin, int rd_pin, int wr_pin){
  macphy.enc624_set_pinmap(ad_pin, a_pin, cs_pin, rd_pin, wr_pin);
  _isInserted = macphy.enc624_reset();
  return _isInserted;
}

void Ethercard::begin(unsigned char ip[4]){
  if(_isInserted){
    set_ip(ip);
    macphy.enc624_packet_read_enable();
    macphy.enc624_get_mac_address(_MAC_ADDR);
  }
}

void Ethercard::begin(unsigned char ip[4], unsigned char mac[6]){
  if(_isInserted){
    set_ip(ip);
    macphy.enc624_packet_read_enable();
    macphy.enc624_set_mac_address(_MAC_ADDR);
  }
}

void Ethercard::set_ip(unsigned char ip[4]){
  _IP[0] = ip[0];
  _IP[1] = ip[1];
  _IP[2] = ip[2];
  _IP[3] = ip[3];
}

void Ethercard::get_mac_address(unsigned char mac_addr[6]){
  macphy.enc624_get_mac_address(mac_addr);
}

void Ethercard::set_mac_address(unsigned char mac_addr[6]){
  _MAC_ADDR[0] = mac_addr[0];
  _MAC_ADDR[1] = mac_addr[1];
  _MAC_ADDR[2] = mac_addr[2];
  _MAC_ADDR[3] = mac_addr[3];
  _MAC_ADDR[4] = mac_addr[4];
  _MAC_ADDR[5] = mac_addr[5];

  macphy.enc624_set_mac_address(mac_addr);
}

void Ethercard::send_packet(Packet* packet){
  macphy.enc624_send_packet(packet);
}

void Ethercard::get_packet(Packet* packet){
  macphy.enc624_get_packet(packet);
}

unsigned int Ethercard::get_macphy_state(){
  return macphy.enc624_get_interrupt_flag();
}

Ethercard::ecState Ethercard::get_ethercard_state(){
  return NONE;
}

