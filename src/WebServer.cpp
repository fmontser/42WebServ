#include "WebServer.hpp"
#include "colors.hpp"
#include <algorithm>

WebServer::WebServer(WebSocket& ws) : _ws(ws) {}

WebServer::~WebServer() {
	_ws.stop();
}

void WebServer::run() {
	// Configuración inicial del poll para escuchar eventos en el socket del servidor.
	pollfd pfd = { _ws.getServersocket(), POLLIN, 0 };
	_fds.push_back(pfd); // Agrega el descriptor del servidor al conjunto de descriptores monitoreados.
	std::cout << GREEN << "Starting webserver loop..." << END << std::endl;
	
	while (true) {
		// Espera a que ocurra un evento en alguno de los descriptores monitoreados 
		//usand poll con un timeout de -1. Poll monitorea los fd (en _fds) y maneja 
		//los eventos (conexiones) sin bloquear el hilo...
		int ret = poll(_fds.data(), _fds.size(), -1);
		if (ret == -1) {
			std::cerr << RED << "poll() failed" << END << std::endl;
			break;
		}
		if (ret == 0) {
			std::cerr << RED << "poll() timed out" << END << std::endl;
			continue;
		}
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _ws.getServersocket()) {
					_acceptConnection();
				} else {
					_handleClient(_fds[i]);
				}
			}
		}
	}
}

void	WebServer::_acceptConnection() {
	// Acepta una nueva conexión y agrega el nuevo descriptor al conjunto de descriptores monitoreados.
	// _client_addr es una estructura que contiene la dirección del cliente que se conecta.
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int newSocket = accept(_ws.getServersocket(), (sockaddr*)&client_addr, &client_addr_len);
	if (newSocket == -1) {
		std::cerr << RED << "Error accepting new connection " << END << "࣬࣬ 👾" << std::endl;
		return;
	}
	std::cout << GREEN << "New connection, socket fd: " << newSocket << END << std::endl;
	}
	//he creado la struct RemovePollfd para poder borrar el fd del cliente que se desconecta 
	//sin la utilizacion de una funcion lambda (prohibida en el proyecto)
struct RemovePollfd {
		int fdToRemove;
		RemovePollfd(int fd) : fdToRemove(fd) {}
		bool operator()(const pollfd& p) const {
				return p.fd == fdToRemove;
	}
};
//la funcion _handleClient recibe un pollfd y se encarga de recibir los datos del cliente y almacenarlos en un buffer
//esta funcion se utiliza una vez que se ha aceptado la conexion del cliente
void	WebServer::_handleClient(pollfd& pfd) {
	char buf[1024] = {0};
	// la funcion recv recibe los datos del cliente y los almacena en el buffer
	int len = recv(pfd.fd, buf, sizeof(buf), 0);
	if (len == -1) {
		std::cerr << RED << "Client disconnected, socket fd: " << pfd.fd << END << std::endl;
		close(pfd.fd);
		_fds.erase(std::remove_if(_fds.begin(), _fds.end(), RemovePollfd(pfd.fd)), _fds.end()); 
		_clients.erase(pfd.fd);
		return;
	} else if (len == 0) {
		std::cerr << RED << "recv() failed" << END << std::endl;
		return;
	} else {
		std::cout << BLUE << "Received: " << buf << END << std::endl;
		_clients[pfd.fd] += buf;
	}
}