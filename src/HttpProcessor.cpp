#include <iostream>
#include "HttpHeader.hpp"
#include "HttpProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"
#include "PathManager.hpp"
#include "Utils.hpp"
#include "Index.hpp"
#include "CgiAdapter.hpp"
#include <unistd.h>

static	HttpResponse::responseType	validateRoute(DataAdapter& dataAdapter) {
	HttpRequest	request = dataAdapter.getRequest();
	Server&		server = dataAdapter.getConnection()->getServer();
	Route		*actualRoute;
	HttpResponse::responseType result = dataAdapter.getConnection()
			->getServer().getRequestedRoute(&actualRoute, dataAdapter);

	if(result != HttpResponse::EMPTY)
		return result;
	if (!actualRoute->getRedirect().empty())
		return HttpResponse::SEE_OTHER;
	if (!actualRoute->isMethodImplemented(server, request.method))
		return HttpResponse::METHOD_NOT_IMPLEMENTED;
	if (!actualRoute->isMethodAllowed(request.method))
		return HttpResponse::METHOD_NOT_ALLOWED;
	if (Index::isIndexRoute(dataAdapter, actualRoute)) {
		return HttpResponse::DIR_LIST;
	}
	return HttpResponse::EMPTY;
}


#include "Config.hpp"
#include "Server.hpp"

static bool	deriveConnection(DataAdapter& dataAdapter) {
	HttpRequest&	request = dataAdapter.getRequest();
	Connection		*actualConnection = dataAdapter.getConnection();
	std::string		hostName = request.getHostName();
	
	std::map<std::string, Server>& serverList = Config::getServers();
	

	for (std::map<std::string, Server>::iterator server = serverList.begin(); server != serverList.end(); ++server) {

		// busca una coinidencia de host en todos los servers 
		std::vector<std::string>& serverHosts = server->second.getHosts();
		for (std::vector<std::string>::iterator host = serverHosts.begin(); host != serverHosts.end(); ++host) {
			if (*host == hostName) {

				//si la encuentra...


				std::list<Connection *>& connectionList = server->second.getConnectionList();
				for (std::list<Connection *>::iterator connection = connectionList.begin(); connection != connectionList.end(); ++connection) {

					// busca la conexion actual  en la lista de las conexiones de ESTE server

					//si la encuentra, adelante
					if (*connection == actualConnection) {
						actualConnection->isDerived = false;
						return false;
					}

				}
				// si no tiene esta conexion, debe tenerla, mover conexion aqui.

				connectionList.push_back(actualConnection);
				actualConnection->getServer().getConnectionList().remove(actualConnection);
				actualConnection->setServer(server->second);
				actualConnection->isDerived = true;
				return true;

			}
				
		}
	
	}

	//buscar server default para este puerto
	int port = actualConnection->getServer().getPort();

	//mover la conexion al default para el port (el primero)
	for (std::map<std::string, Server>::iterator server = serverList.begin(); server != serverList.end(); ++server) {
		if (server->second.getPort() == port) {
			std::list<Connection *>& connectionList = server->second.getConnectionList();
			connectionList.push_back(actualConnection);
			actualConnection->getServer().getConnectionList().remove(actualConnection);
			actualConnection->setServer(server->second);
			actualConnection->isDerived = true;
			return true;
		}
	}
	actualConnection->isDerived = false;
	return false;
}


void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter) {
	
	HttpResponse::responseType rtype;
	
	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();
	
	//TODO abstraer
	if (deriveConnection(dataAdapter)) {
		std::cout << BLUE << "Info: Connection derived to server " << connection->getServer().getName() << END << std::endl;
		return ;
	}

	if (connection->requestMode == Connection::SINGLE) {
		rtype = validateRoute(dataAdapter);
		if (rtype != HttpResponse::EMPTY) {
			response.setupResponse(rtype, dataAdapter);
			return;
		}
	}

	if (connection->isOverPayloadLimit) {
		connection->requestMode = Connection::SINGLE;
		response.setupResponse(HttpResponse::PAYLOAD_TOO_LARGE, dataAdapter);
		return ;
	}
	
	if (request.method == "GET") {
		if (request.isCgiRequest) {
			rtype = cgiAdapter.processCgi(dataAdapter);
			if (rtype != HttpResponse::EMPTY)
				response.setupResponse(rtype, dataAdapter);
			return;
		}
		else {
			rtype  = FileManager::readFile(dataAdapter);
			response.setupResponse(rtype, dataAdapter);
			connection->responseMode = (Connection::ResponseMode)response.isChunked();
		}
	}
	else if (request.method == "POST") {


		if (connection->requestMode == Connection::SINGLE) {
			if (request.handlePostMode(connection)) {
				return;
			} else {
				response.setupResponse(HttpResponse::UNSUPPORTED_MEDIA_TYPE, dataAdapter);
				return;
			}

		}


		if (request.isCgiRequest) {
			
			for (std::vector<char>::iterator it = request.body.begin(); it != request.body.end(); ++it)
				cgiAdapter.body.push_back(*it);

			if (connection->contentLength == 0) {
				rtype = cgiAdapter.processCgi(dataAdapter);
				if (rtype != HttpResponse::EMPTY)
					response.setupResponse(rtype, dataAdapter);
			}
			return;
		}
		else {

			HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter);
			if ((connection->requestMode == Connection::PARTS && connection->contentLength == 0)
				|| (connection->requestMode == Connection::CHUNKS && connection->hasChunksEnded))
				response.setupResponse(rtype, dataAdapter);
			
		}
	}
	else if (request.method == "DELETE") {
		rtype = FileManager::deleteFile(dataAdapter);
		response.setupResponse(rtype, dataAdapter);
	}
}


