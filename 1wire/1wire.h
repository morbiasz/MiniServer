
//definicja pod³aczenia czujnika DS18B20
#define OW_GPIO	GPIOC
#define OW_PIN	GPIO_Pin_10

#define wH	OW_GPIO->BSRR = OW_PIN
#define wL	OW_GPIO->BRR = OW_PIN
#define wR  (OW_GPIO->IDR) & OW_PIN

#define wH_parasite wH
#define wL_parasite wL

#define MAXSENSORS 3
#define DS18B20_ID 0x28

#define DS18S20_ID 0x10

#define SCRATCHPAD_SIZE 9

#define CRC8INIT	0x00
#define CRC8POLY	0x18     

unsigned char crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read );
uint8_t read_temp(int8_t * calkowita_temp, int8_t * ulamkowa_temp);
void ow_command( unsigned int command, unsigned int *id );
unsigned int ow_rom_search( uint8_t diff, uint8_t *id );
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[]);
void ow_write_byte (unsigned char data);
void ow_write_bit (unsigned char bit);
unsigned char ow_read_bit (void);
unsigned char ow_read_byte(void);
unsigned char ow_reset (void);
uint8_t search_sensors(void);
void ow_parasite_off(void);
void ow_parasite_on(void);
