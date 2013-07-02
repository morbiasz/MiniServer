#ifndef SDCARD_H_
#define SDCARD_H_

#include "spi_sd.h"
#include "Petit_FAT/diskio.h"

#define	xmit_spi(c)	SD_SPI_RW(c)
#define	rcvr_spi()	SD_SPI_RW(0xFF)
#define	rcv_spi()	SD_SPI_RW(0xFF)
#define	init_spi()	SD_SPI_Config()

#define _WRITE_FUNC	0

#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

DSTATUS disk_initialize(void);
BYTE wait_ready (void);

DRESULT disk_readp ( BYTE *buff, DWORD lba, WORD ofs, WORD cnt );
DRESULT disk_writep ( const BYTE *buff, DWORD sa );

void SELECT(void);
void DESELECT(void);
void release_spi (void);
BYTE send_cmd (BYTE,	DWORD);

#endif /* SDCARD_H_ */

