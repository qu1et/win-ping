#pragma once
#include <fcntl.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
														// структура описывающая список параметров, передаваемых в функции
typedef struct args
{
	int count;											// Параметр -n
	int packet_size;									// Параметр -l
	int timeout;										// Параметр -w
	char* target;										// Адрес, введенный пользователем
} optArgs;

someArgs_t* sb;											// Объявляем переменную типа структура
optArgs* StartArgs;										// Объявляем переменную типа структура
SOCKET sock;											// Переменная под сокет
int seq_no;												// Номер последовательности отправленного пакета
int start;												// Переменная для хранения времени отправки запроса
double percent;											// Процент потерянных пакетов
int loopArgs;											// Переменная для перебора аргументов
int req_timeouts;										// Количество реквест таймаутов подряд

void MyFinish(someArgs_t* sb, int i);										// сигнатура функции MyStart
int MyStart(int argc, char** argv, optArgs* StartArgs, someArgs_t* sb);		// сигнатура функции MyFinish