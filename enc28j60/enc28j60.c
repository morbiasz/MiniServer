
#include "min_printf.h"
#include "enc28j60.h"
#include "spi.h"
#include "delay.h"
#include <server_config.h>

static unsigned char Enc28j60Bank;
static unsigned int NextPacketPtr;


unsigned char enc28j60ReadOp(unsigned char op, unsigned char address)
	{
	unsigned char dat = 0;
	
	ENC28J60_CSL();
	Delay_us(1);
	dat = op | (address & ADDR_MASK);
	SPI1_ReadWrite(dat);
	Delay_us(1);
	dat = SPI1_ReadWrite(0xFF);
	// do dummy read if needed (for mac and mii, see datasheet page 29)
	if(address & 0x80)
	    {
				dat = SPI1_ReadWrite(0xFF);
	    }
	// release CS
	Delay_us(1);
	ENC28J60_CSH();
	return dat;
	}

void enc28j60WriteOp(unsigned char op, unsigned char address, unsigned char data)
	{
	unsigned char dat = 0;
	  
	ENC28J60_CSL();
	Delay_us(1);
	// issue write command
	dat = op | (address & ADDR_MASK);
	SPI1_ReadWrite(dat);
	// write data
	dat = data;
	SPI1_ReadWrite(dat);
	ENC28J60_CSH();
	}

void enc28j60ReadBuffer(unsigned int len, unsigned char* data)
	{
	ENC28J60_CSL();
	// issue read command
	SPI1_ReadWrite(ENC28J60_READ_BUF_MEM);
	while(len)
		{
        len--;
        // read data
        *data = (unsigned char)SPI1_ReadWrite(0);
        data++;
		}
	*data='\0';
	ENC28J60_CSH();
	}

void enc28j60WriteBuffer(unsigned int len, unsigned char* data)
	{
	ENC28J60_CSL();
	// issue write command
	SPI1_ReadWrite(ENC28J60_WRITE_BUF_MEM);
	
	while(len)
		{
		len--;
		SPI1_ReadWrite(*data);
		data++;
		}
	ENC28J60_CSH();
	}

void enc28j60SetBank(unsigned char address)
	{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
		{
        // set the bank
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        Enc28j60Bank = (address & BANK_MASK);
		}
	}

unsigned char enc28j60Read(unsigned char address)
	{
	// set the bank
	enc28j60SetBank(address);
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
	}

void enc28j60Write(unsigned char address, unsigned char data)
	{
	// set the bank
	enc28j60SetBank(address);
	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
	}

void enc28j60PhyWrite(unsigned char address, unsigned int data)
	{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	// write the PHY data
	enc28j60Write(MIWRL, data);
	enc28j60Write(MIWRH, data>>8);
	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY)
		{
	    	//Del_10us(1);
			//_nop_();
		}
	}

void enc28j60clkout(unsigned char clk)
	{
    //setup clkout: 2 is 12.5MHz:
	enc28j60Write(ECOCON, clk & 0x7);
	}
	
	// A number of utility functions to enable/disable broadcast 
void enc28j60EnableBroadcast( void ) {
        uint8_t erxfcon;
        erxfcon=enc28j60Read(ERXFCON);
        erxfcon |= ERXFCON_BCEN;
        enc28j60Write(ERXFCON, erxfcon);
}

void enc28j60DisableBroadcast( void ) {
        uint8_t erxfcon;
        erxfcon=enc28j60Read(ERXFCON);
        erxfcon &= (0xff ^ ERXFCON_BCEN);
        enc28j60Write(ERXFCON, erxfcon);
}

// read upper 8 bits
uint16_t enc28j60PhyReadH(uint8_t address)
{

	// Set the right address and start the register read operation
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);

	// wait until the PHY read completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);

	// reset reading bit
	enc28j60Write(MICMD, 0x00);
	
	return (enc28j60Read(MIRDH));
}

// link status
uint8_t enc28j60linkup(void)
{
        // bit 10 (= bit 3 in upper reg)
        if (enc28j60PhyReadH(PHSTAT2) && 4){
                return(1);
        }
        return(0);
}
	

void enc28j60Init(unsigned char* macaddr)
	{

	// initialize I/O
	ENC28J60_CSH();
	Delay(1);

	// perform system reset
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	Delay(100);

	NextPacketPtr = RXSTART_INIT;

   // Rx start
	enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXSTH, RXSTART_INIT>>8);
	// set receive pointer address
	enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
	// RX end
	enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
	// TX start
	enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60Write(ETXSTH, TXSTART_INIT>>8);
	// TX end
	enc28j60Write(ETXNDL, TXSTOP_INIT&0xFF);
	enc28j60Write(ETXNDH, TXSTOP_INIT>>8);

	enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	enc28j60Write(EPMM0, 0x3f);
	enc28j60Write(EPMM1, 0x30);
	enc28j60Write(EPMCSL, 0xf9);
	enc28j60Write(EPMCSH, 0xf7);

	// enable MAC receive
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset
	enc28j60Write(MACON2, 0x00);
	// enable automatic padding to 60bytes and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// set inter-frame gap (non-back-to-back)
	enc28j60Write(MAIPGL, 0x12);
	enc28j60Write(MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	enc28j60Write(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
        // Do not send packets longer than MAX_FRAMELEN:
	enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);
	enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);
	// do bank 3 stuff
        // write MAC address
        // NOTE: MAC address in ENC28J60 is byte-backward
        enc28j60Write(MAADR5, macaddr[0]);
        enc28j60Write(MAADR4, macaddr[1]);
        enc28j60Write(MAADR3, macaddr[2]);
        enc28j60Write(MAADR2, macaddr[3]);
        enc28j60Write(MAADR1, macaddr[4]);
        enc28j60Write(MAADR0, macaddr[5]);
				
	// no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
	// switch to bank 0
	enc28j60SetBank(ECON1);
	// enable interrutps
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	// enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
#if UART_DEBUG
	min_printf("MAC: = %x:%x:%x:%x:%x:%x\r\n", enc28j60Read(MAADR5), enc28j60Read(MAADR4),
			enc28j60Read(MAADR3), enc28j60Read(MAADR2), enc28j60Read(MAADR1), enc28j60Read(MAADR0));
#endif
	}

// read the revision of the chip:
unsigned char enc28j60getrev(void)
	{
	return(enc28j60Read(EREVID));
	}

void enc28j60PacketSend(uint16_t len, unsigned char* packet)
	{
       // Check no transmit in progress
        while (enc28j60ReadOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS)
        {
                // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
                if( (enc28j60Read(EIR) & EIR_TXERIF) ) {
                        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
                        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
                }
        }
				
	// Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, TXSTART_INIT&0xFF);
	enc28j60Write(EWRPTH, TXSTART_INIT>>8);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+len)&0xFF);
	enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);
	// write per-packet control byte (0x00 means use macon3 settings)
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
				
	}

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
unsigned int enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
	{
	unsigned int rxstat;
	unsigned int len;

	// check if a packet has been received and buffered
	//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
        // The above does not work. See Rev. B4 Silicon Errata point 6.
	if( enc28j60Read(EPKTCNT) ==0 )
		{
		return(0);
        }

	// Set the read pointer to the start of the received packet
	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

	// read the next packet pointer
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

	// read the packet length (see datasheet page 43)
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    len-=4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// limit retrieve length
    if (len>maxlen-1)
		{
        len=maxlen-1;
    	}

        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not
        // need to check this.
    if ((rxstat & 0x80)==0)
			{
		    // invalid
		    len=0;
		    }
		else
			{
            // copy the packet from the receive buffer
            enc28j60ReadBuffer(len, packet);
        	}
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	enc28j60Write(ERXRDPTL, (NextPacketPtr));
	enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);

	// decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
	}



