#include <inttypes.h>
#include <sys/types.h>
#include <errno.h>

void RC5Timer_Config(void);
void GPIO_Config(void);
void RCC_Conf(void); 
void NVIC_Config(void);
void pwmSetup(void);
void rgbPwmSetup(void);
caddr_t _sbrk (int size);
