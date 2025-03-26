#pragma once

#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Connection;

class DataAdapter {
	private:

		HttpRequest		_request;
		HttpResponse	_response;
		Connection		*_connection;

	public:
		DataAdapter(Connection *connection);
		~DataAdapter();
		DataAdapter(const DataAdapter& src);
		DataAdapter& operator=(const DataAdapter& src);

		bool				isHeadersComplete;

		Connection			*getConnection() const;

		HttpRequest&		getRequest();
		HttpResponse&		getResponse();
		void				deserializeRequest();
		void				serializeResponse();
		static HttpHeader	deserializeHeader(std::string header);
};
