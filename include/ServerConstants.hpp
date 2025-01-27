#pragma once

#define GREEN	"\033[0;32m"
#define RED		"\033[0;31m"
#define BLUE	"\033[0;34m"
#define END		"\033[0m"

#define CRLF	"\r\n"

#define MIN_PAYLOAD 0
#define MAX_PAYLOAD 67108864 //64Mb
#define TIMEOUT 1000
#define SOCKET_BUFFER_SIZE 65536
#define SOCKET_LISTEN_QUEUE_SIZE 5
#define MIN_PORT_NUMBER 0
#define MAX_PORT_NUMBER 65536
#define BUFFER_SIZE 8192
#define HTTP_VERSION "HTTP/1.1"
