/*										***Директивы препроцессора, необходимые для включения заголовочных файлов.***			*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Diag.h"										// Содержит описание функций (signature) и процедур и структур для диагностики.
#include "ECHO_REQUEST.h"								// Содержит описание функций (signature) и процедур для ECHO_REQUEST.
#include "logger.h"										// Содержит описание функций (signature) и процедур для логгера
#include "main.h"										// Содержит описание функций (signature) и процедур для main

int main(int argc, char* argv[])
/*										***	ОПРЕДЕЛЕНИЕ ФУНКЦИИ MAIN ***
		* Функция main является точкой входа в программу и осуществляет вызов функций, необходимых для работы утилиты.

		*В качестве начальных входных параметров функция main получает набор аргументов argc и argv,
		* где argc - кол-во переданных параметров; argv-массив входных параметров, введенных пользователяем.
		*Пользователь может вводить любое кол-во параметров запуска, но корректно обрабатываться будет только последовательность от 2 до 9 параметров.

		*Программа запускается в формате: MyPing [-n number_of_repetitions] [-l packet_size] [-w timeout] <-a Ip-address OR HostName> ");
		** ПРИМЕР**:	ping2020 -n 3 -l 64 -w 5 -a yandex.ru
		**где argc=6;
		**Содержимое argv:
		argv[0] - ping
		argv[1] - -n
		argv[2] - 3
		argv[3] - -l
		argv[4] - 64
		argv[5] - -w
		argv[6] - 5
		argv[7] - -a
		argv[8] - yandex.ru
		** END ПРИМЕР**:
*/

{															// Begin MAIN
	SetConsoleOutputCP(1251);								// Устанавливаем кодировку в для вывода в консоль

															/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/

	sb = NULL;												// Переменная типа структура для хранения значений для логера, echo и диагностики
	StartArgs = NULL;										// Переменная типа структура для хранения введенных параметров
	sock = NULL;											// Переменная под сокет
	seq_no = 0;												// Переменная, хранящая количество отправленных пакетов
	start = 0;												// Переменная, хранящая время отправки пакета
	req_timeouts = 0;										// Количество реквест таймаутов подряд
	sb = (someArgs_t*)malloc(sizeof * sb);					// Выделяем память под структуру значений для логера
	if (sb == NULL) exit(-1);								// Если не выделилась, завершаем программу 
	sb->stats = (statInfo*)malloc(sizeof * sb->stats);		// Выделяем память под структуру значений для информации о передаче пакетов
	if (sb->stats == NULL) exit(-1);						// Если не выделилась, завершаем программу 

	sb->LogFileName = (char*)"LogFile.txt";					// Переменная с именем лог-файла
	sb->message = NULL;										// Переменная для сообщения для записи в лог
	sb->messageType = NULL;									// Переменная для типа сообщения для записи в лог (INFO, ERROR)

	sb->stats->max = 0;										// Переменная, хранящая максимальное время роутинга пакета
	sb->stats->min = -1;									// Переменная, хранящая минимальное время роутинга пакета
	sb->stats->avg = NULL;									// Переменная, хранящая среднее время роутинга пакета
	sb->stats->successPackets = 0;							// Переменная, хранящая количество принятых пакетов
	sb->stats->sentPackets = 0;								// Переменная, хранящая количество отправленных пакетов

	StartArgs = (optArgs*)malloc(sizeof * StartArgs);		// Выделяем память под структуру значений для логера
	if (StartArgs == NULL) exit(-1);						// Если не выделилась, завершаем программу 

	StartArgs->count = NULL;								// Переменная, хранящая количество запросов
	StartArgs->packet_size = NULL;							// Переменная, хранящая размер пакета
	StartArgs->timeout = NULL;								// Переменная, хранящая время ожидания ответа
	StartArgs->target = NULL;								// Переменная, хранящая доменное имя или IP-адрес 

	switch (MyStart(argc, argv, StartArgs, sb))				// Вызов функции MyStart, осуществляющей проверку кол-ва входных параметров.
	{
	case 1:
		MyFinish(sb, -1);
		break;

	case 0:
	{
		switch (initLog(sb))
		{
		case 1:
			myLoggerInitialDiagnostic(sb);					// Вызываем диагностуку
			MyFinish(sb, -1);
			break;
		case 0:

															// Вызов функции диагностики входных данных (СЕМАНТИЧЕСКИЙ АНАЛИЗ входных параметров).
			switch (My_NODE_DIAGNOSTIC(StartArgs->target, sb))
			{
			case 1:
				if (print_log(sb))
				{
					MY_LOG_DIAGNOSTIC(sb);
				}
				MyFinish(sb, 0);
				break;

			case 0:
			{
				while (true)								// Цикл пока не достигли уст. знач.
				{
					switch (echo_request(&sock, sb->dstAddress, seq_no, &start, sb, StartArgs->count, StartArgs->packet_size))
					{
					case 1:
						if (!myECHODiagnostic(sb))
						{
							if (print_log(sb))
							{
								MY_LOG_DIAGNOSTIC(sb);
							}
						}
						MyFinish(sb, -2);
						break;

					case 2:
						MyFinish(sb, 0);
						break;

					case 0:
					{
						seq_no++;
						if (catcher(sock, sb, &start, &req_timeouts, StartArgs->packet_size, StartArgs->timeout))
						{
							if (!myECHODiagnostic(sb))
							{
								if (print_log(sb))
								{
									MY_LOG_DIAGNOSTIC(sb);
								}
							}
							MyFinish(sb, -2);
						}
						AddIntoLogFile(sb, sb->returnValue, (char*)"INFO", NULL);		// Пытаемся записать данные в лог
						break;
					}

					}
				}
			}
			}
		}
		break;
	}

	}
}


void MyFinish(someArgs_t* sb, int i)
{																						//BEGIN MyFinish
													// Процедура производит завершение программы и вывод статистике о времени
													// someArgs_t *sb - указатель на процедуру, хранящую данные о логе

																						/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/
	percent = (sb->stats->sentPackets - sb->stats->successPackets) * 100 / sb->stats->sentPackets;
																						// --- Основная работа процедуры ---												

	WSACleanup();																		// Освобождаем память из под библиотеки
	initLog(sb);
	AddIntoLogFile(sb, (char*)"Конец программы\n", (char*)"INFO", NULL);					// Добавляем информацию о завершении в лог
	_close(sb->fd_RW);																	// Закрываем лог

																						// --- Вывод статистики ---

	if (i != -2)																		// Если ошибка сокета, не выводим стату по пакетам
	{
		printf("\nСтатистика Ping:\n");
		printf("\tПакетов: Отправлено = %d; Получено = %d; Потеряно = %d\n", sb->stats->sentPackets, sb->stats->successPackets, sb->stats->sentPackets - sb->stats->successPackets);
		printf("\t(%.0f %% потерь)\n", percent);
		printf("Приблизительное время приема-передачи в мс:\n");
		printf("\tМинимальное = %d, Максимальное = %d, Среднее = %.0f\n", sb->stats->min, sb->stats->max, sb->stats->avg);
	}
	exit(0);																			// Завершаем программу
}																						// END MyFinish

//									//проверяем сколько агрументов нам подали на вход
int MyStart(int argc, char** argv, optArgs* StartArgs, someArgs_t* sb)
{
													// Процедура проверяет входные аргументы и записывает их в структуру для последующего использования
													// int argc			  - количество аргументов
													// char **argv		  - массив аргументов
													// optArgs *startArgs - указатель на структуру, в которой хранятся параметры работы приложения

													/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/

	loopArgs = 1;																		// Инициализируем единицей, поскольку нужные аргументы идут с 1
																						// Проверяем, чтобы кол-во аргументов соответсвовало необходимому
	if (argc < 2 || argc > 9)
	{
																						// Выводим пользователю пример запуска программы
		printf("Ошибка ввода!\nДля запуска программы необходимо ввести строку типа: \nping2020 [-n число запросов] [-l размер пакета] [-w максимальное время ожидания ответа] <-a IP-адрес или доменное имя> ");
		return 1;
	}
																						// Начинаем проход по аргументам
	for (; loopArgs < argc; loopArgs++)
	{
		if (strcmp(argv[loopArgs], "-n") == 0)											// Если флаг -n
		{
			StartArgs->count = atoi(argv[++loopArgs]);									// Записываем кол-во повторений
			continue;
		}
		else if (strcmp(argv[loopArgs], "-l") == 0)										// Если флаг -l			
		{
			StartArgs->packet_size = atoi(argv[++loopArgs]);							// Записываем размер пакета
			continue;
		}
		else if (strcmp(argv[loopArgs], "-w") == 0) {
			StartArgs->timeout = atoi(argv[++loopArgs]);								// Записываем время ожидания ответа
			continue;
		}
		else if (strcmp(argv[loopArgs], "-a") == 0)										// Если флаг -a
		{
			StartArgs->target = argv[++loopArgs];										// Записываем адрес назначения
			continue;
		}
		else if (strcmp(argv[loopArgs], "-?") == 0) {
			printf("ПАРАМЕТРЫ:\n");
			printf("-n - количество запросов\n");
			printf("-l - размер пакета\n");
			printf("-w - время ожидания ответа\n");
			printf("-a - доменное имя или IP адрес\n");
			printf("ПРИМЕР:\n");
			printf("ping2020 -n 3 -l 64 -w 15 -a yandex.ru\n");
			return 1;
		}
		else
		{
			printf("Параметры введены неверно. %d\n", loopArgs);
			printf("Для просмотра доступных для ввода параметров введите \"ping2020 -?\" ");
			return 1;																	// Если аргументы введены неправильно, выдаем 1
		}
	}
	return 0;
}