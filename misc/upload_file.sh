#!/bin/bash

# Nombre del archivo a subir
ARCHIVO="en.subject.pdf"
#ARCHIVO="image.jpg"
#ARCHIVO="guia.html"



# URL del servidor
URL="http://localhost:8042"

# Comando curl
curl \
  -v \
  -F "archivo=@$ARCHIVO" \
  "$URL"