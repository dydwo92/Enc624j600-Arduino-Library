#include "ethercard.h"
#include "packet.h"

int A_PIN[] = {25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39}; // PA3, PA4, PA5, PA6, PA7, PC7, PC6, PC5, PC4, PC3, PC2, PC1, PC0, PD7, PG2
int AD_PIN[] = {41, 40, 43, 42, 45, 44, 47, 46}; // PG0, PG1, PL6, PL7, PL4, PL5, PL2, PL3
int CS_PIN = 22; int RD_PIN = 23; int WR_PIN = 24;

Ethercard ec;
Packet receivePacket;

unsigned char gateway_ip[4] = {192, 168, 9, 1};
unsigned char mac_addr[6];
int i;
void setup(){
  Serial.begin(9600);
  if(!ec.insert_ethercard(AD_PIN, A_PIN, CS_PIN, RD_PIN, WR_PIN)){
    Serial.println("Ethernet Card Insert Fail!");
    while(1){}
  }

  ec.begin(gateway_ip);
  ec.get_mac_address(mac_addr);
}

void loop(){

  if(ec.get_macphy_state() & ec.PACKET_RECEIVED){
    
    ec.get_packet(&receivePacket);

    // Set static ip
    if(receivePacket.eType == receivePacket.ARP){
      if(receivePacket.arp.destination_ip_is(gateway_ip) && receivePacket.arp.op_code == 1){
        receivePacket.swap_mac();
        receivePacket.arp.swap_ip();
        receivePacket.arp.swap_mac();
        receivePacket.arp.op_code = 2;
        receivePacket.arp.set_source_mac(mac_addr);
        receivePacket.packet_construction();
        
        ec.send_packet(&receivePacket);
      }
    }else if(receivePacket.eType == receivePacket.IPv4){
      if(receivePacket.ipv4.pType == receivePacket.ipv4.ICMP){
        // ping
        if(receivePacket.ipv4.icmp.type == 8){
          Serial.println(receivePacket.ipv4.version, HEX);
          receivePacket.swap_mac();
          receivePacket.ipv4.swap_ip();
          receivePacket.ipv4.icmp.type = 0;
          receivePacket.packet_construction();
          
          ec.send_packet(&receivePacket);
        }
      }
    }

  }
  

}



