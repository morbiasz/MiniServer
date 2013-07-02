#include "stm32f10x.h"
#include "delay.h"
#include "1wire.h"
#include <usart.h>

extern volatile uint32_t TimingDelay;			//zmienna potrzebna po to aby mo¿na by³o przerwaæ czekanie jesli nacisnieto przycisk "menu"
extern volatile char menu;

unsigned int gSensorIDs[MAXSENSORS][8];

GPIO_InitTypeDef GPIO_InitStructure;

void wOut(void)
{
   GPIO_InitStructure.GPIO_Pin = OW_PIN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
   GPIO_Init(OW_GPIO, &GPIO_InitStructure);
}

void wIn(void)
{
   GPIO_InitStructure.GPIO_Pin = OW_PIN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(OW_GPIO, &GPIO_InitStructure);

}

unsigned char ow_reset (void) 		//reset magistrali 1wire
{
	unsigned char present;
	unsigned char i=4;
	while(i--) ;

	wL;
	wOut();
	Delay_us( 500 ); 						// 480 us
	wIn();
	Delay_us(60); 							// 70 us
	present = wR;
	Delay_us(500); 							// 410 us
											// !! 0 jesli jest obecny !!
	return present;
}


unsigned char ow_read_bit (void) {            //czytanie bitu

	unsigned char data=0;

	wOut();
	wL;
	Delay_us(4); // 6 us
	wIn();
	Delay_us(4); // 9 us

	if (wR) data=1;

	Delay_us(60); // 55 us

	return data;

}

void ow_write_bit (unsigned char bit) {   //wysylanie bitu

	wOut();                            	  //wyjscie
	wL;                                	  //stan niski

	if (bit)                       		  //procedura wprost z datasheeta ds18b20
	{
		Delay_us(6);            		  // 6 us
		wIn();                            // zwolnienie linii
		Delay_us(62);            		  // 64 us
	}
	else
	{
		Delay_us(62);           		  // 60 us
		wIn();             				  // zwolnienie linii
		Delay_us(6);        			  // 10 us
	}

}

void ow_write_byte (unsigned char data) {    //wysylanie bajtu

	unsigned char i;

	for (i=0;i<8;i++)
    {
        ow_write_bit(data & 0x01);          //najmlodszy bit pobrany i wyslany na linie 1wire
        data>>=1;                    		//nastepnie bajt przesuwamy w prawo aby pobrac kolejny bit
                                    		//i tak w kolko az przeslemy wszystkie bity
    }

}


unsigned char ow_read_byte(void) {          //czytanie bajtu

    unsigned char i, bit, data;

    data=0;                            		//aby nie bylo zadnych smieci
    for (i=0; i<8; i++)            			//bajt to 8 bitow
    {
    	bit=ow_read_bit();             		//po kolei czytamy bit po bicie
    	data|=bit<<i;               	    //pierwszy odczytany bit jest ostatnim w bajcie
    }

    return data;

}

void ow_parasite_on(void) {            			//stronger pull up on

	wH_parasite;
}

void ow_parasite_off(void) {        				//stronger pull up off

	wL_parasite;
}


void ow_command( unsigned int command, unsigned int *id )
{
    uint8_t i;

    ow_reset();

    if( id ) {
        ow_write_byte( 0x55 );            // to a single device MATCH ROM command
        i = 8;
        do {
            ow_write_byte( *id );
            id++;
        } while( --i );
    }
    else {
        ow_write_byte( 0xcc );            // to all devices SKIP ROM command
    }
    
    ow_write_byte( command );
}

unsigned int ow_rom_search( uint8_t diff, uint8_t *id )
{
    uint8_t i, j, next_diff;
    uint8_t b;
    
    if( ow_reset() ) return(0);            // error, no device found
    
    ow_write_byte(0xf0);                    // ROM search command
    next_diff = 0;                            // unchanged on last device
    
    i = 64;                                    // 8 bytes
    
    do {
        j = 8;                                // 8 bits
        do {
            b = ow_read_bit();                // read bit
            if( ow_read_bit() ) {            // read complement bit
                if( b )                        // 11
                return(0);                    // data error
            }
            else {
                if( !b ) {                    // 00 = 2 devices
                    if( diff > i || ((*id & 1) && diff != i) ) {
                    b = 1;                    // now 1
                    next_diff = i;            // next pass 0
                    }
                }
            }
            ow_write_bit( b );             // write bit
            *id >>= 1;
            if( b ) *id |= 0x80;            // store bit
            
            i--;
            
        } while( --j );
        
        id++;                                // next byte
    
    } while( i );
    
    return next_diff;                        // to continue search
}

void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
	
		uint16_t i =0;
	
    for (;;) {
			i++;
        *diff = ow_rom_search( *diff, &id[0] );
        if ( *diff== 0xFF || *diff==0xFE ||
         *diff == 0 ) return;
        if ( id[0] == DS18B20_ID || id[0] == DS18S20_ID ) return;
				
				if (i > 10) return;
    }
}


uint8_t search_sensors(void)
{
    uint8_t i;
    uint8_t id[8];
	  uint8_t j = 100;
    uint8_t diff, nSensors;
    #if UART_DEBUG
		usart_printf("Bus scanning..\r");
		#endif
    nSensors = 0;
    
    for( diff = 0xff; diff !=0 && nSensors < MAXSENSORS && j-- ; )
    {
        DS18X20_find_sensor( &diff, &id[0] );

        if( diff == 0xff ) {
            #if UART_DEBUG
						usart_printf("No sensor found\r");
						#endif
            break;
        }
        
        if( diff == 0xfe ) {
            #if UART_DEBUG
						usart_printf("Bus error\r");
						#endif
            break;
        }
        
        for (i=0;i<8;i++)
            gSensorIDs[nSensors][i]=id[i];
        
        nSensors++;
    }
    
    return nSensors;
}


//odczytuje temp.
uint8_t read_temp(int8_t * calkowita_temp, int8_t * ulamkowa_temp)
{
	uint8_t scratchpad[9];
	uint8_t i=0;
	uint8_t msb = 0, lsb = 0, minus = 0;

	for(i=0;i< SCRATCHPAD_SIZE;i++)
		scratchpad[i] = ow_read_byte();
		
	if (crc8(scratchpad, 8) != scratchpad[8])
		return 1;

	lsb = scratchpad[0]; 				  //czytamy LSB i MSB przechowujace temperature
	msb = scratchpad[1];
	
	if (msb & 0x80)        			  //dla liczb ujemnych negacja i +1
	{
		msb=~msb;
		lsb=~lsb+1;
		minus=1;
	}
	else 
		minus=0;				// in case of changing from + to -
	
	*calkowita_temp = (uint8_t) ((uint8_t) (msb&0x7)<<4 ) | ((uint8_t) (lsb&0xf0)>>4 );    //wyodrêbnia ca³kowit¹ wartoœæ temperatury    
	if(minus)
		*calkowita_temp = - *calkowita_temp;
		
	*ulamkowa_temp = ((lsb & 0x0F)*625)/1000;            //wyodrebnia ulamkowa czesc temperatury
	
	return 0; 					//everything ok
	
}

unsigned char crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read )
{
	uint8_t	 crc;
	uint16_t loop_count;
	uint8_t  bit_counter;
	uint8_t  data;
	uint8_t  feedback_bit;

	crc = CRC8INIT;

	for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
	{
		data = data_in[loop_count];

		bit_counter = 8;
		do {
			feedback_bit = (crc ^ data) & 0x01;

			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}

			data = data >> 1;
			bit_counter--;

		} while (bit_counter > 0);
	}

	return crc;
}



