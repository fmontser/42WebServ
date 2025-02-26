#!/bin/bash

# Nombre del archivo a subir
ARCHIVO="archivo_de_prueba.txt"

# Crea un archivo de prueba de 10KB
head /dev/urandom | tr -dc A-Za-z0-9 | head -c 10240 > "$ARCHIVO"

# URL del servidor
URL="http://localhost:8042"

# Comando curl
curl \
  -v \
  -F "archivo=@$ARCHIVO" \
  -F "campo_extra=valor_extra" \
  "$URL"

# Opcional: Elimina el archivo de prueba después de la subida
# rm "$ARCHIVO"