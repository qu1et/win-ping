#ifndef ECHO_REQUEST_H
#define ECHO_REQUEST_H
							/*Макроопределелния*/
#define PACKET_SIZE 32
#define REP_NUMBER 4
#define DEFAULT_TIMEOUT 10
#define ICMP_ECHO_REQUEST 8
#define ECHO_REQUEST_ERROR -11
							/*Определение пользоательских типов данных (ALIAS для существующих)*/
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

							//структура описывающая заголовок ICMP пакета
typedef struct ICMP_HEADER {
	BYTE type;				// Тип
	BYTE code;				// Код
	USHORT checksum;		// Контрольная сумма
	USHORT id;				// Идентификатор
	USHORT seq;				// Порядковый номер
} icmphdr, * PICMPHDR;

typedef struct IPHeader {
	BYTE h_len : 4;			// Длина заголовка в dwords
	BYTE version : 4;		// Версия IP
	BYTE tos;				// Тип сервиса
	USHORT total_len;		// Длина пакета в dwords
	USHORT ident;			// Уникальный идентификатор
	USHORT flags;			// Флаги
	BYTE ttl;				// Time to live
	BYTE proto;				// Protocol number (TCP, UDP etc)
	USHORT checksum;		// Контрольная сумма IP
	ULONG source_ip;		// Ip источника
	ULONG dest_ip;			// Ip получателя
}IPHeader_t, * pIPHeader;

USHORT ip_checksum(USHORT*, int);
int echo_request(SOCKET*, char*, int, int*, someArgs_t*, int, int);
int catcher(SOCKET, someArgs_t*, int*, int*, int, int);
#endif						/* ECHO_REQUEST_H */
