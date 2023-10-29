#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
/*макроопределелния*/
#define ERROR_INIT_LOG   -12
#define BAD_MESSAGE      -13
#define SUCCESS           0

															// Структура описывающая список параметров статистики
typedef struct statInfo
{
	int min;												// Минимальное значение rtt
	int max;												// Максимальное значение rtt
	double avg;												// Среднее значение rtt
	int sentPackets;										// Отправлено пакетов
	int successPackets;										// Пакетов доставлено
} statInfo;

															// Структура для предачи/возврата нескольких значений из функций логера

typedef struct someArgs {
	FILE* LogFile;											// Переменная для создания файла лога
	int fd_RW;												// Код возврата после записи/чтения файла лога
	char* LogFileName;										// Имя лог-файла
	char* message;											// Сообщение для записи в лог
	char returnValue[512];									// Возвращаемое функцией значение
	int wsaErrorCode;										// Значение ошибки wsa
	int logErrorCode;										// Значение ошибки записи в лог-файл
	char* messageType;										// Тип сообщения, добавляемого в лог (INFO, ERROR)
	statInfo* stats;										// Структура для статистики
	char* dstAddress;										// Целевой адрес для отправки пакетов
}someArgs_t;
															/*Константы*/

															// Описание функций (signature) логера
void IsExistLog(someArgs_t*, long*, int*);
int print_log(someArgs_t*);
int log_info(someArgs_t*);
int initLog(someArgs_t*);


#endif														/* LOGGER_H */