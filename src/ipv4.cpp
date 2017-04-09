#include "ipv4.h"

void IPV4_PACKET::swap_ip(){
    unsigned char temp;
    int i;
    for(i=0;i<4;i++){
      temp = destination_address[i];
      destination_address[i] = source_address[i];
      source_address[i] = temp;
    }
}

void IPV4_PACKET::ipv4_analyze(unsigned char* data){
  version = data[0] >> 4;
  hlen = data[0] & 0x0F;
  tos = data[1];
  total_length = (unsigned int)data[2] << 8 | (unsigned int)data[3];
  identifier = (unsigned int)data[4] << 8 | (unsigned int)data[5];
  flags = (data[6] & 0xE0) >> 5;
  fragment_offset = (unsigned int)(data[6] & 0x1F) << 8 | (unsigned int)data[7];
  time_to_live = data[8];
  protocol = data[9];
  header_checksum = (unsigned int)data[10] << 8 | (unsigned int)data[11];

  source_address[0] = data[12];
  source_address[1] = data[13];
  source_address[2] = data[14];
  source_address[3] = data[15];

  destination_address[0] = data[16];
  destination_address[1] = data[17];
  destination_address[2] = data[18];
  destination_address[3] = data[19];

  packet_length = total_length;

  switch(protocol){
    case 1:
      pType = ICMP;
      icmp.icmp_analyze(data+hlen*4, total_length - hlen*4);
      break;
    case 6:
      pType = TCP;
      break;
    case 17:
      pType = UDP;
      break;
    default:
      pType = NONE;
  }

}

void IPV4_PACKET::ipv4_construction(){
  int i;

  ipv4_packet[0] = version << 4 | hlen;
  ipv4_packet[1] = tos;
  //ipv4_packet[2] = (unsigned char)(total_length >> 8);
  //ipv4_packet[3] = (unsigned char)(total_length & 0x00FF);
  ipv4_packet[4] = (unsigned char)(identifier >> 8);
  ipv4_packet[5] = (unsigned char)(identifier & 0x00FF);
  ipv4_packet[6] = flags << 5 | (unsigned char)(fragment_offset >> 8);
  ipv4_packet[7] = (unsigned char)(fragment_offset & 0x00FF);
  ipv4_packet[8] = time_to_live;
  ipv4_packet[9] = protocol;
  ipv4_packet[10] = 0x00; // checksum_h
  ipv4_packet[11] = 0x00; // checksum_l
  ipv4_packet[12] = source_address[0];
  ipv4_packet[13] = source_address[1];
  ipv4_packet[14] = source_address[2];
  ipv4_packet[15] = source_address[3];
  ipv4_packet[16] = destination_address[0];
  ipv4_packet[17] = destination_address[1];
  ipv4_packet[18] = destination_address[2];
  ipv4_packet[19] = destination_address[3];

  switch(protocol){
    case 1:
      pType = ICMP;
      icmp.icmp_contruction();
      for(i=0;i<icmp.packet_length;i++) ipv4_packet[20+i] = icmp.icmp_packet[i];
      total_length = 20 + icmp.packet_length;
      ipv4_packet[2] = (unsigned char)(total_length >> 8);
      ipv4_packet[3] = (unsigned char)(total_length & 0x00FF);
      break;
  }

  // checksum
  unsigned int temp_checksum = 0;
  unsigned int temp_16bit;
  for(i=0;i<10;i++){
    temp_16bit = (unsigned int)ipv4_packet[2*i] << 8 | (unsigned int)ipv4_packet[2*i+1];
    if(0xFFFF - temp_checksum < temp_16bit)
      temp_checksum += temp_16bit + 1;
    else
      temp_checksum += temp_16bit;
  }
  header_checksum = 0xFFFF - temp_checksum;
  ipv4_packet[10] = (unsigned char)(header_checksum >> 8);
  ipv4_packet[11] = (unsigned char)(header_checksum & 0x00FF);

}
