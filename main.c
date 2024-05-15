#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_ENTRIES 100

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

//Eliminar un elemento por id
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
int editar_campo_alumno(struct Alumno diccionario[], int num_entries, int id, const char* campo, const void* valor) {
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
int insertar_alumno(struct Alumno diccionario[], int *num_entries, int id, const char* nombre, const char* apellido, int semestre, const char* carrera) {
    if (*num_entries >= MAX_ENTRIES) {
        // No hay espacio en el diccionario
        return 0;
    }

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
    for (int i = num_entries-1; i > 0; i--) {
        printf("ID: %d, Nombre: %s, Apellido: %s, Semestre: %d, Carrera: %s\n",
               diccionario[i].id, diccionario[i].nombre, diccionario[i].apellido,
               diccionario[i].semestre, diccionario[i].carrera);
    }
}

int main() {
    FILE *archivo;
    char line[MAX_LINE_LENGTH];
    struct Alumno diccionario[MAX_ENTRIES];
    int num_entries = 0;

    int id, semestre;
    char nombre[50],  apellido[50], carrera[50];

// Leer desde un archivo
    archivo = fopen("alumnos.txt", "r"); // Abrir el archivo en modo lectura
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    //Menu 

    //Leer buffer de socket
    
    //Guarda entrada de cliente





    char buffer[100];
    while (fgets(buffer, sizeof(buffer), archivo) != NULL) { // Leer línea por línea
        printf("%s", buffer); // Mostrar en la consola
    
        // Dividir la línea y guardar los datos en variables
        int id, semestre;
        char nombre[50], apellido[50], carrera[50];

        sscanf(buffer, "%d,%49[^,],%49[^,],%d,%49[^\n]", &id, nombre, apellido, &semestre, carrera);
        
        // Guardar los datos en el arreglo de estructuras
        diccionario[num_entries].id = id;
        strcpy(diccionario[num_entries].nombre, nombre);
        strcpy(diccionario[num_entries].apellido, apellido);
        diccionario[num_entries].semestre = semestre;
        strcpy(diccionario[num_entries].carrera, carrera);
        
        num_entries++;
    }


    // Cerrar el archivo
    fclose(archivo);
    //Mostrar el diccionario
    printDiccionary(num_entries,diccionario);

    //Buscar un elemento por id
    searchWhere(diccionario,num_entries,1);

    //Insertar elemento
    insertar_alumno(diccionario, &num_entries, 3, "Luis", "Martinez", 1, "Derecho");
    printDiccionary(num_entries,diccionario);

    //Editar
    // Editar el campo 'nombre' del alumno con ID 2
    if (editar_campo_alumno(diccionario, num_entries, 2, "nombre", "Ana Maria")) {
        printf("Nombre del alumno con ID 2 editado con éxito.\n");
    } else {
        printf("Alumno con ID 2 no encontrado o campo no válido.\n");
    }
    printDiccionary(num_entries,diccionario);

    //Borrar elemento por id
    int id_a_eliminar = 1;
    if (eliminar_alumno(diccionario, &num_entries, id_a_eliminar)) {
        printf("Alumno con ID %d eliminado.\n", id_a_eliminar);
    } else {
        printf("Alumno con ID %d no encontrado.\n", id_a_eliminar);
    }
    printDiccionary(num_entries,diccionario);

    //Seleccionar campos
    char ** campos[5];
    int campos_lenght;

    campos[0] = "nombre";
    campos[1] = "apellido";
    seleccionar(diccionario,campos,2,num_entries);
    printDiccionary(num_entries,diccionario);
    return 0;
}