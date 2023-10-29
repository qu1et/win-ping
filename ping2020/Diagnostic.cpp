#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Diag.h"
#include <stdlib.h>

using namespace std;

std::regex IS_HOSTNAME("^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$");
std::regex IS_IP("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");

char* hostnameToIp(char* hostname)

										/*						**ОПИСАНИЕ ФУНКЦИИ hostnameToIp**
											*Функция hostnameToIp осуществляет преобразование Имени Узла в IP-адрес.
											*В качестве входных параметров функция принимает строку адреса.
										*/

{																//Begin hostnameToIp
																/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
	struct addrinfo hints;
	struct addrinfo* res;								        // Указатель на результат преобразования
	struct in_addr addr;

	memset(&hints, 0, sizeof(hints));							// Пустая структура для результата преобразования
	hints.ai_socktype = SOCK_STREAM;							// TCP stream-sockets;
	hints.ai_family = AF_INET;									// Обрабатываем только адреса IPv4

	if ((getaddrinfo(hostname, NULL, &hints, &res)) != 0)       // Если в процессе преобразования произошла ошибка
	{
		return 0;
	}

	addr.S_un = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.S_un;

	freeaddrinfo(res);										    // Освобождаем память, занимаемую структурой результата

	return inet_ntoa(addr);										// Преобразуем IP в строку и возвращаем её;
}


int AddIntoLogFile(someArgs_t* sb, char* Message, char* messageType, char* threadReturnValue)
					/*				**Процедура для инициализации и диагностики добавления данных в лог-файл**
						*Осуществляет инициализацию полей структуры (sb), передаваемой в функцию log_info, производящую запись в файл,
						а также обрабатывает значение возврата функции, осуществляющей запись.
						*В качестве параметров получает набор аргументов (sb), строку с сообщением, строку с типом сообщения
						и указатель на возвращаемое значение.
					*/
{																	//Begin AddIntoLogFile

																	/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
																	/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (INITIALIZATION)*/
	sb->message = Message;											// Устанавливаем содержимое сообщения;
	sb->messageType = messageType;									// Устанавливаем тип сообщение;

	if (!log_info(sb))												// Передаем сформированный набор параметров (sb) в функцию записи в файл
	{
		return 0; 													// Если функция отработала успешно, возвращаем 0;
	}
	else
	{
		return 1; 													// Иначе, возвращаем 1;
	}
}																	// End AddIntoLogFile

int My_NODE_DIAGNOSTIC(char* target, someArgs_t* sb)
							/*		**Функция, осуществляющая диагностику входной строки, хранящей адрес или имя Хоста**
									*В качестве параметров получает набор аргументов (sb) и строку для диагностики.
							*/				
{																	// Begin My_NODE_DIAGNOSTIC
																	/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
	char* result;													// Переменная для IP-адреса
	WSADATA wsaData;												// Структура, содержащая информацию о сокете
																	/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (INITIALIZATION)*/
	result = NULL;
																	/*Анализ входной строки. */
	if (!regex_match(target, IS_IP))								// Если входная строка не соответствует паттерну Ip-адреса;
	{
		if (!regex_match(target, IS_HOSTNAME))						// Если входная строка не соответствует паттерну Hostname,
		{															
			AddIntoLogFile(sb, (char*)"Введенные данные неверны", (char*)"ERROR", NULL);	// Запись в лог информации об ошибке;
			return 1;												// Возвращаем 0;
		}


		else														// Если входная строка соответствует Hostname;
		{
			sb->wsaErrorCode = WSAStartup(MAKEWORD(2, 0), &wsaData);
			result = hostnameToIp(target);							// Получаем IP-адрес по Hostname
			if (result)												// Если преобразование Hostname прошло успешно;	
			{
				AddIntoLogFile(sb, (char*)"Трансформация адреса прошла успешно.",
					(char*)"INFO", NULL);							// Запись в лог информации;
				AddIntoLogFile(sb, (char*)"Введенные параметры записаны", (char*)"INFO", NULL);
				sb->dstAddress = result;							// Сохраняем полученный Ip;
				printf("\nОбмен пакетами с [%s]\n", result);		
				return 0;											// Возвращаем 1;
			}
			else													// Иначе, если преобразование не удалось;
			{
				AddIntoLogFile(sb, (char*)"Ошибка трансформации адреса.",
					(char*)"ERROR", NULL);							// Записываем в лог информацию об ошибке;
				return 1;											// Возвращаем 0;
			}
		}
	}
	/*Иначе, входная строка соответствует паттерну Ip-адреса;*/
	sb->dstAddress = target;										// Сохраняем Ip;
	printf("\nОбмен пакетами с [%s]\n", target);
	return 0;														// Возвращаем 1;
}																	// End My_NODE_DIAGNOSTIC

int myLoggerInitialDiagnostic(someArgs_t* sb)
/*											***	ОПРЕДЕЛЕНИЕ ФУНКЦИИ myLoggerInitialDiagnostic ***
		* Функция myLoggerInitialDiagnostic предназначена для начальной диагностики состояния логгера.

		*В качестве начальных входных параметров функция получает указатель (sb) на набор аргументов для записи в лог,
		*и указатель (logFile) на имя лог-файла.
*/
{																	// Begin myLoggerInitialDiagnostic

	if (sb->fd_RW == -1)											// Если не открылся файл
	{
		printf("\n open() failed with error [%s]\n", strerror(errno));
		return 0;
	}
}																	// End myLoggerInitialDiagnostic

int myECHODiagnostic(someArgs_t* sb)
/*											***	ОПРЕДЕЛЕНИЕ ФУНКЦИИ myECHODiagnostic ***
		* Функция myECHODiagnostic предназначена для начальной диагностики кодов возврата на сокет.

		*В качестве начальных входных параметров функция получает указатель (sb) на значение ошибки при инициализации сокета
*/
{																	// Begin myECHODiagnostic
	switch (sb->wsaErrorCode)
	{
	case 0:															
		printf("Ошибка инициализации сокета: %u\n", WSAGetLastError());
		return 1;													// Возвращаем ошибку			
	case WSASYSNOTREADY:											
	{
		printf("ERROR: Сетевая подсистема не готова!\n");
		WSACleanup();
		return 1;
	}
	case WSAVERNOTSUPPORTED:										
		printf("ERROR: Winsock 2.0+ не поддерживается!\n");
		WSACleanup();
		return 1;
	case WSAEINPROGRESS:											
		printf("ERROR: Обнаружена блокировка!\n");
		WSACleanup();
		return 1;
	case WSAEPROCLIM:												
		printf("ERROR: Достигнуть лимит процессов\n");
		WSACleanup();
		return 1;
	}
	return 0;
}																	// End myECHODiagnostic

int MY_LOG_DIAGNOSTIC(someArgs_t* sb)
						/*						***	ОПРЕДЕЛЕНИЕ ФУНКЦИИ MY_LOG_DIAGNOSTIC ***
							* Функция MY_LOG_DIAGNOSTIC предназначена для диагностики кодов возврата при чтении/записи лог-файла
							*В качестве начальных входных параметров функция получает указатель (sb) на значение ошибки при записи/чтении
						*/
{
	switch (sb->logErrorCode)
	{
	case EPERM:
		printf("ERROR: Операция не разрешена\n");
		return 1;
	case ENOENT: 
		printf("ERROR: Отсутствует такой файл или каталог\n");
		return 1;
	case ESRCH:
		printf("ERROR: Отсутствует такой процесс\n");
		return 1;
	case EINTR:
		printf("ERROR: Прервана функция\n");
		return 1;
	case EIO:
		printf("ERROR: Ошибка ввода-вывода\n");
		return 1;
	case EBADF:
		printf("ERROR: Неверный номер файла\n");
		return 1;
	case EAGAIN:
		printf("ERROR: Больше процессов нет или недостаточно памяти, или достигнут максимальный уровень вложенности\n");
		return 1;
	case ENOMEM:
		printf("ERROR: Недостаточно памяти\n");
		return 1;
	}
	return 0;
}