
# Mictlán Core: estado del desarrollo

Fecha: 5 de noviembre de 2026
Etapa: desarrollo inicial de Mictlán Alfa 0.1

## Origen

Proyecto derivado de Latino-Core, con licencia MIT.

Revisión base:
53e27d254f4befff205c50ac2d7ac58bae83a255

Rama de desarrollo:
desarrollo/mictlan-alpha

## Funcionalidades comprobadas

- Compilación Debug en Debian: correcta.
- Intérprete heredado `latino`: generado.
- Biblioteca compartida: generada.
- Biblioteca estática: generada.
- Programa ejemplos/hola_mictlan.lat: ejecutado.
- Suites de regresión: 11 de 11 aprobadas.

## Limitaciones conocidas

- El ejecutable todavía se llama `latino`.
- La sintaxis sigue siendo la de Latino.
- La configuración CMake modifica latino.rc.
- Existen advertencias históricas del compilador.
- No se ha publicado una distribución de Mictlán.
- Las pruebas no garantizan ausencia total de defectos.

## Siguientes objetivos

1. Registrar este primer avance en Git.
2. Corregir la generación de archivos de recursos.
3. Adaptar el nombre público del ejecutable a mictlan.
4. Comprobar que los programas existentes siguen funcionando.
5. Integrar el SDK gráfico y probar una ventana real.
6. Preparar documentación e instalación para usuarios.
