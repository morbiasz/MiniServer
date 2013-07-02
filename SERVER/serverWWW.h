#include <stm32f10x.h>
#include <enc28j60.h>
#include <delay.h>
#include "tcpip/ip_arp_udp_tcp.h"
#include <min_printf.h>
#include <conversion.h>
#include <string.h>
#include "websrv_help_functions.h"
#include "timeconversions.h"
#include "tcpip/net.h"

#define PSTR(s) s

#define WWW_CONFIG_ENABLED 	1
#define WWW_CONFIG_DISABLED 0

#define GW_ARP_REQUEST		0
#define GW_ARP_REQUEST_SENT	1
#define GW_ARP_RECEIVED		2

#define DNS_IP_REQUEST			0
#define DNS_IP_REQUEST_SENT		1
#define DNS_IP_RECEIVED			2
#define WEB_CLIENT_REQUEST		1
#define WEB_CLIENT_REQUEST_SENT 2
#define NTP_IDLE				0
#define NTP_REQUEST				1

void InitServer(void);
void ServerCheckForJob(void);
uint16_t http200ok(void);
uint16_t http200image(void);
int16_t analyse_get_url(char *str);
uint8_t verify_password(char *str);
uint16_t print_webpage(uint8_t *buf);

void ping_callback(uint8_t *ip);
void updateIP();
void resetNTPClient();
void disableWWWConfig();
void enableWWWConfig();
void changeIP( uint8_t * ip);
void www_clock_tick();
