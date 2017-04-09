#ifndef __ENC624J600_H__
#define __ENC624J600_H__

#define NOP __asm__ __volatile__ ("nop\n\t")

#include "Arduino.h"
#include "packet.h"

class Enc624j600{
  public:
    void enc624_write(unsigned int address, unsigned char data);
    unsigned char enc624_read(unsigned int address);

    bool enc624_reset();
    void enc624_set_pinmap(int ad_pin[8], int a_pin[15], int cs_pin, int rd_pin, int wr_pin);

    void enc624_get_mac_address(unsigned char mac[6]);
    void enc624_set_mac_address(unsigned char mac[6]);

    void enc624_get_packet(Packet* received_packet);
    void enc624_send_packet(Packet* send_packet);

    void enc624_packet_read_enable();

    unsigned int enc624_get_interrupt_flag();

    Enc624j600(){
        _pinSet = false;
    }

  private:
    int _ADPIN[8];
    int _APIN[15];
    int _CSPIN, _RDPIN, _WRPIN;

    unsigned int _ERXST;
    unsigned int _ERXTAIL;
    unsigned int _EIR;
    unsigned int _NextPacketPointer;
    bool _pinSet;

    void enc624_init_port();
    void enc624_set_ad_pin_input();
    void enc624_set_ad_pin_output();

    unsigned int _ERXTAIL_(int i);
};

#endif

