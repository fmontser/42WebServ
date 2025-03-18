#!/bin/bash

# Nombre del archivo a subir
ARCHIVO="image.jpg"
#ARCHIVO="test.html"

# URL del servidor
URL="http://localhost:8042"

# Comando curl
curl \
  -v \
  -F "archivo=@$ARCHIVO" \
  "$URL"