#include "enc624j600.h"

void Enc624j600::enc624_set_pinmap(int ad_pin[8], int a_pin[15], int cs_pin, int rd_pin, int wr_pin){
  int i;
  for(i=0;i<8;i++) _ADPIN[i] = ad_pin[i];
  for(i=0;i<15;i++) _APIN[i] = a_pin[i];
  _CSPIN = cs_pin;
  _RDPIN = rd_pin;
  _WRPIN = wr_pin;

  _pinSet = true;
  enc624_init_port();
}

void Enc624j600::enc624_init_port(){
  int i;
  if(_pinSet){
    // AD<7:0> : Bi-Direction, ELSE : OUTPUT
    for(i=0;i<8;i++) pinMode(_ADPIN[i], INPUT);
    for(i=0;i<15;i++) pinMode(_APIN[i], OUTPUT);
    pinMode(_CSPIN, OUTPUT);
    pinMode(_RDPIN, OUTPUT);
    pinMode(_WRPIN, OUTPUT);
  }
}

void Enc624j600::enc624_set_ad_pin_input(){
  int i;
  if(_pinSet){
    for(i=0;i<8;i++) pinMode(_ADPIN[i], INPUT);
  }
}

void Enc624j600::enc624_set_ad_pin_output(){
  int i;
  if(_pinSet){
    for(i=0;i<8;i++) pinMode(_ADPIN[i], OUTPUT);
  }
}

void Enc624j600::enc624_write(unsigned int address, unsigned char data){
  int i;
  // 1. AD<7:0> : Hi-Z(input)
  enc624_set_ad_pin_input();

  // 2. CS : high, A<14:0> : active, RD :low, WR : low, AD<7:0> : active
  digitalWrite(_CSPIN, LOW);
  digitalWrite(_RDPIN, LOW);
  digitalWrite(_WRPIN, LOW);
  for(i=0;i<15;i++){
    digitalWrite(_APIN[i], (address & (0x01 << i)) == 0 ? LOW : HIGH);
  }
  enc624_set_ad_pin_output();
  for(i=0;i<8;i++){
    digitalWrite(_ADPIN[i], (data & (0x01 << i)) == 0? LOW : HIGH);
  }
  digitalWrite(_CSPIN, HIGH);

  // 3. waiting... (p149. table 17-10.)
  NOP;

  // 4. Writing data
  digitalWrite(_WRPIN, HIGH);
  NOP;
  digitalWrite(_WRPIN, LOW);
  NOP;

  // 5. Release
  enc624_set_ad_pin_input();
  digitalWrite(_CSPIN, LOW);
}

// p56. Figure 5-2.
unsigned char Enc624j600::enc624_read(unsigned int address){
  unsigned char data = 0x00;
  int i;
  // 1. AD<7:0> : Hi-Z(input)
  enc624_set_ad_pin_input();

  // 2. CS : high, A<14:0> : active, RD :low, WR : low
  digitalWrite(_CSPIN, LOW);
  digitalWrite(_RDPIN, LOW);
  digitalWrite(_WRPIN, LOW);
  for(i=0;i<15;i++){
    digitalWrite(_APIN[i], (address & (0x01 << i)) == 0 ? LOW : HIGH);
  }
  digitalWrite(_CSPIN, HIGH);

  // 3. waiting... (p149. table 17-10.)
  NOP; // 1 cycle delay

  // 4. Read Data
  digitalWrite(_RDPIN, HIGH);
  NOP; NOP;  // 2cycle delay
  for(i=0;i<8;i++){
    data |= (digitalRead(_ADPIN[i]) == HIGH ? 0x01 : 0x00) << i;
  }
  digitalWrite(_RDPIN, LOW);
  NOP;
  digitalWrite(_CSPIN, LOW);

  enc624_set_ad_pin_input();

  return data;
}

bool Enc624j600::enc624_reset(){
 unsigned char temp;
 unsigned int EUDAST = 0;
 int timeOut = 0;
 bool state = false;

 // Reset (p77. 8.1)
 // 1. Write 1234h to EUDAST(7E17 | 7E16)
 enc624_write(0x7E17, 0x12);
 enc624_write(0x7E16, 0x34);

 // 2. Read EUDAST to see if it now equals 1234h
 timeOut = 0;
 do{
   EUDAST = (unsigned int)enc624_read(0x7E17);
   EUDAST = (EUDAST<<8) | (unsigned int)enc624_read(0x7E16);
   if(++timeOut >= 100) return false;
 }while(EUDAST != 0x1234);

 // 3. Poll CLKRDY(ESTAT<12> : 7E7B) and wiat for it to become set
 timeOut = 0;
 do{
   temp = enc624_read(0x7E7B);
   if(++timeOut >= 100) return false;
 }while((temp & 0x10) == 0);

 // 4. Issue a System Reset command by setting ETHRST(ECON2<4> : 7F6E)
 enc624_write(0x7F6E,0x10);
 NOP;

 EUDAST = (unsigned int)enc624_read(0x7E17);
 EUDAST = (EUDAST<<8) | (unsigned int)enc624_read(0x7E16);
 if(EUDAST == 0x00) state = true;
 else state = false;

 // 5. Wait at lest 256us
 NOP; NOP; NOP; NOP; NOP; // delay 5 cycles;

 if(!state) return false;

 // 6. Packet read enable (p.89. 9.2.1)
 //enc624_packet_read_enable();

 return state;
}

void Enc624j600::enc624_get_mac_address(unsigned char mac[6]){
  // p 78. 8.6.1.
  mac[5] = enc624_read(0x7E61);
  mac[4] = enc624_read(0x7E60);
  mac[3] = enc624_read(0x7E63);
  mac[2] = enc624_read(0x7E62);
  mac[1] = enc624_read(0x7E65);
  mac[0] = enc624_read(0x7E64);
}

void Enc624j600::enc624_set_mac_address(unsigned char mac[6]){
  // p 78. 8.6.1.
  enc624_write(0x7E61, mac[5]);
  enc624_write(0x7E60, mac[4]);
  enc624_write(0x7E63, mac[3]);
  enc624_write(0x7E62, mac[2]);
  enc624_write(0x7E65, mac[1]);
  enc624_write(0x7E64, mac[0]);
}

void Enc624j600::enc624_packet_read_enable(){
  // p.89. 9.2.1
  // 1. Read _ERXST
  _ERXST = (unsigned int)enc624_read(0x7E05) << 8 | (unsigned int)enc624_read(0x7E04);
  _NextPacketPointer = _ERXST;
  _ERXTAIL = 0x5FFE;

  // Enable RXEN
  enc624_write(0x7E1E, 0x01);
}

void Enc624j600::enc624_get_packet(Packet* received_packet){
  unsigned char RSV[6];
  unsigned char temp;
  int i;
  // p. 89. 9.2.2
  // 1. get_ERXTAIL
  _ERXTAIL = (unsigned int)enc624_read(0x7E07) << 8 | (unsigned int)enc624_read(0x7E06);

  // 2. get Next packet pointer
  _NextPacketPointer = (unsigned int)enc624_read(_ERXTAIL_(3)) << 8 | (unsigned int)enc624_read(_ERXTAIL_(2));

  // 3. get RSV & packet length
  for(i=0;i<6;i++){
    RSV[i] = enc624_read(_ERXTAIL_(i+4));
  }

  received_packet->packet_length = (unsigned int)RSV[1]<<8 | (unsigned int)RSV[0];

  // 4. Read Ethernet frame
  for(i=0;i<received_packet->packet_length;i++){
    temp = enc624_read(_ERXTAIL_(i+10));
    received_packet->packet[i] = temp;
  }

  // 5. Free up memory
  _ERXTAIL = _NextPacketPointer - 2;
  if(_ERXTAIL < _ERXST) _ERXTAIL = 0x5FFE;

  // L bit 癒쇱� �뜥�빞�븿..
  enc624_write(0x7E06, (unsigned char)(_ERXTAIL & 0x00FF));
  enc624_write(0x7E07, (unsigned char)(_ERXTAIL >> 8));

  // 6. Set PKTDEC to decrement the PKTCNT
  unsigned char ECON1_H = enc624_read(0x7E1F);
  ECON1_H |= 0x01;
  enc624_write(0x7E1F, ECON1_H);

  NOP;

  received_packet->packet_analyze();
}

void Enc624j600::enc624_send_packet(Packet* send_packet){
  unsigned char temp;

  // Wait for the hardware to clear TXRTS
  do{
    temp = enc624_read(0x7E1E);
  }while(temp & 0x02 == 1);

  // Transmit a packet procedure(p. 87)

  // 1. (p. 86) Initialze send configuration
  // Default value of
  //  TXMAC = 0 : MAC insertion disabled
  //  PADCFG<2:0> = 101 : Automatic padding enabled
  //  TXCRCEN = 1 : CRC generation enabled

  // 2. Copy the packet to the SRAM buffer
  unsigned int i;
  for(i=0;i<send_packet->packet_length;i++){
    enc624_write(i, send_packet->packet[i]);
  }

  // 3. Program ETXST to start address of packet
  enc624_write(0x7E00, 0x00);
  enc624_write(0x7E01, 0x00);

  // 4. Program ETXLEN with the length of data copied to the memory
  enc624_write(0x7E02, (unsigned char)(send_packet->packet_length & 0x00FF));
  enc624_write(0x7E03, (unsigned char)(send_packet->packet_length >> 8));

  // 5. Set the TXRTS bit to initiate transmission
  temp = enc624_read(0x7E1E); // ECON1_L
  temp |= 0x02; // Set TXRTS bit
  enc624_write(0x7E1E, temp);

  // 6. Wait for the hardware to clear TXRTS
  do{
    temp = enc624_read(0x7E1E);
  }while(temp & 0x02 == 1);

};

unsigned int Enc624j600::_ERXTAIL_(int i){
  // Cyclic index
  if(_ERXTAIL+i > 0x5FFF){
    return _ERXTAIL+i-(0x5FFF-_ERXST)-1;
  }else{
    return _ERXTAIL+i;
  }
}

unsigned int Enc624j600::enc624_get_interrupt_flag(){
  _EIR = (unsigned int)enc624_read(0x7E1D) << 8 | (unsigned int)enc624_read(0x7E1C);
  return _EIR;
}
