#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

// Configuración de la base de datos
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS ""
#define DB_NAME "olimpiadas_invierno"

// Definiciones de colores
#define RESET "\033[0m"
#define ROJO "\033[31m"
#define VERDE "\033[32m"
#define AMARILLO "\033[33m"
#define AZUL "\033[34m"
#define MAGENTA "\033[35m"
#define CIAN "\033[36m"
#define BLANCO "\033[37m"

// Conexión global
MYSQL *conn;

// Cambiar color de texto
void setcolor(int color) {
    switch(color) {
        case 1: printf(ROJO); break;
        case 2: printf(VERDE); break;
        case 3: printf(AMARILLO); break;
        case 4: printf(AZUL); break;
        case 5: printf(MAGENTA); break;
        case 6: printf(CIAN); break;
        case 7: printf(BLANCO); break;
        default: printf(RESET); break;
    }
}

// Mover cursor
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Limpiar pantalla
void limpiar_pantalla() {
    system("clear");
}

// Pausar ejecución
void pausar() {
    printf("\nPresiona ENTER para continuar...");
    while (getchar() != '\n'); // Espera Enter
}

// Limpiar buffer
void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
// Conexión a la base de datos
void conectar_bd() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        setcolor(1); // Rojo
        printf("Error al inicializar la conexión.\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0) == NULL) {
        setcolor(1);
        printf("Error de conexión: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    setcolor(2); // Verde
    printf("Conectado a la base de datos exitosamente.\n");
    setcolor(0);
}

// Desconectar de la base de datos
void desconectar_bd() {
    if (conn != NULL) {
        mysql_close(conn);
        conn = NULL;
        printf("Conexión cerrada correctamente.\n");
    }
}
// Mostrar encabezado general
void mostrar_encabezado(const char *titulo) {
    limpiar_pantalla();
    setcolor(4); // Azul
    printf("============================================\n");
    printf("       SISTEMA DE GESTIÓN DE OLIMPIADAS      \n");
    printf("============================================\n");
    setcolor(5); // Magenta
    printf(">>> %s\n", titulo);
    setcolor(0); // Reset
}

// Mostrar ejemplo de tabla (usado en menús)
void mostrar_ejemplos_tabla(const char *nombre_tabla) {
    setcolor(6); // Cian
    printf("Ejemplo de datos en la tabla %s:\n", nombre_tabla);
    setcolor(0);
    
    char consulta[256];
    snprintf(consulta, sizeof(consulta), "SELECT * FROM %s LIMIT 5", nombre_tabla);

    if (mysql_query(conn, consulta) == 0) {
        MYSQL_RES *resultado = mysql_store_result(conn);
        if (resultado) {
            int num_campos = mysql_num_fields(resultado);
            MYSQL_ROW fila;

            while ((fila = mysql_fetch_row(resultado))) {
                for (int i = 0; i < num_campos; i++) {
                    printf("%s\t", fila[i] ? fila[i] : "NULL");
                }
                printf("\n");
            }

            mysql_free_result(resultado);
        } else {
            setcolor(1);
            printf("No se pudo recuperar el resultado.\n");
        }
    } else {
        setcolor(1);
        printf("Error al ejecutar la consulta: %s\n", mysql_error(conn));
    }

    setcolor(0);
}
void crear_federacion() {
    mostrar_encabezado("CREAR NUEVA FEDERACIÓN");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("federacion");

    int id = -1, num_federados = -1;
    char nombre[101];
    char query[512];
    char confirmacion;

    // === Validar ID ===
    while (1) {
        printf("Ingrese ID de la federación (entero positivo): ");
        if (scanf("%d", &id) != 1 || id <= 0) {
            setcolor(1);
            printf("ID inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        // Verificar si ID ya existe
        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("El ID ya existe. Intenta con otro.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // === Validar nombre ===
    while (1) {
        printf("Ingrese nombre de la federación (1 a 100 caracteres): ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0'; // quitar salto de línea

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Validar número de federados ===
    while (1) {
        printf("Ingrese número de federados (entero positivo): ");
        if (scanf("%d", &num_federados) != 1 || num_federados <= 0) {
            setcolor(1);
            printf("Número inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // Confirmar acción
    printf("¿Deseas guardar esta federación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar inserción
    snprintf(query, sizeof(query),
             "INSERT INTO federacion (id, nombre, num_federados) VALUES (%d, '%s', %d)",
             id, nombre, num_federados);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al insertar federación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        setcolor(2);
        printf("¡Federación creada exitosamente!\n");
        setcolor(0);
    }

    pausar();
}
void leer_federaciones() {
    mostrar_encabezado("LISTADO DE FEDERACIONES");
    gotoxy(1, 6);

    MYSQL_RES *result;
    MYSQL_ROW row;

    if (mysql_query(conn, "SELECT * FROM federacion ORDER BY id")) {
        setcolor(1);
        printf("Error al consultar federaciones: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        setcolor(1);
        printf("Error al obtener resultados: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    int total = mysql_num_rows(result);
    if (total == 0) {
        setcolor(3); // Amarillo
        printf("No hay federaciones registradas.\n");
        setcolor(0);
        mysql_free_result(result);
        pausar();
        return;
    }

    setcolor(4); // Azul
    printf("%-10s %-30s %-15s\n", "ID", "NOMBRE", "NUM_FEDERADOS");
    printf("--------------------------------------------------------------\n");
    setcolor(0);

    while ((row = mysql_fetch_row(result))) {
        printf("%-10s %-30s %-15s\n",
               row[0] ? row[0] : " - ",
               row[1] ? row[1] : " - ",
               row[2] ? row[2] : " - ");
    }

    mysql_free_result(result);
    pausar();
}
void actualizar_federacion() {
    mostrar_encabezado("ACTUALIZAR FEDERACIÓN");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("federacion");

    int id = -1, num_federados = -1;
    char nombre[101];
    char query[512];
    char confirmacion;

    // === Leer ID y verificar existencia ===
    while (1) {
        printf("Ingrese ID de la federación a actualizar: ");
        if (scanf("%d", &id) != 1 || id <= 0) {
            setcolor(1);
            printf("ID inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // === Leer nombre ===
    while (1) {
        printf("Ingrese nuevo nombre de la federación (1 a 100 caracteres): ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0'; // eliminar salto de línea

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Leer número de federados ===
    while (1) {
        printf("Ingrese nuevo número de federados: ");
        if (scanf("%d", &num_federados) != 1 || num_federados <= 0) {
            setcolor(1);
            printf("Número inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // Confirmar actualización
    printf("¿Deseas actualizar esta federación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar la actualización
    snprintf(query, sizeof(query),
             "UPDATE federacion SET nombre='%s', num_federados=%d WHERE id=%d",
             nombre, num_federados, id);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al actualizar federación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Federación actualizada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se actualizó ningún dato.\n");
            setcolor(0);
        }
    }

    pausar();
}
void eliminar_federacion() {
    mostrar_encabezado("ELIMINAR FEDERACIÓN");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("federacion");

    int id = -1;
    char query[256];
    char confirmacion;

    // === Solicitar ID con validación ===
    while (1) {
        printf("Ingrese ID de la federación a eliminar: ");
        if (scanf("%d", &id) != 1 || id <= 0) {
            setcolor(1);
            printf("ID inválido. Debe ser un número entero positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        // Verificar existencia
        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No se encontró federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmar eliminación
    printf("¿Estás seguro de eliminar esta federación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar eliminación
    snprintf(query, sizeof(query), "DELETE FROM federacion WHERE id = %d", id);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al eliminar federación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Federación eliminada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se encontró federación con ID %d.\n", id);
            setcolor(0);
        }
    }

    pausar();
}
void crear_esquiador() {
    mostrar_encabezado("CREAR NUEVO ESQUIADOR");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("esquiador");

    char dni[21], nombre[101], query[512], confirmacion;
    int edad = -1, id_federacion = -1;

    // === Leer y validar DNI ===
    while (1) {
        printf("Ingrese DNI del esquiador (hasta 20 caracteres): ");
        fgets(dni, sizeof(dni), stdin);
        dni[strcspn(dni, "\n")] = '\0';

        if (strlen(dni) == 0) {
            setcolor(1);
            printf("El DNI no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        // Verificar duplicado
        snprintf(query, sizeof(query), "SELECT dni FROM esquiador WHERE dni = '%s'", dni);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("Ya existe un esquiador con ese DNI.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Leer y validar nombre ===
    while (1) {
        printf("Ingrese nombre del esquiador: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Leer y validar edad ===
    while (1) {
        printf("Ingrese edad (mínimo 5 años): ");
        if (scanf("%d", &edad) != 1 || edad < 5 || edad > 120) {
            setcolor(1);
            printf("Edad inválida. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // === Leer y validar ID de federación ===
    while (1) {
        printf("Ingrese ID de federación existente: ");
        if (scanf("%d", &id_federacion) != 1 || id_federacion <= 0) {
            setcolor(1);
            printf("ID inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        // Verificar existencia en federación
        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id_federacion);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmar antes de insertar
    printf("¿Deseas guardar este esquiador? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Insertar en la base de datos
    snprintf(query, sizeof(query),
             "INSERT INTO esquiador (dni, nombre, edad, id_federacion) VALUES ('%s', '%s', %d, %d)",
             dni, nombre, edad, id_federacion);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al insertar esquiador: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        setcolor(2);
        printf("¡Esquiador creado exitosamente!\n");
        setcolor(0);
    }

    pausar();
}
void leer_esquiadores() {
    mostrar_encabezado("LISTADO DE ESQUIADORES");
    gotoxy(1, 6);

    MYSQL_RES *result;
    MYSQL_ROW row;

    const char* query = "SELECT e.dni, e.nombre, e.edad, f.nombre AS federacion "
                        "FROM esquiador e "
                        "LEFT JOIN federacion f ON e.id_federacion = f.id "
                        "ORDER BY e.nombre";

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al consultar esquiadores: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        setcolor(1);
        printf("Error al obtener resultados: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    int total = mysql_num_rows(result);
    if (total == 0) {
        setcolor(3);
        printf("No hay esquiadores registrados.\n");
        setcolor(0);
        mysql_free_result(result);
        pausar();
        return;
    }

    setcolor(4); // Azul
    printf("%-15s %-25s %-8s %-20s\n", "DNI", "NOMBRE", "EDAD", "FEDERACIÓN");
    setcolor(0);
    printf("----------------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-15s %-25s %-8s %-20s\n",
               row[0] ? row[0] : " - ",
               row[1] ? row[1] : " - ",
               row[2] ? row[2] : " - ",
               row[3] ? row[3] : "Sin federación");
    }

    mysql_free_result(result);
    pausar();
}
void actualizar_esquiador() {
    mostrar_encabezado("ACTUALIZAR ESQUIADOR");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("esquiador");

    char dni[21], nombre[101], query[512], confirmacion;
    int edad = -1, id_federacion = -1;

    // === Leer y verificar existencia de DNI ===
    while (1) {
        printf("Ingrese DNI del esquiador a actualizar: ");
        fgets(dni, sizeof(dni), stdin);
        dni[strcspn(dni, "\n")] = '\0';

        if (strlen(dni) == 0) {
            setcolor(1);
            printf("El DNI no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT dni FROM esquiador WHERE dni = '%s'", dni);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe un esquiador con ese DNI.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Leer nombre ===
    while (1) {
        printf("Ingrese nuevo nombre: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Leer edad ===
    while (1) {
        printf("Ingrese nueva edad: ");
        if (scanf("%d", &edad) != 1 || edad < 5 || edad > 120) {
            setcolor(1);
            printf("Edad inválida. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // === Leer ID de federación ===
    while (1) {
        printf("Ingrese nuevo ID de federación: ");
        if (scanf("%d", &id_federacion) != 1 || id_federacion <= 0) {
            setcolor(1);
            printf("ID inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id_federacion);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmación
    printf("¿Deseas actualizar los datos del esquiador? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar actualización
    snprintf(query, sizeof(query),
             "UPDATE esquiador SET nombre='%s', edad=%d, id_federacion=%d WHERE dni='%s'",
             nombre, edad, id_federacion, dni);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al actualizar esquiador: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Esquiador actualizado exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se actualizó ningún dato.\n");
            setcolor(0);
        }
    }

    pausar();
}
void eliminar_esquiador() {
    mostrar_encabezado("ELIMINAR ESQUIADOR");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("esquiador");

    char dni[21], query[256], confirmacion;

    // === Solicitar y validar DNI ===
    while (1) {
        printf("Ingrese DNI del esquiador a eliminar: ");
        fgets(dni, sizeof(dni), stdin);
        dni[strcspn(dni, "\n")] = '\0';

        if (strlen(dni) == 0) {
            setcolor(1);
            printf("El DNI no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        // Verificar existencia
        snprintf(query, sizeof(query), "SELECT dni FROM esquiador WHERE dni = '%s'", dni);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No se encontró un esquiador con ese DNI.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // Confirmación de acción
    printf("¿Estás seguro de eliminar este esquiador? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar eliminación
    snprintf(query, sizeof(query), "DELETE FROM esquiador WHERE dni = '%s'", dni);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al eliminar esquiador: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Esquiador eliminado exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se encontró esquiador con DNI %s\n", dni);
            setcolor(0);
        }
    }

    pausar();
}
void crear_estacion() {
    mostrar_encabezado("CREAR NUEVA ESTACIÓN DE ESQUÍ");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("estacion");

    char codigo[21], nombre[101], direccion[151], telefono[21], personas_contacto[151];
    float km_esquiables = -1;
    char query[1024], confirmacion;

    // === Código (clave primaria) ===
    while (1) {
        printf("Ingrese código de la estación: ");
        fgets(codigo, sizeof(codigo), stdin);
        codigo[strcspn(codigo, "\n")] = '\0';

        if (strlen(codigo) == 0) {
            setcolor(1);
            printf("El código no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT codigo FROM estacion WHERE codigo = '%s'", codigo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("Ya existe una estación con ese código.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Nombre ===
    while (1) {
        printf("Ingrese nombre de la estación: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Dirección ===
    while (1) {
        printf("Ingrese dirección: ");
        fgets(direccion, sizeof(direccion), stdin);
        direccion[strcspn(direccion, "\n")] = '\0';

        if (strlen(direccion) == 0) {
            setcolor(1);
            printf("La dirección no puede estar vacía.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Teléfono ===
    while (1) {
        printf("Ingrese teléfono: ");
        fgets(telefono, sizeof(telefono), stdin);
        telefono[strcspn(telefono, "\n")] = '\0';

        if (strlen(telefono) == 0) {
            setcolor(1);
            printf("El teléfono no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Kilómetros esquiables ===
    while (1) {
        printf("Ingrese kilómetros esquiables: ");
        if (scanf("%f", &km_esquiables) != 1 || km_esquiables < 0) {
            setcolor(1);
            printf("Dato inválido. Debe ser un número decimal positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // === Personas de contacto ===
    while (1) {
        printf("Ingrese personas de contacto: ");
        fgets(personas_contacto, sizeof(personas_contacto), stdin);
        personas_contacto[strcspn(personas_contacto, "\n")] = '\0';

        if (strlen(personas_contacto) == 0) {
            setcolor(1);
            printf("El campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // Confirmación final
    printf("¿Deseas guardar esta estación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Insertar en la base de datos
    snprintf(query, sizeof(query),
             "INSERT INTO estacion (codigo, nombre, direccion, telefono, km_esquiables, personas_contacto) "
             "VALUES ('%s', '%s', '%s', '%s', %.2f, '%s')",
             codigo, nombre, direccion, telefono, km_esquiables, personas_contacto);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al insertar estación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        setcolor(2);
        printf("¡Estación creada exitosamente!\n");
        setcolor(0);
    }

    pausar();
}
void leer_estaciones() {
    mostrar_encabezado("LISTADO DE ESTACIONES DE ESQUÍ");
    gotoxy(1, 6);

    MYSQL_RES *result;
    MYSQL_ROW row;

    if (mysql_query(conn, "SELECT * FROM estacion ORDER BY nombre")) {
        setcolor(1);
        printf("Error al consultar estaciones: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        setcolor(1);
        printf("Error al obtener resultados: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    int total = mysql_num_rows(result);
    if (total == 0) {
        setcolor(3); // Amarillo
        printf("No hay estaciones registradas.\n");
        setcolor(0);
        mysql_free_result(result);
        pausar();
        return;
    }

    setcolor(4); // Azul
    printf("%-10s %-20s %-25s %-15s %-10s\n", "CÓDIGO", "NOMBRE", "DIRECCIÓN", "TELÉFONO", "KM_ESQUI");
    setcolor(0);
    printf("--------------------------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-10s %-20s %-25s %-15s %-10s\n",
               row[0] ? row[0] : " - ",
               row[1] ? row[1] : " - ",
               row[2] ? row[2] : " - ",
               row[3] ? row[3] : " - ",
               row[4] ? row[4] : " - ");
    }

    mysql_free_result(result);
    pausar();
}
void actualizar_estacion() {
    mostrar_encabezado("ACTUALIZAR ESTACIÓN DE ESQUÍ");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("estacion");

    char codigo[21], nombre[101], direccion[151], telefono[21], personas_contacto[151], query[1024], confirmacion;
    float km_esquiables = -1;

    // === Código de estación ===
    while (1) {
        printf("Ingrese código de la estación a actualizar: ");
        fgets(codigo, sizeof(codigo), stdin);
        codigo[strcspn(codigo, "\n")] = '\0';

        if (strlen(codigo) == 0) {
            setcolor(1);
            printf("El código no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT codigo FROM estacion WHERE codigo = '%s'", codigo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe una estación con ese código.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Nuevo nombre ===
    while (1) {
        printf("Ingrese nuevo nombre: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Nueva dirección ===
    while (1) {
        printf("Ingrese nueva dirección: ");
        fgets(direccion, sizeof(direccion), stdin);
        direccion[strcspn(direccion, "\n")] = '\0';

        if (strlen(direccion) == 0) {
            setcolor(1);
            printf("La dirección no puede estar vacía.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Nuevo teléfono ===
    while (1) {
        printf("Ingrese nuevo teléfono: ");
        fgets(telefono, sizeof(telefono), stdin);
        telefono[strcspn(telefono, "\n")] = '\0';

        if (strlen(telefono) == 0) {
            setcolor(1);
            printf("El teléfono no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Nuevos km esquiables ===
    while (1) {
        printf("Ingrese nuevos kilómetros esquiables: ");
        if (scanf("%f", &km_esquiables) != 1 || km_esquiables < 0) {
            setcolor(1);
            printf("Dato inválido. Debe ser un número decimal positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }
        break;
    }
    limpiar_buffer();

    // === Nuevas personas de contacto ===
    while (1) {
        printf("Ingrese nuevas personas de contacto: ");
        fgets(personas_contacto, sizeof(personas_contacto), stdin);
        personas_contacto[strcspn(personas_contacto, "\n")] = '\0';

        if (strlen(personas_contacto) == 0) {
            setcolor(1);
            printf("El campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // Confirmación
    printf("¿Deseas actualizar los datos de esta estación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar actualización
    snprintf(query, sizeof(query),
             "UPDATE estacion SET nombre='%s', direccion='%s', telefono='%s', km_esquiables=%.2f, personas_contacto='%s' WHERE codigo='%s'",
             nombre, direccion, telefono, km_esquiables, personas_contacto, codigo);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al actualizar estación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Estación actualizada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se actualizó ningún dato.\n");
            setcolor(0);
        }
    }

    pausar();
}
void eliminar_estacion() {
    mostrar_encabezado("ELIMINAR ESTACIÓN DE ESQUÍ");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("estacion");

    char codigo[21], query[256], confirmacion;

    // === Solicitar y validar código ===
    while (1) {
        printf("Ingrese código de la estación a eliminar: ");
        fgets(codigo, sizeof(codigo), stdin);
        codigo[strcspn(codigo, "\n")] = '\0';

        if (strlen(codigo) == 0) {
            setcolor(1);
            printf("El código no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        // Verificar existencia
        snprintf(query, sizeof(query), "SELECT codigo FROM estacion WHERE codigo = '%s'", codigo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe una estación con ese código.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // Confirmar eliminación
    printf("¿Estás seguro de eliminar esta estación? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar eliminación
    snprintf(query, sizeof(query), "DELETE FROM estacion WHERE codigo = '%s'", codigo);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al eliminar estación: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Estación eliminada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se encontró estación con código %s\n", codigo);
            setcolor(0);
        }
    }

    pausar();
}
int validar_formato_hora(const char *hora) {
    int h, m, s;
    if (sscanf(hora, "%2d:%2d:%2d", &h, &m, &s) != 3) return 0;
    return (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60);
}

void crear_prueba() {
    mostrar_encabezado("CREAR NUEVA PRUEBA");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("prueba");

    int id_prueba = -1;
    char nombre[101], tipo[20], participante_vencedor_tipo[20], participante_vencedor_id[51], tiempo[20];
    char query[1024], confirmacion;

    // === Validar ID ===
    while (1) {
        printf("Ingrese ID de la prueba: ");
        if (scanf("%d", &id_prueba) != 1 || id_prueba <= 0) {
            setcolor(1);
            printf("ID inválido. Intenta de nuevo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id_prueba FROM prueba WHERE id_prueba = %d", id_prueba);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("Ya existe una prueba con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // === Nombre ===
    while (1) {
        printf("Ingrese nombre de la prueba: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';
        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Tipo (fondo/slalom/salto) ===
    while (1) {
        printf("Ingrese tipo (fondo/slalom/salto): ");
        fgets(tipo, sizeof(tipo), stdin);
        tipo[strcspn(tipo, "\n")] = '\0';
        if (strlen(tipo) == 0) {
            setcolor(1);
            printf("El tipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Tipo de participante ===
    while (1) {
        printf("Ingrese tipo de participante vencedor (individual/equipo): ");
        fgets(participante_vencedor_tipo, sizeof(participante_vencedor_tipo), stdin);
        participante_vencedor_tipo[strcspn(participante_vencedor_tipo, "\n")] = '\0';
        if (strlen(participante_vencedor_tipo) == 0) {
            setcolor(1);
            printf("Este campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === ID del participante vencedor ===
    while (1) {
        printf("Ingrese ID del participante vencedor: ");
        fgets(participante_vencedor_id, sizeof(participante_vencedor_id), stdin);
        participante_vencedor_id[strcspn(participante_vencedor_id, "\n")] = '\0';
        if (strlen(participante_vencedor_id) == 0) {
            setcolor(1);
            printf("Este campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === Tiempo empleado ===
    while (1) {
        printf("Ingrese tiempo empleado (HH:MM:SS): ");
        fgets(tiempo, sizeof(tiempo), stdin);
        tiempo[strcspn(tiempo, "\n")] = '\0';
        if (!validar_formato_hora(tiempo)) {
            setcolor(1);
            printf("Formato inválido. Usa HH:MM:SS (ej. 01:30:45).\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // Confirmación
    printf("¿Deseas guardar esta prueba? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Inserción
    snprintf(query, sizeof(query),
        "INSERT INTO prueba (id_prueba, nombre, tipo, participante_vencedor_tipo, participante_vencedor_id, tiempo_empleado_participantevencedor) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s')",
        id_prueba, nombre, tipo, participante_vencedor_tipo, participante_vencedor_id, tiempo);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al insertar prueba: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        setcolor(2);
        printf("¡Prueba creada exitosamente!\n");
        setcolor(0);
    }

    pausar();
}
void leer_pruebas() {
    mostrar_encabezado("LISTADO DE PRUEBAS");
    gotoxy(1, 6);

    MYSQL_RES *result;
    MYSQL_ROW row;

    if (mysql_query(conn, "SELECT * FROM prueba ORDER BY id_prueba")) {
        setcolor(1);
        printf("Error al consultar pruebas: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        setcolor(1);
        printf("Error al obtener resultados: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    int total = mysql_num_rows(result);
    if (total == 0) {
        setcolor(3);
        printf("No hay pruebas registradas.\n");
        setcolor(0);
        mysql_free_result(result);
        pausar();
        return;
    }

    setcolor(4);
    printf("%-8s %-20s %-10s %-15s %-15s %-10s\n", "ID", "NOMBRE", "TIPO", "TIPO_GANADOR", "ID_GANADOR", "TIEMPO");
    setcolor(0);
    printf("-------------------------------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-8s %-20s %-10s %-15s %-15s %-10s\n",
               row[0] ? row[0] : " - ",
               row[1] ? row[1] : " - ",
               row[2] ? row[2] : " - ",
               row[3] ? row[3] : " - ",
               row[4] ? row[4] : " - ",
               row[5] ? row[5] : " - ");
    }

    mysql_free_result(result);
    pausar();
}
void actualizar_prueba() {
    mostrar_encabezado("ACTUALIZAR PRUEBA");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("prueba");

    int id_prueba = -1;
    char nombre[101], tipo[20], participante_vencedor_tipo[20], participante_vencedor_id[51], tiempo[20];
    char query[1024], confirmacion;

    // === Validar ID de prueba existente ===
    while (1) {
        printf("Ingrese ID de la prueba a actualizar: ");
        if (scanf("%d", &id_prueba) != 1 || id_prueba <= 0) {
            setcolor(1);
            printf("ID inválido. Debe ser un número entero positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id_prueba FROM prueba WHERE id_prueba = %d", id_prueba);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe prueba con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // === Leer y validar campos ===
    while (1) {
        printf("Ingrese nuevo nombre de la prueba: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';
        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    while (1) {
        printf("Ingrese nuevo tipo (fondo/slalom/salto): ");
        fgets(tipo, sizeof(tipo), stdin);
        tipo[strcspn(tipo, "\n")] = '\0';
        if (strlen(tipo) == 0) {
            setcolor(1);
            printf("El tipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    while (1) {
        printf("Ingrese nuevo tipo de participante vencedor (individual/equipo): ");
        fgets(participante_vencedor_tipo, sizeof(participante_vencedor_tipo), stdin);
        participante_vencedor_tipo[strcspn(participante_vencedor_tipo, "\n")] = '\0';
        if (strlen(participante_vencedor_tipo) == 0) {
            setcolor(1);
            printf("Este campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    while (1) {
        printf("Ingrese nuevo ID del participante vencedor: ");
        fgets(participante_vencedor_id, sizeof(participante_vencedor_id), stdin);
        participante_vencedor_id[strcspn(participante_vencedor_id, "\n")] = '\0';
        if (strlen(participante_vencedor_id) == 0) {
            setcolor(1);
            printf("Este campo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    while (1) {
        printf("Ingrese nuevo tiempo empleado (HH:MM:SS): ");
        fgets(tiempo, sizeof(tiempo), stdin);
        tiempo[strcspn(tiempo, "\n")] = '\0';
        if (!validar_formato_hora(tiempo)) {
            setcolor(1);
            printf("Formato inválido. Usa HH:MM:SS (ej. 01:30:45).\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // Confirmación final
    printf("¿Deseas actualizar los datos de esta prueba? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Ejecutar actualización
    snprintf(query, sizeof(query),
             "UPDATE prueba SET nombre='%s', tipo='%s', participante_vencedor_tipo='%s', "
             "participante_vencedor_id='%s', tiempo_empleado_participantevencedor='%s' WHERE id_prueba=%d",
             nombre, tipo, participante_vencedor_tipo, participante_vencedor_id, tiempo, id_prueba);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al actualizar prueba: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Prueba actualizada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se actualizó ningún dato.\n");
            setcolor(0);
        }
    }

    pausar();
}
void eliminar_prueba() {
    mostrar_encabezado("ELIMINAR PRUEBA");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("prueba");

    int id_prueba = -1;
    char query[256], confirmacion;

    // === Solicitar y validar ID ===
    while (1) {
        printf("Ingrese ID de la prueba a eliminar: ");
        if (scanf("%d", &id_prueba) != 1 || id_prueba <= 0) {
            setcolor(1);
            printf("ID inválido. Debe ser un número entero positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        // Verificar existencia
        snprintf(query, sizeof(query), "SELECT id_prueba FROM prueba WHERE id_prueba = %d", id_prueba);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No se encontró prueba con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmación
    printf("¿Estás seguro de eliminar esta prueba? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Eliminar prueba
    snprintf(query, sizeof(query), "DELETE FROM prueba WHERE id_prueba = %d", id_prueba);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al eliminar prueba: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Prueba eliminada exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se encontró prueba con ID %d\n", id_prueba);
            setcolor(0);
        }
    }

    pausar();
}
void crear_equipo() {
    mostrar_encabezado("CREAR NUEVO EQUIPO");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("equipo");

    char id_equipo[21], nombre[101], query[512], confirmacion;
    int id_federacion = -1;

    // === ID del equipo ===
    while (1) {
        printf("Ingrese ID del equipo: ");
        fgets(id_equipo, sizeof(id_equipo), stdin);
        id_equipo[strcspn(id_equipo, "\n")] = '\0';

        if (strlen(id_equipo) == 0) {
            setcolor(1);
            printf("El ID del equipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id_equipo FROM equipo WHERE id_equipo = '%s'", id_equipo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("Ya existe un equipo con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Nombre del equipo ===
    while (1) {
        printf("Ingrese nombre del equipo: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre del equipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === ID de federación ===
    while (1) {
        printf("Ingrese ID de federación: ");
        if (scanf("%d", &id_federacion) != 1 || id_federacion <= 0) {
            setcolor(1);
            printf("ID inválido. Debe ser un número entero positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id_federacion);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmación
    printf("¿Deseas guardar este equipo? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Inserción
    snprintf(query, sizeof(query),
             "INSERT INTO equipo (id_equipo, nombre, id_federacion) VALUES ('%s', '%s', %d)",
             id_equipo, nombre, id_federacion);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al insertar equipo: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        setcolor(2);
        printf("¡Equipo creado exitosamente!\n");
        setcolor(0);
    }

    pausar();
}
void leer_equipos() {
    mostrar_encabezado("LISTADO DE EQUIPOS");
    gotoxy(1, 6);

    MYSQL_RES *result;
    MYSQL_ROW row;

    const char* query = "SELECT e.id_equipo, e.nombre, f.nombre AS federacion "
                        "FROM equipo e "
                        "LEFT JOIN federacion f ON e.id_federacion = f.id "
                        "ORDER BY e.nombre";

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al consultar equipos: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    result = mysql_store_result(conn);
    if (result == NULL) {
        setcolor(1);
        printf("Error al obtener resultados: %s\n", mysql_error(conn));
        setcolor(0);
        pausar();
        return;
    }

    int total = mysql_num_rows(result);
    if (total == 0) {
        setcolor(3);
        printf("No hay equipos registrados.\n");
        setcolor(0);
        mysql_free_result(result);
        pausar();
        return;
    }

    setcolor(4);
    printf("%-15s %-25s %-20s\n", "ID_EQUIPO", "NOMBRE", "FEDERACIÓN");
    setcolor(0);
    printf("--------------------------------------------------------------\n");

    while ((row = mysql_fetch_row(result))) {
        printf("%-15s %-25s %-20s\n",
               row[0] ? row[0] : " - ",
               row[1] ? row[1] : " - ",
               row[2] ? row[2] : "Sin federación");
    }

    mysql_free_result(result);
    pausar();
}
void actualizar_equipo() {
    mostrar_encabezado("ACTUALIZAR EQUIPO");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("equipo");

    char id_equipo[21], nombre[101], query[512], confirmacion;
    int id_federacion = -1;

    // === ID del equipo a actualizar ===
    while (1) {
        printf("Ingrese ID del equipo a actualizar: ");
        fgets(id_equipo, sizeof(id_equipo), stdin);
        id_equipo[strcspn(id_equipo, "\n")] = '\0';

        if (strlen(id_equipo) == 0) {
            setcolor(1);
            printf("El ID del equipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id_equipo FROM equipo WHERE id_equipo = '%s'", id_equipo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe un equipo con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // === Nombre del equipo ===
    while (1) {
        printf("Ingrese nuevo nombre: ");
        fgets(nombre, sizeof(nombre), stdin);
        nombre[strcspn(nombre, "\n")] = '\0';

        if (strlen(nombre) == 0) {
            setcolor(1);
            printf("El nombre no puede estar vacío.\n");
            setcolor(0);
            continue;
        }
        break;
    }

    // === ID de federación ===
    while (1) {
        printf("Ingrese nuevo ID de federación: ");
        if (scanf("%d", &id_federacion) != 1 || id_federacion <= 0) {
            setcolor(1);
            printf("ID inválido. Debe ser un número entero positivo.\n");
            setcolor(0);
            limpiar_buffer();
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id FROM federacion WHERE id = %d", id_federacion);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No existe federación con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }
    limpiar_buffer();

    // Confirmación
    printf("¿Deseas actualizar este equipo? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    // Actualizar
    snprintf(query, sizeof(query),
             "UPDATE equipo SET nombre='%s', id_federacion=%d WHERE id_equipo='%s'",
             nombre, id_federacion, id_equipo);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al actualizar equipo: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Equipo actualizado exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se actualizó ningún equipo.\n");
            setcolor(0);
        }
    }

    pausar();
}
void eliminar_equipo() {
    mostrar_encabezado("ELIMINAR EQUIPO");
    gotoxy(1, 6);
    mostrar_ejemplos_tabla("equipo");

    char id_equipo[21], confirmacion;
    char query[256];

    while (1) {
        printf("Ingrese ID del equipo a eliminar: ");
        fgets(id_equipo, sizeof(id_equipo), stdin);
        id_equipo[strcspn(id_equipo, "\n")] = '\0';

        if (strlen(id_equipo) == 0) {
            setcolor(1);
            printf("El ID del equipo no puede estar vacío.\n");
            setcolor(0);
            continue;
        }

        snprintf(query, sizeof(query), "SELECT id_equipo FROM equipo WHERE id_equipo = '%s'", id_equipo);
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (mysql_num_rows(res) == 0) {
                mysql_free_result(res);
                setcolor(1);
                printf("No se encontró equipo con ese ID.\n");
                setcolor(0);
                continue;
            }
            mysql_free_result(res);
        }
        break;
    }

    // Confirmación
    printf("¿Estás seguro de eliminar este equipo? (s/n): ");
    confirmacion = getchar();
    limpiar_buffer();
    if (confirmacion != 's' && confirmacion != 'S') {
        setcolor(3);
        printf("Operación cancelada por el usuario.\n");
        setcolor(0);
        pausar();
        return;
    }

    snprintf(query, sizeof(query), "DELETE FROM equipo WHERE id_equipo='%s'", id_equipo);

    if (mysql_query(conn, query)) {
        setcolor(1);
        printf("Error al eliminar equipo: %s\n", mysql_error(conn));
        setcolor(0);
    } else {
        if (mysql_affected_rows(conn) > 0) {
            setcolor(2);
            printf("¡Equipo eliminado exitosamente!\n");
            setcolor(0);
        } else {
            setcolor(3);
            printf("No se encontró equipo con ID %s\n", id_equipo);
            setcolor(0);
        }
    }

    pausar();
    
}
void menu_federaciones() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ - FEDERACIONES");

        gotoxy(10, 7);  setcolor(6); printf("1. Crear federación");
        gotoxy(10, 8);  printf("2. Leer federaciones");
        gotoxy(10, 9);  printf("3. Actualizar federación");
        gotoxy(10, 10); printf("4. Eliminar federación");
        gotoxy(10, 12); printf("0. Regresar al menú principal");
        setcolor(0);

        gotoxy(10, 14); printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            gotoxy(10, 16); setcolor(1); printf("Entrada inválida. Usa números."); setcolor(0);
            pausar(); continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1: crear_federacion(); break;
            case 2: leer_federaciones(); break;
            case 3: actualizar_federacion(); break;
            case 4: eliminar_federacion(); break;
            case 0: break;
            default:
                gotoxy(10, 16); setcolor(1);
                printf("Opción no válida. Intenta otra vez.");
                setcolor(0); pausar();
        }
    } while (opcion != 0);
}
void menu_esquiadores() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ - ESQUIADORES");

        gotoxy(10, 7);  setcolor(6); printf("1. Crear esquiador");
        gotoxy(10, 8);  printf("2. Leer esquiadores");
        gotoxy(10, 9);  printf("3. Actualizar esquiador");
        gotoxy(10, 10); printf("4. Eliminar esquiador");
        gotoxy(10, 12); printf("0. Regresar al menú principal");
        setcolor(0);

        gotoxy(10, 14); printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            gotoxy(10, 16); setcolor(1); printf("Entrada inválida. Usa números."); setcolor(0);
            pausar(); continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1: crear_esquiador(); break;
            case 2: leer_esquiadores(); break;
            case 3: actualizar_esquiador(); break;
            case 4: eliminar_esquiador(); break;
            case 0: break;
            default:
                gotoxy(10, 16); setcolor(1);
                printf("Opción no válida. Intenta otra vez.");
                setcolor(0); pausar();
        }
    } while (opcion != 0);
}
void menu_estaciones() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ - ESTACIONES");

        gotoxy(10, 7);  setcolor(6); printf("1. Crear estación");
        gotoxy(10, 8);  printf("2. Leer estaciones");
        gotoxy(10, 9);  printf("3. Actualizar estación");
        gotoxy(10, 10); printf("4. Eliminar estación");
        gotoxy(10, 12); printf("0. Regresar al menú principal");
        setcolor(0);

        gotoxy(10, 14); printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            gotoxy(10, 16); setcolor(1); printf("Entrada inválida. Usa números."); setcolor(0);
            pausar(); continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1: crear_estacion(); break;
            case 2: leer_estaciones(); break;
            case 3: actualizar_estacion(); break;
            case 4: eliminar_estacion(); break;
            case 0: break;
            default:
                gotoxy(10, 16); setcolor(1);
                printf("Opción no válida. Intenta otra vez.");
                setcolor(0); pausar();
        }
    } while (opcion != 0);
}
void menu_pruebas() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ - PRUEBAS");

        gotoxy(10, 7);  setcolor(6); printf("1. Crear prueba");
        gotoxy(10, 8);  printf("2. Leer pruebas");
        gotoxy(10, 9);  printf("3. Actualizar prueba");
        gotoxy(10, 10); printf("4. Eliminar prueba");
        gotoxy(10, 12); printf("0. Regresar al menú principal");
        setcolor(0);

        gotoxy(10, 14); printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            gotoxy(10, 16); setcolor(1); printf("Entrada inválida. Usa números."); setcolor(0);
            pausar(); continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1: crear_prueba(); break;
            case 2: leer_pruebas(); break;
            case 3: actualizar_prueba(); break;
            case 4: eliminar_prueba(); break;
            case 0: break;
            default:
                gotoxy(10, 16); setcolor(1);
                printf("Opción no válida. Intenta otra vez.");
                setcolor(0); pausar();
        }
    } while (opcion != 0);
}
void menu_equipos() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ - EQUIPOS");

        gotoxy(10, 7);  setcolor(6); printf("1. Crear equipo");
        gotoxy(10, 8);  printf("2. Leer equipos");
        gotoxy(10, 9);  printf("3. Actualizar equipo");
        gotoxy(10, 10); printf("4. Eliminar equipo");
        gotoxy(10, 12); printf("0. Regresar al menú principal");
        setcolor(0);

        gotoxy(10, 14); printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            gotoxy(10, 16); setcolor(1); printf("Entrada inválida. Usa números."); setcolor(0);
            pausar(); continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1: crear_equipo(); break;
            case 2: leer_equipos(); break;
            case 3: actualizar_equipo(); break;
            case 4: eliminar_equipo(); break;
            case 0: break;
            default:
                gotoxy(10, 16); setcolor(1);
                printf("Opción no válida. Intenta otra vez.");
                setcolor(0); pausar();
        }
    } while (opcion != 0);
}
void menu_principal() {
    int opcion;

    do {
        limpiar_pantalla();
        mostrar_encabezado("MENÚ PRINCIPAL");

        gotoxy(10, 7); setcolor(6); printf("1. Gestionar Federaciones");
        gotoxy(10, 8); printf("2. Gestionar Esquiadores");
        gotoxy(10, 9); printf("3. Gestionar Estaciones de Esquí");
        gotoxy(10, 10); printf("4. Gestionar Pruebas");
        gotoxy(10, 11); printf("5. Gestionar Equipos");
        gotoxy(10, 13); printf("0. Salir del sistema");
        setcolor(0);

        gotoxy(10, 15);
        printf("Selecciona una opción: ");
        if (scanf("%d", &opcion) != 1) {
            limpiar_buffer();
            setcolor(1);
            gotoxy(10, 17); printf("Entrada inválida. Debe ser un número.");
            setcolor(0);
            pausar();
            continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1:
                menu_federaciones();
                break;
            case 2:
                menu_esquiadores();
                break;
            case 3:
                menu_estaciones();
                break;
            case 4:
                menu_pruebas();
                break;
            case 5:
                menu_equipos();
                break;
            case 0:
                gotoxy(10, 17);
                setcolor(3);
                printf("Saliendo del sistema...");
                setcolor(0);
                break;
            default:
                gotoxy(10, 17);
                setcolor(1);
                printf("Opción no válida. Intenta nuevamente.");
                setcolor(0);
                pausar();
        }

    } while (opcion != 0);
}

// ==================== FUNCIÓN PRINCIPAL ====================
int main() {
    setcolor(6);
    printf("Iniciando Sistema de Gestión de Olimpiadas de Invierno...\n");
    setcolor(0);
    conectar_bd();

    menu_principal();  // Arranca el sistema con todos los módulos disponibles

    desconectar_bd();  // Siempre cerrar conexión al final

    setcolor(2);
    printf("Sistema terminado correctamente.\n");
    setcolor(0);

    return 0;
}
