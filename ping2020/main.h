#pragma once
#include <fcntl.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
														// ��������� ����������� ������ ����������, ������������ � �������
typedef struct args
{
	int count;											// �������� -n
	int packet_size;									// �������� -l
	int timeout;										// �������� -w
	char* target;										// �����, ��������� �������������
} optArgs;

someArgs_t* sb;											// ��������� ���������� ���� ���������
optArgs* StartArgs;										// ��������� ���������� ���� ���������
SOCKET sock;											// ���������� ��� �����
int seq_no;												// ����� ������������������ ������������� ������
int start;												// ���������� ��� �������� ������� �������� �������
double percent;											// ������� ���������� �������
int loopArgs;											// ���������� ��� �������� ����������
int req_timeouts;										// ���������� ������� ��������� ������

void MyFinish(someArgs_t* sb, int i);										// ��������� ������� MyStart
int MyStart(int argc, char** argv, optArgs* StartArgs, someArgs_t* sb);		// ��������� ������� MyFinish