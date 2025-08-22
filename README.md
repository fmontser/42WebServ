# WebServ: Servidor HTTP/1.1 en C++

> Implementación de un servidor web HTTP/1.1 funcional desde cero en C++98, sin dependencias externas. Proyecto del Common Core de 42 Barcelona, enfocado en la programación de red de bajo nivel y la gestión de clientes concurrentes.

---

Stack Tecnológico y Conceptos Clave

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![HTTP/1.1](https://img.shields.io/badge/HTTP/1.1-Protocol-lightgrey?style=for-the-badge)
![Sockets API](https://img.shields.io/badge/Sockets%20API-Linux-yellow?style=for-the-badge)
![I/O Multiplexing](https://img.shields.io/badge/I/O%20Multiplexing-select()--poll()-orange?style=for-the-badge)

---

Características Principales

*   **Manejo de Clientes Concurrentes:** Utiliza I/O multiplexing (`select()`/`poll()`) para gestionar múltiples conexiones de clientes de forma simultánea en un único hilo, sin bloqueos.
*   **Cumplimiento del Protocolo HTTP/1.1:** Parsea y gestiona peticiones (GET, POST, DELETE), cabeceras, y construye respuestas válidas con códigos de estado correctos.
*   **Servidores Virtuales (Virtual Hosts):** Soporte para configurar múltiples "servidores" o dominios en diferentes puertos desde un único archivo de configuración, similar a Nginx.
*   **Contenido Estático y Dinámico:** Sirve archivos estáticos (HTML, CSS, imágenes) y ejecuta scripts para generar contenido dinámico a través de la interfaz **CGI (Common Gateway Interface)**.
*   **Gestión de Rutas y Métodos:** El archivo de configuración permite definir rutas específicas, limitar los métodos HTTP permitidos por ruta y configurar páginas de error personalizadas.
*   **Gestión de Carga de Archivos (Uploads):** Capacidad para manejar peticiones POST con cuerpos de mensaje para la subida de archivos al servidor.
