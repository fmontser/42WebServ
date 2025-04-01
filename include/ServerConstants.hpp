#pragma once

#define GREEN	"\033[0;32m"
#define RED		"\033[0;31m"
#define YELLOW  "\033[0;33m"
#define BLUE	"\033[0;34m"
#define END		"\033[0m"

#define CRLF					"\r\n"
#define CRLF_OFFSET				2
#define HTTP_BODY_START			"\r\n\r\n"
#define HTTP_BODY_START_OFFSET	4

#define MIN_PAYLOAD 1
#define MAX_PAYLOAD 131072
#define SOCKET_BUFFER_SIZE 131072
#define SOCKET_LISTEN_QUEUE_SIZE 5
#define MIN_PORT_NUMBER 0
#define MAX_PORT_NUMBER 131072
#define READ_BUFFER 131072
#define HTTP_VERSION "HTTP/1.1"
