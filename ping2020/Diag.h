#ifndef DIAG_H
#define DIAG_H

/*										***Директивы препроцессора, необходимые для включения заголовочных файлов.***			*/
#include <WinSock2.h>
#include <regex>
#include <sys/types.h>
#include "logger.h"
#include <sys/stat.h>
#include <stdbool.h>
#include <ws2tcpip.h>
/*										***Константы описывающие регулярные выражения для обработки входных данных.***			*/

										//описание функций (signature) для диагностики
char* hostnameToIp(char*);
int AddIntoLogFile(someArgs_t*, char*, char*, char*);
int My_NODE_DIAGNOSTIC(char*, someArgs_t*);
int myECHODiagnostic(someArgs_t*);
int myLoggerInitialDiagnostic(someArgs_t*);
int MY_LOG_DIAGNOSTIC(someArgs_t*);


#endif									/* MY_DIAGNOZ*/
