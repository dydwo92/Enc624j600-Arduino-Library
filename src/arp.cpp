#include "arp.h"

void ARP_PACKET::swap_ip(){
  unsigned char temp;
  int i;
  for(i=0;i<4;i++){
    temp = destination_IP[i];
    destination_IP[i] = source_IP[i];
    source_IP[i] = temp;
  }
}

void ARP_PACKET::swap_mac(){
  unsigned char temp;
  int i;
  for(i=0;i<6;i++){
    temp = destination_MAC[i];
    destination_MAC[i] = source_MAC[i];
    source_MAC[i] = temp;
  }
}

void ARP_PACKET::set_destination_mac(unsigned char mac_addr[6]){
  int i;
  for(i=0;i<6;i++) destination_MAC[i] = mac_addr[i];
}

void ARP_PACKET::set_destination_ip(unsigned char ip[4]){
  int i;
  for(i=0;i<4;i++) destination_IP[i] = ip[i];
}

void ARP_PACKET::set_source_mac(unsigned char mac_addr[6]){
  int i;
  for(i=0;i<6;i++) source_MAC[i] = mac_addr[i];
}

void ARP_PACKET::set_source_ip(unsigned char ip[4]){
  int i;
  for(i=0;i<4;i++) source_IP[i] = ip[i];
}

void ARP_PACKET::arp_analyze(unsigned char* data){
  unsigned char i;
  hardware_type = (unsigned int)data[0] << 8 | (unsigned int)data[1];
  protocol_type = (unsigned int)data[2] << 8 | (unsigned int)data[3];
  hard_add_len = data[4];
  proto_add_len = data[5];
  op_code = (unsigned int)data[6] | (unsigned int)data[7];

  for(i=0;i<hard_add_len;i++){
    source_MAC[i] = data[8+i];
    destination_MAC[i] = data[8+hard_add_len+proto_add_len+i];
  }

  for(i=0;i<proto_add_len;i++){
    source_IP[i] = data[8+hard_add_len+i];
    destination_IP[i] = data[8+2*hard_add_len+proto_add_len+i];
  }

}

void ARP_PACKET::arp_construction(){
  int i;

  // 1. Hardware type(2)
  arp_packet[0] = (unsigned char)(hardware_type >> 8);
  arp_packet[1] = (unsigned char)(hardware_type & 0x00FF);

  // 2. Protocol type(2)
  arp_packet[2] = (unsigned char)(protocol_type >> 8);
  arp_packet[3] = (unsigned char)(protocol_type & 0x00FF);

  // 3. Mac, IP length(2)
  arp_packet[4] = hard_add_len;
  arp_packet[5] = proto_add_len;

  // 4. ARP operation
  arp_packet[6] = (unsigned char)(op_code >> 8);
  arp_packet[7] = (unsigned char)(op_code & 0x00FF);

  // 5. Sender & Target MAC Address
  for(i=0;i<hard_add_len;i++){
      arp_packet[8+i] = source_MAC[i];
      arp_packet[8+hard_add_len+proto_add_len+i] = destination_MAC[i];
  }

  // 6. Sender & Target IP Address
  for(i=0;i<proto_add_len;i++){
    arp_packet[8+hard_add_len+i] = source_IP[i];
    arp_packet[8+2*hard_add_len+proto_add_len+i] = destination_IP[i];
  }

  packet_length = 8 + 2*hard_add_len + 2*proto_add_len;
}

bool ARP_PACKET::destination_ip_is(unsigned char ip[4]){
  if(destination_IP[0] == ip[0] &&
     destination_IP[1] == ip[1] &&
     destination_IP[2] == ip[2] &&
     destination_IP[3] == ip[3])
     return true;
  else
     return false;
}

bool ARP_PACKET::source_ip_is(unsigned char ip[4]){
  if(source_IP[0] == ip[0] &&
     source_IP[1] == ip[1] &&
     source_IP[2] == ip[2] &&
     source_IP[3] == ip[3])
     return true;
  else
     return false;
}
