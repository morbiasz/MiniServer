#include "serverWWW.h"
#include <stdlib.h>
#include <string.h>
#include <conversion.h>
#include <server_config.h>
#include "dnslkup.h"
#include "websrv_help_functions.h"
#include "timeconversions.h"
#include <temp_read.h>
#include "../RTC/rtc.h"

#include <stdio.h>

// przechowuje string z aktualna temperatura od czujnikow
extern char temperatura[MAX_LICZBA_CZUJNIKOW_TEMP][6];

/* Obrazek wyswietlany na stronie
 *
 *
 */
const uint8_t rawData[360] = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46,
		0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
		0xFF, 0xDB, 0x00, 0x43, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01,
		0x01, 0x01, 0x03, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x03,
		0x03, 0x03, 0x03, 0x04, 0x04, 0x06, 0x05, 0x04, 0x04, 0x05, 0x04, 0x03,
		0x03, 0x05, 0x07, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x04, 0x05,
		0x07, 0x07, 0x07, 0x06, 0x07, 0x06, 0x06, 0x06, 0x06, 0xFF, 0xC0, 0x00,
		0x0B, 0x08, 0x00, 0x10, 0x00, 0x10, 0x01, 0x01, 0x11, 0x00, 0xFF, 0xC4,
		0x00, 0x16, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x07, 0x0A, 0xFF, 0xC4,
		0x00, 0x20, 0x10, 0x00, 0x02, 0x03, 0x01, 0x00, 0x02, 0x02, 0x03, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x01, 0x04, 0x05,
		0x06, 0x07, 0x11, 0x00, 0x13, 0x12, 0x14, 0x21, 0xFF, 0xDA, 0x00, 0x08,
		0x01, 0x01, 0x00, 0x00, 0x3F, 0x00, 0xD3, 0xE7, 0x99, 0xBB, 0x87, 0x7D,
		0x56, 0xB4, 0xB9, 0x3E, 0xAB, 0x23, 0x53, 0x64, 0xF0, 0xAC, 0x6A, 0x75,
		0x7D, 0xBE, 0x66, 0xAB, 0x32, 0x06, 0xA3, 0xFF, 0x00, 0x70, 0x4E, 0xB5,
		0x1C, 0xEA, 0xED, 0x91, 0x10, 0xAD, 0x5D, 0xB1, 0xFB, 0x26, 0x72, 0x2D,
		0x73, 0x19, 0xE9, 0xC5, 0x31, 0x02, 0xDF, 0x82, 0x57, 0x91, 0xBC, 0x95,
		0x89, 0x6F, 0x9B, 0xE8, 0xFA, 0x3B, 0x2F, 0xD5, 0xEB, 0x7A, 0x1E, 0xD0,
		0x36, 0x68, 0xE7, 0x33, 0x57, 0x69, 0xB6, 0xB6, 0xE0, 0x35, 0xE5, 0x03,
		0xA1, 0x9C, 0xAD, 0x05, 0x17, 0xF5, 0xEC, 0xB2, 0x29, 0x70, 0xB0, 0x4A,
		0x42, 0x7E, 0xE3, 0x81, 0xFC, 0xE1, 0x93, 0x31, 0x4A, 0xF2, 0xB7, 0x6F,
		0xA5, 0x6E, 0xB7, 0x7A, 0x36, 0x1B, 0xC3, 0xF4, 0x3A, 0xEB, 0xE7, 0xF5,
		0x83, 0x33, 0x56, 0x2F, 0x15, 0x9C, 0xAC, 0xD5, 0x24, 0xD9, 0x00, 0xA6,
		0x42, 0xBD, 0x1B, 0x8D, 0x6A, 0x72, 0x8D, 0x91, 0xF4, 0xB0, 0x4B, 0xF8,
		0x3E, 0xFD, 0xCF, 0xAF, 0x85, 0x63, 0xBA, 0x5D, 0xE5, 0x73, 0xA8, 0xC7,
		0xDD, 0xD8, 0xD6, 0xA1, 0x77, 0x36, 0xE5, 0x7E, 0x92, 0xD5, 0x9C, 0x8C,
		0x75, 0x5A, 0x85, 0x55, 0x86, 0x0B, 0x86, 0xCA, 0x88, 0x0A, 0x1B, 0x5D,
		0x4B, 0x05, 0x55, 0x41, 0x43, 0x08, 0x0A, 0x1C, 0xC5, 0xFE, 0x20, 0x52,
		0x25, 0x23, 0xFF, 0xD9, };

uint8_t mymac[6] = { 0xf0, 0xd4, 0xa2, 0x9b, 0x12, 0x20 };

// Adres ip serwera
static uint8_t myip[4] = { 192, 168, 1, 25};

#define MYUDPPORT 1200
#define MYWWWPORT 80

// Bufor dla pakietow
#define BUFFER_SIZE 1400
static uint8_t buf[BUFFER_SIZE + 1];

// Zmienne specyficzne dla aplikacji

static uint8_t cmd_number = 0; // nr polecenia przeslanego przez URL
static unsigned char switch_state[4]; // aktualny stan przyciskow
static char gStrbuf[25]; // Bufor

static uint8_t WWWConfig_status = WWW_CONFIG_ENABLED;

// ---------- Zmienne uzywane przez funkcje synchronizacji adresu ip z zewnetrznym serwerem ------- //

#if UPDATE_IP
#define TRANS_NUM_GWMAC 1									//jakas stala - identyfikator tylko
#define WEBSERVER_VHOST "kubeqz.cba.pl"						//adres pod ktorym jest dostepny ten serwer
static uint8_t gwip[4] = { 192, 168, 1, 1 }; //ip gateway'a
static uint8_t gwmac[6]; //mac gateway'a

static uint8_t otherside_www_ip[4]; // ip uzyskane od serwera dns
static uint8_t pingsrcip[4];

static volatile uint8_t start_web_client = 1; // = 1 , aby po uruchomieniu zaktualizowal ip po starcie
static volatile uint8_t sec = 0;

static uint8_t web_client_attempts = 0;
static uint8_t web_client_sendok = 0;

static int8_t dns_state = 0;
static int8_t gw_arp_state = 0;

void arpresolver_result_callback(uint8_t *ip __attribute__((unused)), uint8_t transaction_number,
		uint8_t *mac);
void browserresult_callback(uint16_t webstatuscode, uint16_t datapos __attribute__((unused)),
		uint16_t len __attribute__((unused)));
#endif

// Sekcja dotycz¹ca zegara NTP
#if NTP_CLOCK

static char day[16];
static char clock[16];

static uint8_t ntpip[4] = { 17, 254, 0, 31 };
static int16_t hours_offset_to_utc = -10;

volatile uint8_t haveNTPanswer = 0;
volatile uint8_t send_ntp_req_from_idle_loop = 1;

#endif

void www_clock_tick() {

	sec++;

}

//Inicjalizacja serwera
void InitServer(void) {

	enc28j60Init(mymac);
	Delay(1);
	enc28j60clkout(0); //wylacz external CLK dla redukcji zak³oceñ ( EMI )
	Delay(1);
	enc28j60PhyWrite(PHLCON, 0x476);
	Delay(100);

#if UART_DEBUG
	min_printf("ENC28J60 wersja: %x\r\n", enc28j60getrev());
#endif

	//Zainicjalizuj serwer
	init_udp_or_www_server(mymac, myip);
	www_server_port(MYWWWPORT);

}

void changeIP(uint8_t * ip) {

	if (sizeof(ip) == sizeof(myip))
		memcpy(myip, ip, 4);

	init_udp_or_www_server(mymac, myip);

}

void enableWWWConfig() {

	WWWConfig_status = WWW_CONFIG_ENABLED;

}

void disableWWWConfig() {

	WWWConfig_status = WWW_CONFIG_DISABLED;

}

void updateIP() {

	start_web_client = 1;

}

void resetNTPClient() {

	haveNTPanswer = 0;
	send_ntp_req_from_idle_loop = NTP_REQUEST;

}

uint16_t print_image(uint8_t *buf) {
	uint16_t plen;

	plen = http200image();
	plen = fill_tcp_data_len(buf, plen, rawData, 360);

	return (plen);
}

// g³ówna funkcja serwera, sprawdza bufor kontrolera w poszukiwaniu]
// ¿¹dañ obs³ugi
void ServerCheckForJob(void) {
	int16_t cmd;
	uint16_t dat_p;

	uint16_t plen = 0;
	char str[20];
	uint8_t ntpclientportL = 0;
	uint32_t time = 0;
	uint32_t dayclock = 0;
	uint8_t tm_sec, tm_min, tm_hour;

	// handle ping and wait for a tcp packet
	plen = enc28j60PacketReceive(BUFFER_SIZE, buf);
	buf[BUFFER_SIZE] = '\0';
	dat_p = packetloop_arp_icmp_tcp(buf, plen);

	// jezeli to nie request wyswietlenia strony
	if (plen == 0) {

		// we are idle here trigger arp and dns stuff here
		if (gw_arp_state == GW_ARP_REQUEST)
		{
			// find the mac address of the gateway (e.g your dsl router).
			get_mac_with_arp(gwip, TRANS_NUM_GWMAC,
					&arpresolver_result_callback);
			gw_arp_state = GW_ARP_REQUEST_SENT;

			usart_printf("Wyslano DNS_IP_REQUEST\r\n");
		}

		if ((get_mac_with_arp_wait() == 0) && (gw_arp_state
				== GW_ARP_REQUEST_SENT))
		{
			gw_arp_state = GW_ARP_RECEIVED;

		}

		if (dns_state == DNS_IP_REQUEST && gw_arp_state == GW_ARP_RECEIVED)
		{

			if (!enc28j60linkup())
				return;

			sec = 0;
			dns_state = DNS_IP_REQUEST_SENT;
			dnslkup_request(buf, WEBSERVER_VHOST, gwmac);

			return;
		}

		if (dns_state == DNS_IP_REQUEST_SENT && dnslkup_haveanswer())
		{
			dns_state = DNS_IP_RECEIVED;
			dnslkup_get_ip(otherside_www_ip);

#if UART_DEBUG
			usart_printf("Odebrano IP gateway'a\r\n");
#endif

		}

		if (dns_state != DNS_IP_RECEIVED) {

			//Po minucie spróbuj ponownie
			if (sec > DNS_RETRY_TIME) {
				dns_state = DNS_IP_REQUEST;
				gw_arp_state = GW_ARP_REQUEST;
				sec = 0;
			}

			// jezeli nie bedziemy miec IP DNS'a to nie mozemy kontaktowac sie ze stronami www
			return;

		}

		if (start_web_client == WEB_CLIENT_REQUEST)
		{

#if UART_DEBUG
			usart_printf("Wyslano request odswiezenia IP\r\n");
#endif

			sec = 0;
			start_web_client = WEB_CLIENT_REQUEST_SENT;
			web_client_attempts++;
			mk_net_str(str, pingsrcip, 4, '.', 10);
			urlencode(str, gStrbuf);
			client_browse_url("/update_ip.php", gStrbuf, PSTR(WEBSERVER_VHOST),
					&browserresult_callback, otherside_www_ip, gwmac);
		}

		// reset after a delay to prevent permanent bouncing
		if ((sec > 60) && (start_web_client == WEB_CLIENT_REQUEST_SENT))
		{
			start_web_client = 0;
		}

		return;
	}

	// check if udp otherwise continue
	if (dat_p == 0)
	{
		// check for incomming messages not processed
		// as part of packetloop_arp_icmp_tcp, e.g udp messages

		udp_client_check_for_dns_answer(buf, plen);
		goto UDP;
	}

	if (!strncmp("GET /camera.jpg", (char *) &(buf[dat_p]), 15) != 0) {
		plen = print_image(buf);
		goto SENDTCP;
	}

	if (strncmp("GET ", (char *) &(buf[dat_p]), 4) != 0) {

		plen = http200ok();
		plen = fill_tcp_data_p(buf, plen, PSTR("<h1>200 OK</h1>"));
		goto SENDTCP;

	}

	// wyswietl glowna strone
	if (strncmp("/ ", (char *) &(buf[dat_p + 4]), 2) == 0) {
		plen = print_webpage(buf);
		goto SENDTCP;
	}

	cmd = analyse_get_url((char *) &(buf[dat_p + 4]));

	switch (cmd) {
	case 1:

		switch_state[cmd - 1] = ~switch_state[cmd - 1];
		GPIOC->ODR ^= ~GPIO_Pin_9;
		break;

	case 2:

		switch_state[cmd - 1] = ~switch_state[cmd - 1];
		break;

	case 3:

		switch_state[cmd - 1] = ~switch_state[cmd - 1];
		break;

	case 4:

		switch_state[cmd - 1] = ~switch_state[cmd - 1];
		break;

	default:
		if (cmd >= 60 && cmd < 201 && cmd_number == 0) {
			TIM4->CCR4 = (uint16_t) cmd;
		} else if (cmd_number > 0) {
			switch (cmd_number) {
			case 1:
				TIM4->CCR3 = cmd; //set RED
				break;
			case 2:
				TIM4->CCR1 = cmd; //set GREEN
				break;
			case 3:
				TIM4->CCR2 = cmd; //set BLUE
				break;

			default:
				break;

			}
			cmd_number = 0;
		}
		break;

	}

	plen = print_webpage(buf);

	SENDTCP: www_server_reply(buf, plen); // send data
	return;

	UDP:					// UDP/NTP protocol handling and idle tasks
							// check if ip packets are for us:
#if NTP_CLOCK
	// we are idle here (no incomming packet to process).
	if ((send_ntp_req_from_idle_loop == NTP_REQUEST) && (dns_state == DNS_IP_RECEIVED))
	{

		send_ntp_req_from_idle_loop = NTP_IDLE;
		client_ntp_request(buf, ntpip, ntpclientportL, gwip);

		if (haveNTPanswer == 0)
		{
#if UART_DEBUG
			min_printf("Waiting for NTP server\r\n");
#endif
		}


	}
	if (haveNTPanswer==0)
			{
      // ntp src port must be 123 (0x7b):
      if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_SRC_PORT_H_P]==0&&buf[UDP_SRC_PORT_L_P]==0x7b)
			{
				if (client_ntp_process_answer(buf,&time,ntpclientportL))
				{
					// convert to unix time:
					if ((time & 0x80000000UL) ==0)
					{
						// 7-Feb-2036 @ 06:28:16 UTC it will wrap:
						time+=2085978496;
					}
					else
					{
						// from now until 2036:
						time-= GETTIMEOFDAY_TO_NTP_OFFSET;
					}
					time += (int32_t)-360*(int32_t)hours_offset_to_utc;

					// returns day and time-string in seperate variables:
					gmtime(time,day,clock);

					#if UART_DEBUG
						min_printf("Zegar NTP: %s %s\r\n", day, clock);
					#endif

					dayclock = time % 86400UL;

					tm_sec = dayclock % 60UL;
					tm_min = (dayclock % 3600UL) / 60;
					tm_hour = dayclock / 3600UL;

					/* Wait until last write operation on RTC registers has finished */
					while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET);

					RTC_SetCounter(tm_sec + tm_min*60 + tm_hour*3600);

					haveNTPanswer=1;
				}
     }
		}

#endif	 
}

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf) {
	uint16_t plen;
	uint8_t i = 0;
	char bufor[2];
	RTCDate data;

	RTCgetTime(&data);

	plen = http200ok();

	plen
			= fill_tcp_data_p(
					buf,
					plen,
					"<html><head></head><body bgcolor=#CACAE8>\
<h2 style=\"font-family:Arial,Helvetica,sans-serif;color:#E815B0;text-align:center\">Serwer STM32Discovery</h2>\
</br>\
<table border=1 style=\"background-color:#E6E6F0;border:1px solid black;border-collapse:collapse;\" width=60% cellpadding=0 cellspacing=0 align=center>\
<tr style=\"padding:10px;text-align:center;font-family:\"Georgia\",Helvetica,sans-serif\">\
<td colspan=\"4\">Stan wyjsc:</td>\
<td></td>\
<td colspan=\"4\">Aktualna temperatura:</td>\
</tr>\
<tr style=\"padding:5px;text-align:center\">");

	// Uzupelnianie danych na podstawie statusu
	for (i = 0; i < 4; i++) {
		plen = fill_tcp_data_p(buf, plen, "<td width=12%><a href=\"./?sw=");
		itoa(i + 1, bufor, 10);

		//nr switcha
		plen = fill_tcp_data_p(buf, plen, bufor);
		plen = fill_tcp_data_p(buf, plen, "\"><font color=#");

		if (switch_state[i])
			plen = fill_tcp_data_p(buf, plen, "00FF00>ON");
		else
			plen = fill_tcp_data_p(buf, plen, "FF0000>OFF");

		plen = fill_tcp_data_p(buf, plen, "</font></a></td>");

	}
	plen = fill_tcp_data_p(buf, plen, "<td></td>\
<td width=15%>");
	plen = fill_tcp_data_p(buf, plen, temperatura[0]);
	plen = fill_tcp_data_p(buf, plen, "</td>\
<td width=15%>");
	//plen = fill_tcp_data_p(buf, plen, temperatura[1]);
	plen
			= fill_tcp_data_p(
					buf,
					plen,
					"</td>\
</tr>\
</table>\
</br>\
<p style=\"padding:10px;text-align:center;font-family:Arial,Helvetica,sans-serif\">\
<font>Obrazek:</font>\
</p>\
</br>\
<p style=\"text-align:center;\">\
<img src=\"camera.jpg\" height=160 width=160/>\
</p>\
</br>\
<hr><pre>Design & coding by Jakub Zbydniewski 12.2012</br>Aktualny czas RTC: ");
	plen = fill_tcp_data_p(buf, plen, day);
	plen = fill_tcp_data_p(buf, plen, " ");
	sprintf(gStrbuf, "%d:%d", data.hour, data.minute);
	plen = fill_tcp_data_p(buf, plen, gStrbuf);
	plen = fill_tcp_data_p(buf, plen, "<pre></body></html>");

	return (plen);
}

int16_t analyse_get_url(char *str) {
	int16_t temp = 0;

	// Sprawdzanie pierwszego "/"
	if (*str == '/') {
		str++;
	} else {
		return (-1);
	}

	// str is now something like ?sw=1 or just end of url
	if (find_key_val(str, gStrbuf, 5, "sw")) {
		return (gStrbuf[0] - 48);
	}

#if SERVO			
	// if str is like ?pw=120
	if (find_key_val(str,gStrbuf,10,"pw")) {
		temp = myatoi(gStrbuf);
		return (int16_t)temp;
	}
#endif

#if RGB_DIODE
	if (find_key_val(str, gStrbuf, 10, "setR")) {
		temp = myatoi(gStrbuf);
		cmd_number = 1;
		return (int16_t) temp;
	}

	if (find_key_val(str, gStrbuf, 10, "setG")) {
		temp = myatoi(gStrbuf);
		cmd_number = 2;
		return (int16_t) temp;
	}

	if (find_key_val(str, gStrbuf, 10, "setB")) {
		temp = myatoi(gStrbuf);
		cmd_number = 3;
		return (int16_t) temp;
	}
#endif				

	return (-3);
}

uint16_t http200ok(void) {
	return (fill_tcp_data_p(
			buf,
			0,
			PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n")));
}

uint16_t http200image(void) {
	return (fill_tcp_data_p(
			buf,
			0,
			PSTR("HTTP/1.0 200 OK\r\nContent-Type: image/bmp\r\nContent-Length: 623\r\n\r\n")));
}

#if UPDATE_IP

// the __attribute__((unused)) is a gcc compiler directive to avoid warnings about unsed variables.
void browserresult_callback(uint16_t webstatuscode, uint16_t datapos __attribute__((unused)),
		uint16_t len __attribute__((unused))) {
	if (webstatuscode == 200) {
		web_client_sendok++;

#if UART_DEBUG
		usart_printf("Uaktualniono IP\r\n");
#endif
	}
}

// the __attribute__((unused)) is a gcc compiler directive to avoid warnings about unsed variables.
void arpresolver_result_callback(uint8_t *ip __attribute__((unused)), uint8_t transaction_number,
		uint8_t *mac) {
	uint8_t i = 0;
	if (transaction_number == TRANS_NUM_GWMAC) {

#if UART_DEBUG
		min_printf("Adres MAC bramy: %x:%x:%x:%x:%x:%x\r\n", mac[0], mac[1],
				mac[2], mac[3], mac[4], mac[5]);
#endif
		// skopiuj pozyskany adres MAC bramy
		while (i < 6) {
			gwmac[i] = mac[i];
			i++;
		}
	}
}

#endif

