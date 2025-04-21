#pragma once
#include "HttpMessage.hpp"

class DataAdapter;

class HttpResponse : public HttpMessage {
	public:
		enum responseType {
			EMPTY = 0,
			DIR_LIST = 1,
			CONTINUE = 100,
			OK = 200,
			CREATED = 201,
			NO_CONTENT = 204,
			SEE_OTHER = 303,
			BAD_REQUEST = 400,
			FORBIDDEN = 403,
			NOT_FOUND = 404,
			METHOD_NOT_ALLOWED = 405,
			CONFLICT = 409,
			PAYLOAD_TOO_LARGE = 413,
			UNSUPPORTED_MEDIA_TYPE = 415,
			SERVER_ERROR = 500,
			METHOD_NOT_IMPLEMENTED = 501,
			GATEWAY_TIMEOUT = 504
		};

		std::string	statusCode;
		std::string	statusMsg;

		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& src);
		HttpResponse& operator=(const HttpResponse& src);

		int		isChunked();
		void	setupResponse(enum responseType responseType, DataAdapter& dataAdapter);
};