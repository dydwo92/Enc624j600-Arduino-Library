#include "packet.h"

void Packet::set_destination_mac(unsigned char mac_addr[6]){
  int i;
  for(i=0;i<6;i++) destination_MAC[i] = mac_addr[i];
}

void Packet::set_source_mac(unsigned char mac_addr[6]){
  int i;
  for(i=0;i<6;i++) source_MAC[i] = mac_addr[i];
}

void Packet::swap_mac(){
  unsigned char temp;
  int i;
  for(i=0;i<6;i++){
    temp = destination_MAC[i];
    destination_MAC[i] = source_MAC[i];
    source_MAC[i] = temp;
  }
}

void Packet::packet_analyze(){
  int i;
  unsigned int etype;
  for(i=0;i<6;i++) destination_MAC[i] = packet[i];
  for(i=0;i<6;i++) source_MAC[i] = packet[i+6];
  etype = (unsigned int)packet[12] << 8 | (unsigned int)packet[13];

  switch(etype){
    case 0x0806 :
      eType = ARP;
      arp.arp_analyze(packet+14);
      break;
    case 0x0835 :
      eType = RARP;
      break;
    case 0x8138 :
      eType = IPX;
      break;
    case 0x0800 :
      eType = IPv4;
      ipv4.ipv4_analyze(packet+14);
      break;
    default :
      eType = NONE;
  }
}

void Packet::packet_construction(){
  int i;

  // 1. Set Destination & Source MAC
  for(i=0;i<6;i++){
    packet[i] = destination_MAC[i];
    packet[6+i] = source_MAC[i];
  }

  // 2. Set EType
  switch(eType){
      case ARP:
        packet[12] = 0x08; packet[13] = 0x06;
        break;
      case RARP:
        packet[12] = 0x08; packet[13] = 0x35;
        break;
      case IPX:
        packet[12] = 0x81; packet[13] = 0x38;
        break;
      case IPv4:
        packet[12] = 0x08; packet[13] = 0x00;
        break;
  }

  // 3. Copy sub packet
  switch(eType){
    case ARP:
      arp.arp_construction();
      for(i=0;i<arp.packet_length;i++) packet[14+i] = arp.arp_packet[i];
      packet_length = 14 + arp.packet_length;
      break;
    case IPv4:
      ipv4.ipv4_construction();
      for(i=0;i<ipv4.packet_length;i++) packet[14+i] = ipv4.ipv4_packet[i];
      packet_length = 14 + ipv4.packet_length;
      break;
  }

}
