#ifndef DIAG_H
#define DIAG_H

/*										***��������� �������������, ����������� ��� ��������� ������������ ������.***			*/
#include <WinSock2.h>
#include <regex>
#include <sys/types.h>
#include "logger.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <ws2tcpip.h>
/*										***��������� ����������� ���������� ��������� ��� ��������� ������� ������.***			*/

										//�������� ������� (signature) ��� �����������
char* hostnameToIp(char*);
int AddIntoLogFile(someArgs_t*, char*, char*, char*);
int My_NODE_DIAGNOSTIC(char*, someArgs_t*);
int myECHODiagnostic(someArgs_t*);
int myLoggerInitialDiagnostic(someArgs_t*);
int MY_LOG_DIAGNOSTIC(someArgs_t*);


#endif									/* MY_DIAGNOZ*/
