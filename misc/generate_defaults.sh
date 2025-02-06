#!/bin/bash

# Directorio donde se guardarán las páginas de error
ERROR_DIR="../default"

# Crear el directorio si no existe
mkdir -p "$ERROR_DIR"

# Contenido de las páginas de error
generate_page() {
    local error_code=$1
    local error_name=$2
    local file_path="$ERROR_DIR/$error_code.html"

    cat > "$file_path" <<EOL
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>$error_code $error_name</title>
    <link rel="stylesheet" href="default/style.css">
</head>
<body>
    <h1>$error_code $error_name - 42Webserv</h1>
</body>
</html>
EOL

    echo "Página de error $error_code generada: $file_path"
}

# Generar las páginas de error para los códigos seleccionados
generate_page 400 "Bad Request"
generate_page 403 "Forbidden"
generate_page 404 "Not Found"
generate_page 405 "Method Not Allowed"
generate_page 500 "Internal Server Error"
generate_page 501 "Not Implemented"

# Crear el archivo de estilo CSS común para todas las páginas
cat > "$ERROR_DIR/style.css" <<EOL
body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    margin: 0;
    font-family: Arial, sans-serif;
    background-color: #f4f4f4;
    color: #333;
}

h1 {
    font-size: 36px;
    text-align: center;
}
EOL

echo "Todas las páginas de error y el archivo de estilo CSS han sido generados en el directorio $ERROR_DIR"
