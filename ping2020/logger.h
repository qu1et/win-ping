#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
/*�����������������*/
#define ERROR_INIT_LOG   -12
#define BAD_MESSAGE      -13
#define SUCCESS           0

															// ��������� ����������� ������ ���������� ����������
typedef struct statInfo
{
	int min;												// ����������� �������� rtt
	int max;												// ������������ �������� rtt
	double avg;												// ������� �������� rtt
	int sentPackets;										// ���������� �������
	int successPackets;										// ������� ����������
} statInfo;

															// ��������� ��� �������/�������� ���������� �������� �� ������� ������

typedef struct someArgs {
	FILE* LogFile;											// ���������� ��� �������� ����� ����
	int fd_RW;												// ��� �������� ����� ������/������ ����� ����
	char* LogFileName;										// ��� ���-�����
	char* message;											// ��������� ��� ������ � ���
	char returnValue[512];									// ������������ �������� ��������
	int wsaErrorCode;										// �������� ������ wsa
	int logErrorCode;										// �������� ������ ������ � ���-����
	char* messageType;										// ��� ���������, ������������ � ��� (INFO, ERROR)
	statInfo* stats;										// ��������� ��� ����������
	char* dstAddress;										// ������� ����� ��� �������� �������
}someArgs_t;
															/*���������*/

															// �������� ������� (signature) ������
void IsExistLog(someArgs_t*, long*, int*);
int print_log(someArgs_t*);
int log_info(someArgs_t*);
int initLog(someArgs_t*);


#endif														/* LOGGER_H */