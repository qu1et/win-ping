#ifndef ECHO_REQUEST_H
#define ECHO_REQUEST_H
							/*�����������������*/
#define PACKET_SIZE 32
#define REP_NUMBER 4
#define DEFAULT_TIMEOUT 10
#define ICMP_ECHO_REQUEST 8
#define ECHO_REQUEST_ERROR -11
							/*����������� ��������������� ����� ������ (ALIAS ��� ������������)*/
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

							//��������� ����������� ��������� ICMP ������
typedef struct ICMP_HEADER {
	BYTE type;				// ���
	BYTE code;				// ���
	USHORT checksum;		// ����������� �����
	USHORT id;				// �������������
	USHORT seq;				// ���������� �����
} icmphdr, * PICMPHDR;

typedef struct IPHeader {
	BYTE h_len : 4;			// ����� ��������� � dwords
	BYTE version : 4;		// ������ IP
	BYTE tos;				// ��� �������
	USHORT total_len;		// ����� ������ � dwords
	USHORT ident;			// ���������� �������������
	USHORT flags;			// �����
	BYTE ttl;				// Time to live
	BYTE proto;				// Protocol number (TCP, UDP etc)
	USHORT checksum;		// ����������� ����� IP
	ULONG source_ip;		// Ip ���������
	ULONG dest_ip;			// Ip ����������
}IPHeader_t, * pIPHeader;

USHORT ip_checksum(USHORT*, int);
int echo_request(SOCKET*, char*, int, int*, someArgs_t*, int, int);
int catcher(SOCKET, someArgs_t*, int*, int*, int, int);
#endif						/* ECHO_REQUEST_H */
