# Proyecto Final – Olimpiadas de Invierno 🏔

Este proyecto en lenguaje **C** implementa un sistema completo de gestión para las Olimpiadas de Invierno, utilizando una base de datos **MySQL**.

## 📁 Estructura del Proyecto

- **5ManyToMany.c**: Código fuente en C con operaciones CRUD para entidades como:
  - Federaciones
  - Esquiadores
  - Estaciones de esquí
  - Equipos participantes
  - Pruebas
- **script_olimpiadas.sql**: Script SQL corregido con claves foráneas y `ON DELETE CASCADE`, además de restricciones `CHECK` para garantizar la integridad de los datos.

##  Funcionalidades

- Menús interactivos en consola (uso de `gotoxy` y `setcolor`)
- CRUD completo con validaciones:
  - No se aceptan campos vacíos ni negativos
  - Verificación de duplicados
  - Confirmación antes de eliminar o actualizar
  - Formato HH:MM:SS validado
- Conexión con base de datos MySQL mediante la librería oficial
- Diseño relacional normalizado

## 🛠 Requisitos

- MySQL Server
- Librería `mysql.h` instalada (conectado vía terminal o Dev-C++)
- Terminal compatible con ANSI (Linux/macOS o Git Bash para Windows)

##  Instrucciones de uso

1. Ejecutar el script SQL en tu servidor MySQL:
   ```sql
   source script_olimpiadas.sql;
2.-Compilar el archivo en consola:
gcc 5ManyToMany.c -o olimpiadas -lmysqlclient
./olimpiadas
3.	Navegar por los menús para gestionar los datos.
Créditos
Proyecto desarrollado por Kevin Sánchez Calderón, con estructura académica basada en el problema 15 del libro Diseño de Bases de Datos: Problemas Resueltos.
