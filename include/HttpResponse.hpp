#pragma once
#include "HttpMessage.hpp"

class HttpResponse : public HttpMessage {
	public:
		enum responseType {
			CONTINUE = 100,
			OK = 200,
			CREATED = 201,
			NO_CONTENT = 204,
			SEE_OTHER = 303,
			FORBIDDEN = 403,
			NOT_FOUND = 404,
			CONFLICT = 409,
			SERVER_ERROR = 500,
			METHOD_NOT_IMPLEMENTED = 501
		};

		std::string	statusCode;
		std::string	statusMsg;

		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& src);
		HttpResponse& operator=(const HttpResponse& src);

		bool	isChunked();
		void	setupResponse(enum responseType responseType);
};