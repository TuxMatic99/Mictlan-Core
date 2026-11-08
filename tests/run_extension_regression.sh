
#!/usr/bin/env bash
set -euo pipefail

# Localizar el repositorio y el interprete.
raiz="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
interprete="$raiz/../build-core-debug/mictlan"
ejemplo="$raiz/ejemplos/hola_mictlan.lat"

# Comprobar los archivos necesarios.
if [[ ! -x "$interprete" || ! -f "$ejemplo" ]]; then
    echo "ERROR: falta el interprete o el ejemplo." >&2
    exit 1
fi

# Crear archivos temporales para las pruebas.
temporal="$(mktemp -d)"
trap 'rm -rf -- "$temporal"' EXIT

cp "$ejemplo" "$temporal/hola.mict"
cp "$ejemplo" "$temporal/hola.lat"
cp "$ejemplo" "$temporal/hola.txt"
cp "$ejemplo" "$temporal/hola"

# Ejecutar y verificar cada escenario.
probar() {
    local nombre="$1"
    local archivo="$2"
    local debe_funcionar="$3"
    local mensaje="$4"
    local salida
    local codigo

    if salida=$("$interprete" "$archivo" 2>&1); then
        codigo=0
    else
        codigo=$?
    fi

    if [[ "$debe_funcionar" == "si" && "$codigo" -ne 0 ]] ||
       [[ "$debe_funcionar" == "no" && "$codigo" -eq 0 ]] ||
       [[ "$salida" != *"$mensaje"* ]]; then
        echo "FALLO: $nombre (codigo=$codigo)" >&2
        printf '%s\n' "$salida" >&2
        return 1
    fi

    echo "OK: $nombre (codigo=$codigo)"
}

probar "extension mict" "$temporal/hola.mict" \
    "si" "El resultado es: 42"

probar "compatibilidad lat" "$temporal/hola.lat" \
    "si" "El resultado es: 42"

probar "extension txt rechazada" "$temporal/hola.txt" \
    "no" "debe tener extension .mict o .lat"

probar "sin extension rechazado" "$temporal/hola" \
    "no" "debe tener extension .mict o .lat"

probar "archivo inexistente" "$temporal/no_existe.mict" \
    "no" "No se pudo abrir el archivo"

echo "MICTLAN_EXTENSIONES_OK"
