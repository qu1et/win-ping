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
													// Функция подсчитывающая контрольную сумму
													// USHORT* buffer - буфер для которого подсчитывается checksum
													// int size - размер буфера
													// --- Объявления переменных ---
	unsigned long cksum;							// Переменная для контрольной суммы
													// --- Инициализация переменных ---
	cksum = 0;										// Задаем изначально 0

	while (size > 1) {								// Суммируем все, что есть в буфере
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size) {										// Если размер буфера нечетный, добавляем последний байт буфера
		cksum += *(UCHAR*)buffer;
	}
													// Производим некоторые операции
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
													// Возвращаем значение контрольной суммы
	return (USHORT)(~cksum);

}

int echo_request(SOCKET* sock, char* ipv4, int seq_no, int* start, someArgs_t* sb, int packCount, int packSize)
{													// Начало функции ECHO_REQUEST
													// Функция посылает ICMP Echo request
													// SOCKET sock - сокет для передачи пакета
													// char* ipv4 - IPv4 адрес получателя запроса
													// int seq_no - номер последовательности

													// DECLARATIONS
	SOCKADDR_IN destaddr;							// Структура определяющая адрес получателя
	PICMPHDR pIcmpHdr;								// Указатель на структуру ICMP заголовка
	int bwrote;										// Переменная для хранения количества переданных байт
	char buf[512];									// Буфер для вывода и записи в лог
	WSADATA wsaData;								// Структура, содержащая информацию о сокете
													// INITIALITATIONS
	pIcmpHdr = NULL;								// Указатель на структуру ICMP заголовка
	bwrote = 0;										// Переменная для хранения количества переданных байт
	memset(&destaddr, 0, sizeof(destaddr));			// Освобождаем память
	destaddr.sin_addr.S_un.S_addr = inet_addr(ipv4);// Задаем адрес
	destaddr.sin_family = AF_INET;					// Задаем версию IP (в данном случае IPv4)	
	pIcmpHdr = (PICMPHDR)malloc(packSize); 			// Выделяем память для ICMP пакета
	pIcmpHdr->type = ICMP_ECHO_REQUEST;				// Задаем тип
	pIcmpHdr->code = 0;								// Задаем код
	pIcmpHdr->checksum = 0;							// Инициализируем нулём для последующих манипуляций
	pIcmpHdr->id = (USHORT)GetCurrentProcessId();	// Задаем в качестве id идентификатор текущего процесса
	pIcmpHdr->seq = seq_no;							// Задаем номер последовательности

	if (!packSize) { packSize = PACKET_SIZE; }		// Если пользователь не ввел размер пакета, то присвоить дефолтное значение
	if (!packCount) { packCount = REP_NUMBER; }		// Если пользователь не ввел количество запросов, то присвоить дефолтное значение

	sb->wsaErrorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

													// Инициализируем контрольную сумму с помощью функции
	pIcmpHdr->checksum = ip_checksum((USHORT*)pIcmpHdr, packSize);
	if (seq_no >= packCount)
	{
		return 2;									// Возвращаем код выхода из цикла
	}

													// Отправка ICMP пакета
	Sleep(1000);
	bwrote = sendto(*sock, (char*)pIcmpHdr, packSize, 0, (SOCKADDR*)&destaddr, sizeof(SOCKADDR_IN));

													// Записываем время отправки пакета
	*start = clock();

	if (bwrote == SOCKET_ERROR)						// Проверка на ошибку
	{
		return 1;
	}

													// Записываем данные в буфер
	sprintf(buf, "Отправлено %i байтов на %s;", bwrote, inet_ntoa(destaddr.sin_addr));

													// Попытка записать данные в лог
	AddIntoLogFile(sb, buf, (char*)"INFO", NULL);
	AddIntoLogFile(sb, (char*)"ECHO запрос отправлен", (char*)"INFO", NULL);
	sb->stats->sentPackets++;
	return 0;
}													// Конец функции ECHO_REQUEST




int catcher(SOCKET sock, someArgs_t* sb, int *start, int *req_timeouts, int packSize, int timeout)
{	
													// Начало функции CATCHER
													// Функция принимает ECHO REPLY и выводит информацию о пакете
													// Параметры:
													// SOCKET sock - сокет, который мы слушаем

													// --- Объявления переменных ---
	SOCKADDR_IN src; 								// Структура адреса отправителя пакета
	int fromlen;									// Размер структуры адреса
	pIPHeader pIPHdr;								// Указатель на структуру IP заголовка
	PICMPHDR pICMPHdr;								// Указатель на структуру ICMP заголовка
	int bread;										// Кол-во считанных байт
	int timerecv;									// Время получения
	fd_set fdRead;									// Файловый дескриптор

	if (!timeout) { timeout = DEFAULT_TIMEOUT; }	// Если пользователь не ввел таймаут, то присвоить дефолтное значение
	if (!packSize) { packSize = PACKET_SIZE; }		// Если пользователь не ввел размер пакета, то присвоить дефолтное значение

	timeval timeInterval = { 0, 0 };				// Структура для указания временного интервала
	timeInterval.tv_usec = timeout * 1000;			// Время ожидания в микросекундах
	int max_ip_packet_size = packSize + 512;		// Максимальный размер пакета
	int nRoundTripTime = 0;							// Время между запросом и ответом

													// --- Иницилизации переменных ---
	memset(&src, 0, sizeof(src)); 					// Освобождаем память для адреса
	fromlen = sizeof(src);							// Задаем размер
	pIPHdr = (pIPHeader)malloc(max_ip_packet_size);	// Выделяем память под заголовок
	
	FD_ZERO(&fdRead);								// Инициализация набора дескрипторов установкой NULL
	FD_SET(sock, &fdRead);							// Запись дескриптора

													// Считываем данные
	if ((bread = select(0, &fdRead, NULL, NULL, &timeInterval)) == SOCKET_ERROR) {
		return 1;
	}
	
	if (bread > 0 && FD_ISSET(sock, &fdRead)) {
		bread = recvfrom(sock, (char*)pIPHdr, max_ip_packet_size, 0, (SOCKADDR*)&src, &fromlen);
		if (bread == SOCKET_ERROR)					// Проверка на ошибку
		{
			return 1;
		}

		timerecv = clock();							// Записываем время приема данных

		nRoundTripTime = timerecv - *start;			// время между отправкой и получением пакета

		sprintf(sb->returnValue, "Ответ от %s: количество байт = %d, время = %d мс, TTL = %d", inet_ntoa(src.sin_addr),
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

													// Собираем данные для статистики
		sb->stats->successPackets++;
		*req_timeouts = 0;
		sb->stats->avg = (sb->stats->max + sb->stats->min) / 2;

		printf("%s\n", sb->returnValue);			// Выводим статистику на экран
	}
	else {
		++*req_timeouts;
		if (*req_timeouts == 2) {
			printf("Request timed out.\n");
			printf("Проблемы с доступом к серверу\n");
			return 1;
		}

		sprintf(sb->returnValue, "Request timed out.");
		printf("%s\n", sb->returnValue);
		return 0;
	}
										
	return 0;
}													// Конец функции CATCHER


