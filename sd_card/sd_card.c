#include "sd_card.h"
#include "spi_sd.h"
#include <usart.h>
#include <delay.h>
#include "Petit_FAT/diskio.h"

BYTE CardType;

#define SD_RETURN(VALUE) DESELECT();return VALUE;

void SELECT() 
{
	 GPIOB->BRR =  GPIO_Pin_12;
}

void DESELECT() 
{
	 GPIOB->BSRR = 	GPIO_Pin_12;
}

void release_spi (void)
{
	DESELECT();
	rcv_spi();
}


BYTE wait_ready (void)
{
	BYTE res;
	DWORD timeout = 0x7FFF;
	
	do
		res = rcv_spi();
	while ((res != 0xFF) && (--timeout));
	
	return res;
}



BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	DESELECT();
	SELECT();
	if (wait_ready() != 0xFF) {
		return 0xFF;
	}

	/* Send command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */

	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}


void sdCommand(char cmd, uint32_t Addr, char crc)
{	
	// sends a command to the SD/MMC

    xmit_spi(0xFF);
    xmit_spi(0x40 | cmd);
    xmit_spi((uint8_t)(Addr >> 24));
    xmit_spi((uint8_t)(Addr >> 16));
    xmit_spi((uint8_t)(Addr >> 8));
    xmit_spi((uint8_t)Addr);
    xmit_spi(crc);
    xmit_spi(0xFF);

}

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];
	WORD tmr;
	DWORD tmp;

	init_spi();
	DESELECT();

	for (n = 12; n; n--) 
		rcv_spi();	// Dummy clocks 

	//Delay(10);

	SELECT(); 
	//Delay(5);

	ty = 0;
	if (send_cmd(CMD0, 0) == 1)    			// Enter Idle state 
	{			
		if (send_cmd(CMD8, 0x1AA) == 0) 	// SDv2
		{	
			for (n = 0; n < 4; n++) 
				ocr[n] = rcv_spi();		// Get trailing return value of R7 resp

			if (ocr[2] == 0x01 && ocr[3] == 0xAA) // The card can work at vdd range of 2.7-3.6V 
			{				
				for (tmr = 12000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--) ;	// Wait for leaving idle state (ACMD41 with HCS bit)
				
				if (tmr && send_cmd(CMD58, 0) == 0)		// Check CCS bit in the OCR
				{	
					for (n = 0; n < 4; n++) 
						ocr[n] = rcv_spi();

					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	// SDv2 (HC or SC)
				}
			}
		}
		else 		  						// SDv1 or MMCv3 
		{					
			if (send_cmd(ACMD41, 0) <= 1) 	
			{
				ty = CT_SD1; cmd = ACMD41;// SDv1 
			} 
			else 	// Proteus 7.x MMC sim model
			{	
				ty = CT_MMC; cmd = CMD1;	// MMCv3
			}

			for (tmr = 25000; tmr && send_cmd(cmd, 0); tmr--) ; // Wait for leaving idle state

			if (!tmr || send_cmd(CMD16, 512) != 0)		// Set R/W block length to 512
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();
		
	if(ty){
		tmp = SPI2->CR1;
		// Set fast clock (depends on the CSD) , zmiana z 256 na  128
		tmp = ( tmp & ~SPI_BaudRatePrescaler_256 ) | SPI_BaudRatePrescaler_128;
		SPI2->CR1 = tmp;	
	}

	return ty ? 0 : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
	DWORD lba,		/* Sector number (LBA) */
	WORD ofs,		/* Byte offset to read from (0..511) */
	WORD cnt		/* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE rc;
	WORD bc;


	if (!(CardType & CT_BLOCK)) lba *= 512;		/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, lba) == 0) {		/* READ_SINGLE_BLOCK */

		bc = 30000;
		do {							/* Wait for data packet in timeout of 100ms */
			rc = rcv_spi();
		} while (rc == 0xFF && --bc);

		if (rc == 0xFE) {				/* A data packet arrived */
			bc = 514 - ofs - cnt;

			/* Skip leading bytes */
			if (ofs) {
				do rcv_spi(); while (--ofs);
			}

			/* Receive a part of the sector */
			do
				*buff++ = rcv_spi();
			while (--cnt);

			/* Skip trailing bytes and CRC */
			do rcv_spi(); while (--bc);

			res = RES_OK;
		}
	}

	release_spi();

	return res;
}

/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (const BYTE *buff, DWORD sa	)
{
	DRESULT res;
	WORD bc;
	static WORD wc;


	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = (WORD)sa;
		while (bc && wc) {		/* Send data bytes to the card */
			xmit_spi(*buff++);
			wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sa) {	/* Initiate sector write process */
			if (!(CardType & CT_BLOCK)) sa *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sa) == 0) {			/* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF); xmit_spi(0xFE);		/* Data block header */
				wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = wc + 2;
			while (bc--) xmit_spi(0);	/* Fill left bytes and CRC with zeros */
			if ((rcv_spi() & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 300ms */
				for (bc = 65000; rcv_spi() != 0xFF && bc; bc--) ;	/* Wait ready */
				if (bc) res = RES_OK;
			}
			release_spi();
		}
	}

	return res;
}




