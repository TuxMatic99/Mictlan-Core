# Mictlán 0.1.0 Alfa

**Distribución experimental de consola para Linux x86_64**

Mictlán es un proyecto de lenguaje de programación de propósito general, con sintaxis en español, desarrollado a partir del lenguaje Latino.

Esta distribución es experimental y está destinada a pruebas.

## Requisitos

- Sistema Linux x86_64 compatible.
- Bibliotecas del sistema necesarias, incluyendo glibc, libm, readline y tinfo.

Esta distribución se ha probado en Debian 13. No se garantiza todavía su funcionamiento en otras distribuciones.

## Ejecutar Mictlán

Abre una terminal dentro de la carpeta extraída.

Para ejecutar el programa de ejemplo:

```bash
./bin/mictlan ./share/mictlan/ejemplos/hola_mictlan.lat
```

Para ejecutar tu propio programa:

```bash
./bin/mictlan /ruta/al/programa.lat
```

## Desinstalación

Esta versión portable no requiere instalación global. Para retirarla, elimina únicamente la carpeta que extrajiste.

No elimines los directorios donde hayas guardado tus propios programas.

## Limitaciones

Esta primera distribución incluye el intérprete de consola. La interfaz gráfica y las bibliotecas adicionales se distribuirán por separado.

## Origen y licencia

Mictlán deriva del lenguaje Latino.

El código heredado conserva sus atribuciones originales. Consulta `share/doc/mictlan/copyright` para conocer la licencia MIT del núcleo.

Las aportaciones originales de Mictlán tienen sus respectivos avisos de copyright.

## Estado

Versión alfa experimental. Las interfaces y funcionalidades pueden cambiar antes de la versión estable.