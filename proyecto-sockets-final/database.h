//ACTUALIZADO AL 14/05/24
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TANIA_DB_MAX_LINE_LENGTH 100
#define TANIA_DB_MAX_ENTRIES 100

// Definición de la estructura para el diccionario
struct Alumno{
    int id;
    char nombre[50];
    char apellido[50];
    int semestre;
    char carrera[50];
};

// Función para buscar un valor en el diccionario por id
void searchWhere(struct Alumno diccionario[], int size, int id) {
    printf("\nFunción search\n");
    for (int i = 0; i < size; i++) {
        if (diccionario[i].id== id) {
            printf("Encontrado\n");
            printf("ID: %d, Nombre: %s, Apellido: %s, Semestre: %d, Carrera: %s\n",
               diccionario[i].id, diccionario[i].nombre, diccionario[i].apellido,
               diccionario[i].semestre, diccionario[i].carrera);
        }
    }
    //return -1; // Si no se encuentra la clave, devolver -1
}

// Eliminar un elemento por id
int eliminar_alumno(struct Alumno diccionario[], int *num_entries, int id) {
    int encontrado = 0;

    for (int i = 0; i < *num_entries; i++) {
        if (diccionario[i].id == id) {
            encontrado = 1;
            // Mover todos los elementos posteriores un lugar hacia atrás
            for (int j = i; j < *num_entries - 1; j++) {
                diccionario[j] = diccionario[j + 1];
            }
            (*num_entries)--;
            break;
        }
    }

    return encontrado;
}


//Update 
// Editar un campo específico de un alumno en el diccionario por su ID
int editar_campo_alumno(struct Alumno diccionario[], int num_entries, int  id, const char* campo, const void* valor) {
    for (int i = 0; i < num_entries; i++) {
        if (diccionario[i].id == id) {
            if (strcmp(campo, "nombre") == 0) {
                strcpy(diccionario[i].nombre, (const char*)valor);
            } else if (strcmp(campo, "apellido") == 0) {
                strcpy(diccionario[i].apellido, (const char*)valor);
            } else if (strcmp(campo, "semestre") == 0) {
                diccionario[i].semestre = *((const int*)valor);
            } else if (strcmp(campo, "carrera") == 0) {
                strcpy(diccionario[i].carrera, (const char*)valor);
            } else {
                return 0; // Campo no encontrado
            }
            return 1; // Éxito
        }
    }
    return 0; // ID no encontrado
}


//Seleccionar uno o una lista de campos a mostrar
void seleccionar(struct Alumno alumno[], char **campos, int num_campos, int num_entries) {
     for (int j = 0; j < num_entries; j++) {
        printf("ID: %d\n", alumno[j].id);
        for (int i = 0; i < num_campos; i++) {
            if (strcmp(campos[i], "nombre") == 0) {
                printf("Nombre: %s\n", alumno[j].nombre);
            } else if (strcmp(campos[i], "apellido") == 0) {
                printf("Apellido: %s\n", alumno[j].apellido);
            } else if (strcmp(campos[i], "semestre") == 0) {
                printf("Semestre: %d\n", alumno[j].semestre);
            } else if (strcmp(campos[i], "carrera") == 0) {
                printf("Carrera: %s\n", alumno[j].carrera);
            } else {
                printf("Campo no válido: %s\n", campos[i]);
            }
        }
     }
    printf("\n");
}


// Insertar un nuevo alumno en el diccionario
int insertar_alumno(struct Alumno diccionario[], int *num_entries, int id,  char* parametros[]) {

    if (*num_entries >= TANIA_DB_MAX_ENTRIES) {
        // No hay espacio en el diccionario
        return 0;
    }

    // Copiar los parámetros a las variables locales
    char nombre[50];
    char apellido[50];
    int semestre;
    char carrera[50];

    strcpy(nombre, parametros[1]);
    strcpy(apellido, parametros[2]);
    semestre = atoi(parametros[3]);
    strcpy(carrera, parametros[4]);

    // Insertar el nuevo alumno al final del arreglo
    diccionario[*num_entries].id = id;
    strcpy(diccionario[*num_entries].nombre, nombre);
    strcpy(diccionario[*num_entries].apellido, apellido);
    diccionario[*num_entries].semestre = semestre;
    strcpy(diccionario[*num_entries].carrera, carrera);

    // Incrementar el contador de entradas
    (*num_entries)++;
    return 1; // Éxito
}

 // Mostrar el contenido del diccionario
void printDiccionary(int num_entries,struct Alumno *diccionario){
    printf("\nDiccionario:\n");
    for (int i = num_entries-1; i >= 0; i--) {
        printf("ID: %d, Nombre: %s, Apellido: %s, Semestre: %d, Carrera: %s\n",
                diccionario[i].id, 
                diccionario[i].nombre, 
                diccionario[i].apellido,
                diccionario[i].semestre, 
                diccionario[i].carrera
                );
    }
}

void fprintDiccionary(int num_entries, struct Alumno *diccionario, FILE* stream){
    fprintf(stream, "\nDiccionario:\n");
    for (int i = num_entries-1; i >= 0; i--) {

        fprintf(stream,"ID: %d Nombre: %s, Apellido: %s, Semestre: %d, Carrera: %s\n", 
                        diccionario[i].id, 
                        diccionario[i].nombre,
                        diccionario[i].apellido,
                        diccionario[i].semestre,
                        diccionario[i].carrera
                        );
    }
}

//Encontrar el id de mayor rango
int max_id(struct Alumno *diccionario, int num_entries) {
    // Variable para almacenar el mayor id
    int mayor_id = 0;

    // Iterar a través del arreglo para encontrar el mayor id
    for (int i = 1; i <= num_entries; i++) {
        if (diccionario[i].id > mayor_id) {
            mayor_id = diccionario[i].id;
        }
    }
    return mayor_id;
}
