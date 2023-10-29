#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
/***Директивы препроцессора, необходимые для включения заголовочных файлов.***/
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
/*								**Функция проверки текущего размера лог-файла**
					*В качестве параметров получает набор аргументов (sb).
*/
{													// Begin IsExistLog
													/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
	struct stat st;									// Переменна структуры для  хранения/получения информации о файле

													/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/
	*exist = stat(sb->LogFileName, &st);			// Запрашиваем у ОС информацию о файле в структуру st
	if (!*exist)
	{
		*Filesize = st.st_size;						// Выбираем из структуры только инфо. о размере файла и записываем в переменную 
	}

}													// End IsExistLog

int initLog(someArgs_t* sb)
{
													/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
	long Filesize;									// Размер лог-файла
	int exist;										// Создан ли лог-файл
	int Overflow;									// Переполнение лог-файла
	long сLogSIZE;									// Предел размера лог-файла

													/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/
	сLogSIZE = 8388608;								// Верхний предел размера лог-файла в байтах
	exist = 0;										// Создан ли лог-файл
	Filesize = 0;									// Размер лог-файла
	Overflow = 0;									// Переполнение лог-файла

	IsExistLog(sb, &Filesize, &exist);				// Проверяем, существует ли лог-файл с указ. именем
	sb->fd_RW = _open(sb->LogFileName, O_RDWR | O_APPEND, 0);	// Пытаемся открыть лог-файл

	if (Filesize >= сLogSIZE)						// Сравниваем полученное значение с константой max size
	{
		Overflow = 1;
	}
	else
	{
		Overflow = 0;
	}

	if (exist)										// Если файл не существует
	{
													// Пытаемся создать файл
		if ((sb->fd_RW = _creat(sb->LogFileName, _S_IREAD | _S_IWRITE) == -1))
		{
			fprintf(stderr, "IsExistLog: Невозможно создать файл '%s'\n",
				sb->LogFileName);
			sb->logErrorCode = errno;
			return 1;
		}
	}
	else if (Overflow)								// Если лог-файл "переполнен"
	{
		_close(sb->fd_RW);							// Закрываем, чистим и открываем вновь
		if ((sb->fd_RW = _open(sb->LogFileName, O_RDWR | O_TRUNC | O_APPEND, 0)) == -1)
		{
			fprintf(stderr, "IsExistLog: Невозможно очистить файл '%s'\n",
				sb->LogFileName);
			sb->logErrorCode = errno;
			return 1;
		}
	}
	else if (sb->fd_RW == -1)						// Если файл не открылся по каким-то причинам
	{
		fprintf(stderr, "IsExistLog: Невозможно открыть файл '%s'\n",
			sb->LogFileName);
		sb->logErrorCode = errno;
		return 1;
	}

	if (AddIntoLogFile(sb, (char*)"Программа запущена\n", (char*)"INFO", NULL))
	{
		MY_LOG_DIAGNOSTIC(sb);
	}
	
	return 0;
}

int print_log(someArgs_t* sb)
								/*								**Функции вывода лога на экран**
													*В качестве параметров получает набо аргументов (sb).
								*/
{													// Begin print_log

													/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/

	char* line;										// Объявляем переменную для хранения считанной строки из файла
	size_t read_bytes;
													/*Определения Константы*/
	size_t len;										// Макс. длина считываемой строки

													/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/
	len = 512;
	read_bytes = 0;
	line = (char*)calloc(len, sizeof(char));		// Выделение памяти под 512 символов

	if (!sb->fd_RW)
	{
		sb->logErrorCode = errno;
		return 1;
	}

	_lseek(sb->fd_RW, 0, SEEK_SET);					// Смещаемся в начало файла
	while ((read_bytes = _read(sb->fd_RW, line, len)) > 0)
	{
		line[read_bytes] = 0;
		printf("%s", line);							// Выводим считанное на экран
	}

	MY_LOG_DIAGNOSTIC(sb);

	line = NULL;
	return 0;										// Возвращаем 0
}													// End prin_log


int log_info(someArgs_t* sb)
					/*								**Функция для записи данных в лог-файл**
						*Формирует "запись" из полученных параметров и осуществляет запись их в файл.
						*В качестве параметров получает набо аргументов (sb).
					*/
{													// Begin log_info

													/*ОПИСАНИЕ ПЕРЕМЕННЫХ*/
	char buf[512];									// Буфер для формирования записи
	time_t now;										// Переменная для хранения времени
	char* date;										// Переменная для хранения даты

													/*ОПРЕДЕЛЕНИЕ ПЕРЕМЕННЫХ (Initialization)*/
	date = NULL;

	if (!sb->fd_RW)									// Если лог-файл не открылся
	{
		sb->logErrorCode = errno;
		return 1;									// Возвращаем 1
	}												
	time(&now);										// Получаем текущее время
	date = ctime(&now);								// Получаем текузаю дату
	date[strlen(date) - 1] = '\0';					// Дописываем символ конца строки 

	sprintf(buf, "%s [%s] %s \n", date,				// Формируем строку для записи в лог
		sb->messageType, sb->message);

	sb->logErrorCode = _write(sb->fd_RW, buf, (unsigned)strlen(buf));
	return 0;										//	возвращаем 0
}													//end log_info
