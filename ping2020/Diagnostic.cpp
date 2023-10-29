#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Diag.h"
#include <stdlib.h>

using namespace std;

std::regex IS_HOSTNAME("^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$");
std::regex IS_IP("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");

char* hostnameToIp(char* hostname)

										/*						**�������� ������� hostnameToIp**
											*������� hostnameToIp ������������ �������������� ����� ���� � IP-�����.
											*� �������� ������� ���������� ������� ��������� ������ ������.
										*/

{																//Begin hostnameToIp
																/*�������� ����������*/
	struct addrinfo hints;
	struct addrinfo* res;								        // ��������� �� ��������� ��������������
	struct in_addr addr;

	memset(&hints, 0, sizeof(hints));							// ������ ��������� ��� ���������� ��������������
	hints.ai_socktype = SOCK_STREAM;							// TCP stream-sockets;
	hints.ai_family = AF_INET;									// ������������ ������ ������ IPv4

	if ((getaddrinfo(hostname, NULL, &hints, &res)) != 0)       // ���� � �������� �������������� ��������� ������
	{
		return 0;
	}

	addr.S_un = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.S_un;

	freeaddrinfo(res);										    // ����������� ������, ���������� ���������� ����������

	return inet_ntoa(addr);										// ����������� IP � ������ � ���������� �;
}


int AddIntoLogFile(someArgs_t* sb, char* Message, char* messageType, char* threadReturnValue)
					/*				**��������� ��� ������������� � ����������� ���������� ������ � ���-����**
						*������������ ������������� ����� ��������� (sb), ������������ � ������� log_info, ������������ ������ � ����,
						� ����� ������������ �������� �������� �������, �������������� ������.
						*� �������� ���������� �������� ����� ���������� (sb), ������ � ����������, ������ � ����� ���������
						� ��������� �� ������������ ��������.
					*/
{																	//Begin AddIntoLogFile

																	/*�������� ����������*/
																	/*����������� ���������� (INITIALIZATION)*/
	sb->message = Message;											// ������������� ���������� ���������;
	sb->messageType = messageType;									// ������������� ��� ���������;

	if (!log_info(sb))												// �������� �������������� ����� ���������� (sb) � ������� ������ � ����
	{
		return 0; 													// ���� ������� ���������� �������, ���������� 0;
	}
	else
	{
		return 1; 													// �����, ���������� 1;
	}
}																	// End AddIntoLogFile

int My_NODE_DIAGNOSTIC(char* target, someArgs_t* sb)
							/*		**�������, �������������� ����������� ������� ������, �������� ����� ��� ��� �����**
									*� �������� ���������� �������� ����� ���������� (sb) � ������ ��� �����������.
							*/				
{																	// Begin My_NODE_DIAGNOSTIC
																	/*�������� ����������*/
	char* result;													// ���������� ��� IP-������
	WSADATA wsaData;												// ���������, ���������� ���������� � ������
																	/*����������� ���������� (INITIALIZATION)*/
	result = NULL;
																	/*������ ������� ������. */
	if (!regex_match(target, IS_IP))								// ���� ������� ������ �� ������������� �������� Ip-������;
	{
		if (!regex_match(target, IS_HOSTNAME))						// ���� ������� ������ �� ������������� �������� Hostname,
		{															
			AddIntoLogFile(sb, (char*)"��������� ������ �������", (char*)"ERROR", NULL);	// ������ � ��� ���������� �� ������;
			return 1;												// ���������� 0;
		}


		else														// ���� ������� ������ ������������� Hostname;
		{
			sb->wsaErrorCode = WSAStartup(MAKEWORD(2, 0), &wsaData);
			result = hostnameToIp(target);							// �������� IP-����� �� Hostname
			if (result)												// ���� �������������� Hostname ������ �������;	
			{
				AddIntoLogFile(sb, (char*)"������������� ������ ������ �������.",
					(char*)"INFO", NULL);							// ������ � ��� ����������;
				AddIntoLogFile(sb, (char*)"��������� ��������� ��������", (char*)"INFO", NULL);
				sb->dstAddress = result;							// ��������� ���������� Ip;
				printf("\n����� �������� � [%s]\n", result);		
				return 0;											// ���������� 1;
			}
			else													// �����, ���� �������������� �� �������;
			{
				AddIntoLogFile(sb, (char*)"������ ������������� ������.",
					(char*)"ERROR", NULL);							// ���������� � ��� ���������� �� ������;
				return 1;											// ���������� 0;
			}
		}
	}
	/*�����, ������� ������ ������������� �������� Ip-������;*/
	sb->dstAddress = target;										// ��������� Ip;
	printf("\n����� �������� � [%s]\n", target);
	return 0;														// ���������� 1;
}																	// End My_NODE_DIAGNOSTIC

int myLoggerInitialDiagnostic(someArgs_t* sb)
/*											***	����������� ������� myLoggerInitialDiagnostic ***
		* ������� myLoggerInitialDiagnostic ������������� ��� ��������� ����������� ��������� �������.

		*� �������� ��������� ������� ���������� ������� �������� ��������� (sb) �� ����� ���������� ��� ������ � ���,
		*� ��������� (logFile) �� ��� ���-�����.
*/
{																	// Begin myLoggerInitialDiagnostic

	if (sb->fd_RW == -1)											// ���� �� �������� ����
	{
		printf("\n open() failed with error [%s]\n", strerror(errno));
		return 0;
	}
}																	// End myLoggerInitialDiagnostic

int myECHODiagnostic(someArgs_t* sb)
/*											***	����������� ������� myECHODiagnostic ***
		* ������� myECHODiagnostic ������������� ��� ��������� ����������� ����� �������� �� �����.

		*� �������� ��������� ������� ���������� ������� �������� ��������� (sb) �� �������� ������ ��� ������������� ������
*/
{																	// Begin myECHODiagnostic
	switch (sb->wsaErrorCode)
	{
	case 0:															
		printf("������ ������������� ������: %u\n", WSAGetLastError());
		return 1;													// ���������� ������			
	case WSASYSNOTREADY:											
	{
		printf("ERROR: ������� ���������� �� ������!\n");
		WSACleanup();
		return 1;
	}
	case WSAVERNOTSUPPORTED:										
		printf("ERROR: Winsock 2.0+ �� ��������������!\n");
		WSACleanup();
		return 1;
	case WSAEINPROGRESS:											
		printf("ERROR: ���������� ����������!\n");
		WSACleanup();
		return 1;
	case WSAEPROCLIM:												
		printf("ERROR: ���������� ����� ���������\n");
		WSACleanup();
		return 1;
	}
	return 0;
}																	// End myECHODiagnostic

int MY_LOG_DIAGNOSTIC(someArgs_t* sb)
						/*						***	����������� ������� MY_LOG_DIAGNOSTIC ***
							* ������� MY_LOG_DIAGNOSTIC ������������� ��� ����������� ����� �������� ��� ������/������ ���-�����
							*� �������� ��������� ������� ���������� ������� �������� ��������� (sb) �� �������� ������ ��� ������/������
						*/
{
	switch (sb->logErrorCode)
	{
	case EPERM:
		printf("ERROR: �������� �� ���������\n");
		return 1;
	case ENOENT: 
		printf("ERROR: ����������� ����� ���� ��� �������\n");
		return 1;
	case ESRCH:
		printf("ERROR: ����������� ����� �������\n");
		return 1;
	case EINTR:
		printf("ERROR: �������� �������\n");
		return 1;
	case EIO:
		printf("ERROR: ������ �����-������\n");
		return 1;
	case EBADF:
		printf("ERROR: �������� ����� �����\n");
		return 1;
	case EAGAIN:
		printf("ERROR: ������ ��������� ��� ��� ������������ ������, ��� ��������� ������������ ������� �����������\n");
		return 1;
	case ENOMEM:
		printf("ERROR: ������������ ������\n");
		return 1;
	}
	return 0;
}