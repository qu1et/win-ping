#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <sstream>
#include "Diag.h"
#include "ECHO_REQUEST.h"
#include <WinSock2.h>
#include <time.h>
#pragma comment(lib,"Ws2_32.lib")

USHORT ip_checksum(USHORT* buffer, int size)
{
													// ������� �������������� ����������� �����
													// USHORT* buffer - ����� ��� �������� �������������� checksum
													// int size - ������ ������
													// --- ���������� ���������� ---
	unsigned long cksum;							// ���������� ��� ����������� �����
													// --- ������������� ���������� ---
	cksum = 0;										// ������ ���������� 0

	while (size > 1) {								// ��������� ���, ��� ���� � ������
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size) {										// ���� ������ ������ ��������, ��������� ��������� ���� ������
		cksum += *(UCHAR*)buffer;
	}
													// ���������� ��������� ��������
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
													// ���������� �������� ����������� �����
	return (USHORT)(~cksum);

}

int echo_request(SOCKET* sock, char* ipv4, int seq_no, int* start, someArgs_t* sb, int packCount, int packSize)
{													// ������ ������� ECHO_REQUEST
													// ������� �������� ICMP Echo request
													// SOCKET sock - ����� ��� �������� ������
													// char* ipv4 - IPv4 ����� ���������� �������
													// int seq_no - ����� ������������������

													// DECLARATIONS
	SOCKADDR_IN destaddr;							// ��������� ������������ ����� ����������
	PICMPHDR pIcmpHdr;								// ��������� �� ��������� ICMP ���������
	int bwrote;										// ���������� ��� �������� ���������� ���������� ����
	char buf[512];									// ����� ��� ������ � ������ � ���
	WSADATA wsaData;								// ���������, ���������� ���������� � ������
													// INITIALITATIONS
	pIcmpHdr = NULL;								// ��������� �� ��������� ICMP ���������
	bwrote = 0;										// ���������� ��� �������� ���������� ���������� ����
	memset(&destaddr, 0, sizeof(destaddr));			// ����������� ������
	destaddr.sin_addr.S_un.S_addr = inet_addr(ipv4);// ������ �����
	destaddr.sin_family = AF_INET;					// ������ ������ IP (� ������ ������ IPv4)	
	pIcmpHdr = (PICMPHDR)malloc(packSize); 			// �������� ������ ��� ICMP ������
	pIcmpHdr->type = ICMP_ECHO_REQUEST;				// ������ ���
	pIcmpHdr->code = 0;								// ������ ���
	pIcmpHdr->checksum = 0;							// �������������� ���� ��� ����������� �����������
	pIcmpHdr->id = (USHORT)GetCurrentProcessId();	// ������ � �������� id ������������� �������� ��������
	pIcmpHdr->seq = seq_no;							// ������ ����� ������������������

	if (!packSize) { packSize = PACKET_SIZE; }		// ���� ������������ �� ���� ������ ������, �� ��������� ��������� ��������
	if (!packCount) { packCount = REP_NUMBER; }		// ���� ������������ �� ���� ���������� ��������, �� ��������� ��������� ��������

	sb->wsaErrorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

													// �������������� ����������� ����� � ������� �������
	pIcmpHdr->checksum = ip_checksum((USHORT*)pIcmpHdr, packSize);
	if (seq_no >= packCount)
	{
		return 2;									// ���������� ��� ������ �� �����
	}

													// �������� ICMP ������
	Sleep(1000);
	bwrote = sendto(*sock, (char*)pIcmpHdr, packSize, 0, (SOCKADDR*)&destaddr, sizeof(SOCKADDR_IN));

													// ���������� ����� �������� ������
	*start = clock();

	if (bwrote == SOCKET_ERROR)						// �������� �� ������
	{
		return 1;
	}

													// ���������� ������ � �����
	sprintf(buf, "���������� %i ������ �� %s;", bwrote, inet_ntoa(destaddr.sin_addr));

													// ������� �������� ������ � ���
	AddIntoLogFile(sb, buf, (char*)"INFO", NULL);
	AddIntoLogFile(sb, (char*)"ECHO ������ ���������", (char*)"INFO", NULL);
	sb->stats->sentPackets++;
	return 0;
}													// ����� ������� ECHO_REQUEST




int catcher(SOCKET sock, someArgs_t* sb, int *start, int *req_timeouts, int packSize, int timeout)
{	
													// ������ ������� CATCHER
													// ������� ��������� ECHO REPLY � ������� ���������� � ������
													// ���������:
													// SOCKET sock - �����, ������� �� �������

													// --- ���������� ���������� ---
	SOCKADDR_IN src; 								// ��������� ������ ����������� ������
	int fromlen;									// ������ ��������� ������
	pIPHeader pIPHdr;								// ��������� �� ��������� IP ���������
	PICMPHDR pICMPHdr;								// ��������� �� ��������� ICMP ���������
	int bread;										// ���-�� ��������� ����
	int timerecv;									// ����� ���������
	fd_set fdRead;									// �������� ����������

	if (!timeout) { timeout = DEFAULT_TIMEOUT; }	// ���� ������������ �� ���� �������, �� ��������� ��������� ��������
	if (!packSize) { packSize = PACKET_SIZE; }		// ���� ������������ �� ���� ������ ������, �� ��������� ��������� ��������

	timeval timeInterval = { 0, 0 };				// ��������� ��� �������� ���������� ���������
	timeInterval.tv_usec = timeout * 1000;			// ����� �������� � �������������
	int max_ip_packet_size = packSize + 512;		// ������������ ������ ������
	int nRoundTripTime = 0;							// ����� ����� �������� � �������

													// --- ������������ ���������� ---
	memset(&src, 0, sizeof(src)); 					// ����������� ������ ��� ������
	fromlen = sizeof(src);							// ������ ������
	pIPHdr = (pIPHeader)malloc(max_ip_packet_size);	// �������� ������ ��� ���������
	
	FD_ZERO(&fdRead);								// ������������� ������ ������������ ���������� NULL
	FD_SET(sock, &fdRead);							// ������ �����������

													// ��������� ������
	if ((bread = select(0, &fdRead, NULL, NULL, &timeInterval)) == SOCKET_ERROR) {
		return 1;
	}
	
	if (bread > 0 && FD_ISSET(sock, &fdRead)) {
		bread = recvfrom(sock, (char*)pIPHdr, max_ip_packet_size, 0, (SOCKADDR*)&src, &fromlen);
		if (bread == SOCKET_ERROR)					// �������� �� ������
		{
			return 1;
		}

		timerecv = clock();							// ���������� ����� ������ ������

		nRoundTripTime = timerecv - *start;			// ����� ����� ��������� � ���������� ������

		sprintf(sb->returnValue, "����� �� %s: ���������� ���� = %d, ����� = %d ��, TTL = %d", inet_ntoa(src.sin_addr),
			bread - sizeof(pIPHdr) - sizeof(pICMPHdr), nRoundTripTime, (int)pIPHdr->ttl);

		if (sb->stats->min == -1)
		{
			sb->stats->min = nRoundTripTime;
			sb->stats->max = nRoundTripTime;
		}
		else if (nRoundTripTime < sb->stats->min)
		{
			sb->stats->min = nRoundTripTime;
		}
		else if (nRoundTripTime > sb->stats->max)
		{
			sb->stats->max = nRoundTripTime;
		}

													// �������� ������ ��� ����������
		sb->stats->successPackets++;
		*req_timeouts = 0;
		sb->stats->avg = (sb->stats->max + sb->stats->min) / 2;

		printf("%s\n", sb->returnValue);			// ������� ���������� �� �����
	}
	else {
		++*req_timeouts;
		if (*req_timeouts == 2) {
			printf("Request timed out.\n");
			printf("�������� � �������� � �������\n");
			return 1;
		}

		sprintf(sb->returnValue, "Request timed out.");
		printf("%s\n", sb->returnValue);
		return 0;
	}
										
	return 0;
}													// ����� ������� CATCHER


