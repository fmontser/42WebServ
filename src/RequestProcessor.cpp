#include <iostream>
#include "HttpHeader.hpp"
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();

	//TODO @@@@@@@@ 111111 @@@@@@@@ LOS ENVIOS TAMBIEN SUFREN DE LA CODIFICACION TEXTO PLANO!!!! (por eso no funcniona favicon??)
	if (request.method == "GET") {
		FileManager::readFile(dataAdapter);
		if (response.statusCode.empty())
			response.setupResponse(HttpResponse::OK);
		connection->isChunkedResponse = response.isChunked();
	}
	else if (request.method == "POST") {
		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.setupResponse(HttpResponse::CONTINUE);
			return ;
		}
		HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter);
		if (rtype != HttpResponse::CREATED || (response.statusCode.empty() && connection->contentLength == 0))
			response.setupResponse(rtype);
	}
	else if (request.method == "DELETE") {
		//TODO DELETE METHOD
		response.setupResponse(HttpResponse::NO_CONTENT);
	}
	else {
		request.url = "/default/501.html"; //TODO hardcoded, debe obtener la ruta del config.
		FileManager::readFile(dataAdapter);	//TODO devolver pagina error!!!
		response.setupResponse(HttpResponse::METHOD_NOT_IMPLEMENTED);
	}
}


