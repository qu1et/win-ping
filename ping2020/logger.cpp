#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
/***��������� �������������, ����������� ��� ��������� ������������ ������.***/
#include "Diag.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>


void IsExistLog(someArgs_t* sb, long* Filesize, int* exist)
/*								**������� �������� �������� ������� ���-�����**
					*� �������� ���������� �������� ����� ���������� (sb).
*/
{													// Begin IsExistLog
													/*�������� ����������*/
	struct stat st;									// ��������� ��������� ���  ��������/��������� ���������� � �����

													/*����������� ���������� (Initialization)*/
	*exist = stat(sb->LogFileName, &st);			// ����������� � �� ���������� � ����� � ��������� st
	if (!*exist)
	{
		*Filesize = st.st_size;						// �������� �� ��������� ������ ����. � ������� ����� � ���������� � ���������� 
	}

}													// End IsExistLog

int initLog(someArgs_t* sb)
{
													/*�������� ����������*/
	long Filesize;									// ������ ���-�����
	int exist;										// ������ �� ���-����
	int Overflow;									// ������������ ���-�����
	long �LogSIZE;									// ������ ������� ���-�����

													/*����������� ���������� (Initialization)*/
	�LogSIZE = 8388608;								// ������� ������ ������� ���-����� � ������
	exist = 0;										// ������ �� ���-����
	Filesize = 0;									// ������ ���-�����
	Overflow = 0;									// ������������ ���-�����

	IsExistLog(sb, &Filesize, &exist);				// ���������, ���������� �� ���-���� � ����. ������
	sb->fd_RW = _open(sb->LogFileName, O_RDWR | O_APPEND, 0);	// �������� ������� ���-����

	if (Filesize >= �LogSIZE)						// ���������� ���������� �������� � ���������� max size
	{
		Overflow = 1;
	}
	else
	{
		Overflow = 0;
	}

	if (exist)										// ���� ���� �� ����������
	{
													// �������� ������� ����
		if ((sb->fd_RW = _creat(sb->LogFileName, _S_IREAD | _S_IWRITE) == -1))
		{
			fprintf(stderr, "IsExistLog: ���������� ������� ���� '%s'\n",
				sb->LogFileName);
			sb->logErrorCode = errno;
			return 1;
		}
	}
	else if (Overflow)								// ���� ���-���� "����������"
	{
		_close(sb->fd_RW);							// ���������, ������ � ��������� �����
		if ((sb->fd_RW = _open(sb->LogFileName, O_RDWR | O_TRUNC | O_APPEND, 0)) == -1)
		{
			fprintf(stderr, "IsExistLog: ���������� �������� ���� '%s'\n",
				sb->LogFileName);
			sb->logErrorCode = errno;
			return 1;
		}
	}
	else if (sb->fd_RW == -1)						// ���� ���� �� �������� �� �����-�� ��������
	{
		fprintf(stderr, "IsExistLog: ���������� ������� ���� '%s'\n",
			sb->LogFileName);
		sb->logErrorCode = errno;
		return 1;
	}

	if (AddIntoLogFile(sb, (char*)"��������� ��������\n", (char*)"INFO", NULL))
	{
		MY_LOG_DIAGNOSTIC(sb);
	}
	
	return 0;
}

int print_log(someArgs_t* sb)
								/*								**������� ������ ���� �� �����**
													*� �������� ���������� �������� ���� ���������� (sb).
								*/
{													// Begin print_log

													/*�������� ����������*/

	char* line;										// ��������� ���������� ��� �������� ��������� ������ �� �����
	size_t read_bytes;
													/*����������� ���������*/
	size_t len;										// ����. ����� ����������� ������

													/*����������� ���������� (Initialization)*/
	len = 512;
	read_bytes = 0;
	line = (char*)calloc(len, sizeof(char));		// ��������� ������ ��� 512 ��������

	if (!sb->fd_RW)
	{
		sb->logErrorCode = errno;
		return 1;
	}

	_lseek(sb->fd_RW, 0, SEEK_SET);					// ��������� � ������ �����
	while ((read_bytes = _read(sb->fd_RW, line, len)) > 0)
	{
		line[read_bytes] = 0;
		printf("%s", line);							// ������� ��������� �� �����
	}

	MY_LOG_DIAGNOSTIC(sb);

	line = NULL;
	return 0;										// ���������� 0
}													// End prin_log


int log_info(someArgs_t* sb)
					/*								**������� ��� ������ ������ � ���-����**
						*��������� "������" �� ���������� ���������� � ������������ ������ �� � ����.
						*� �������� ���������� �������� ���� ���������� (sb).
					*/
{													// Begin log_info

													/*�������� ����������*/
	char buf[512];									// ����� ��� ������������ ������
	time_t now;										// ���������� ��� �������� �������
	char* date;										// ���������� ��� �������� ����

													/*����������� ���������� (Initialization)*/
	date = NULL;

	if (!sb->fd_RW)									// ���� ���-���� �� ��������
	{
		sb->logErrorCode = errno;
		return 1;									// ���������� 1
	}												
	time(&now);										// �������� ������� �����
	date = ctime(&now);								// �������� ������� ����
	date[strlen(date) - 1] = '\0';					// ���������� ������ ����� ������ 

	sprintf(buf, "%s [%s] %s \n", date,				// ��������� ������ ��� ������ � ���
		sb->messageType, sb->message);

	sb->logErrorCode = _write(sb->fd_RW, buf, (unsigned)strlen(buf));
	return 0;										//	���������� 0
}													//end log_info
