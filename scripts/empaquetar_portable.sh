
#!/usr/bin/env bash

# Mictlán: generador experimental de distribución portable.
set -euo pipefail

# Localizar el proyecto independientemente de la terminal.
core="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
proyecto="$(dirname "$core")"

compilacion="$proyecto/build-core-release"

# Preparar carpetas nuevas, sin sobrescribir paquetes previos.
mkdir -p "$proyecto/artefactos"

temporal="$(mktemp -d "$HOME/mictlan-armado-XXXXXX")"
salida="$(mktemp -d "$proyecto/artefactos/lote-XXXXXX")"

echo "===== 1. CONFIGURAR ====="

cmake -S "$core" -B "$compilacion" \
    -DCMAKE_BUILD_TYPE=Release

echo "===== 2. COMPILAR ====="

cmake --build "$compilacion" \
    --target latino --parallel 2
identidad=$("$compilacion/mictlan" --version)
version=${identidad#Mictlán }
[[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[a-zA-Z0-9.]+)?$ ]]
nombre="mictlan-$version-linux-x86_64"

echo "===== 3. INSTALAR COMPONENTE PORTABLE ====="

cmake --install "$compilacion" \
    --prefix "$temporal/$nombre" \
    --component MictlanPortable

echo "===== 4. COMPRIMIR ====="

archivo="$salida/$nombre.tar.gz"

tar -czf "$archivo" -C "$temporal" "$nombre"

echo "===== 5. VERIFICAR SHA-256 ====="

(
    cd "$salida"
    sha256sum "$nombre.tar.gz" > "$nombre.tar.gz.sha256"
    sha256sum -c "$nombre.tar.gz.sha256"
)

echo "===== 6. PROBAR EXTRACCION ====="

mkdir -p "$temporal/verificacion"

tar -xzf "$archivo" -C "$temporal/verificacion"

ejecutable="$temporal/verificacion/$nombre/bin/mictlan"
ejemplo="$temporal/verificacion/$nombre/share/mictlan/ejemplos/hola_mictlan.mict"

resultado="$(env -u LD_LIBRARY_PATH "$ejecutable" "$ejemplo")"

printf '%s\n' "$resultado"

if ! grep -Fq "El resultado es: 42" <<< "$resultado"; then
    echo "ERROR: el ejemplo no produjo el resultado esperado." >&2
    exit 1
fi

echo "===== DISTRIBUCION LOCAL VERIFICADA ====="
echo "Paquete: $archivo"
echo "Comprobacion: $archivo.sha256"
du -sh "$archivo"
