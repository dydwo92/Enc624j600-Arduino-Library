#include "icmp.h"

void ICMP_PACKET::icmp_analyze(unsigned char* data, unsigned int total_length){
  unsigned int i;
  type = data[0];
  code = data[1];
  checksum = (unsigned int)data[2] << 8 | (unsigned int)data[3];
  identifier = (unsigned int)data[4] << 8 | (unsigned int)data[5];
  sequence_number = (unsigned int)data[6] << 8 | (unsigned int)data[7];

  for(i=0;i<total_length - 8;i++){
    payload[i] = data[8+i];
  }

  packet_length = total_length;
}

void ICMP_PACKET::icmp_contruction(){
  unsigned int i;
  icmp_packet[0] = type;
  icmp_packet[1] = code;
  icmp_packet[2] = 0x00;
  icmp_packet[3] = 0x00;
  icmp_packet[4] = (unsigned char)(identifier >> 8);
  icmp_packet[5] = (unsigned char)(identifier & 0x00FF);
  icmp_packet[6] = (unsigned char)(sequence_number >> 8);
  icmp_packet[7] = (unsigned char)(sequence_number & 0x00FF);

  for(i=0;i<packet_length - 8;i++){
    icmp_packet[8+i] = payload[i];
  }

  // checksum
  unsigned int temp_checksum = 0;
  unsigned int temp_16bit;
  for(i=0;i<packet_length/2;i++){
    temp_16bit = (unsigned int)icmp_packet[2*i] << 8 | (unsigned int)icmp_packet[2*i+1];
    if(0xFFFF - temp_checksum < temp_16bit)
      temp_checksum += temp_16bit + 1;
    else
      temp_checksum += temp_16bit;
  }

  if((packet_length % 2) == 1){
    temp_16bit = (unsigned int)icmp_packet[packet_length - 1] << 8;
    if(0xFFFF - temp_checksum < temp_16bit)
      temp_checksum += temp_16bit + 1;
    else
      temp_checksum += temp_16bit;
  }

  checksum = 0xFFFF - temp_checksum;
  icmp_packet[2] = (unsigned char)(checksum >> 8);
  icmp_packet[3] = (unsigned char)(checksum & 0x00FF);

}
