//servidor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <regex.h>
#include "database.h"

#define SERVER_PORT 3306
#define BUFFER_SIZE 1024

int es_keyword(const char* token) {
    const char* keywords[] = {"INSERT", "INTO", "VALUES", "(", ")", "alumnos","SET","DELETE","SELECT", "UPDATE", "WHERE", "FROM", "="};
    size_t num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < num_keywords; ++i) {
        if (strcmp(token, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Función para escribir el contenido del diccionario en un archivo en el formato especificado
void escribir_alumnos(struct Alumno diccionario[], int num_entries) {
    FILE *file = fopen("alumnos.txt", "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Escribir encabezado
    fprintf(file, "Id,Nombre,Apellido,Semestre,Carrera\n");

    // Escribir contenido del diccionario
    for (int i = 0; i < num_entries; i++) {
        fprintf(file, "%d,%s,%s,%d,%s\n", 
                diccionario[i].id, 
                diccionario[i].nombre, 
                diccionario[i].apellido, 
                diccionario[i].semestre, 
                diccionario[i].carrera);
    }

    fclose(file);
}



int main() {
    int sockfd, new_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    char buffer[BUFFER_SIZE];
    int n;


    // Crear Expresiones regulares para validar peticiones del cliente. 
    regex_t regex_select, regex_insert, regex_update, regex_delete;
    int regerr,regerr1,regerr2,regerr3;

    regerr = regcomp(&regex_select,  "^SELECT .+ FROM [A-z]+", REG_EXTENDED );
    regerr1 = regcomp(&regex_insert, "^INSERT INTO [A-z]+ VALUES .+", REG_EXTENDED );
    regerr2 = regcomp(&regex_update, "^UPDATE [A-z]+ SET .+ WHERE id = [0-9]+$", REG_EXTENDED );
    regerr3 = regcomp(&regex_delete, "^DELETE FROM [A-z]+ WHERE id = [0-9]+$", REG_EXTENDED );
    
/*    if (regerr == 0 && regerr1 == 0 && regerr2  == 0 && regerr3  == 0 )
    {
        printf("all regex compiled successfully\n");
    } else
    {
        printf(" regex compilation error\n" );
    }
*/

    //INICIARLIZAR BASE DE DATOS.
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

    char filebuffer[100];
    while (fgets(filebuffer, sizeof(filebuffer), archivo) != NULL) { // Leer línea por línea
        printf("%s", filebuffer); // Mostrar en la consola
    
        // Dividir la línea y guardar los datos en variables
        int id, semestre;
        char nombre[50], apellido[50], carrera[50];

        sscanf(filebuffer, "%d,%49[^,],%49[^,],%d,%49[^\n]", &id, nombre, apellido, &semestre, carrera);
        
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







    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    // Enlazar el socket a la dirección del servidor
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    // Escuchar por conexiones entrantes
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    printf("Esperando conexiones...\n");

    while(1) {
        sin_size = sizeof(struct sockaddr_in);
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        printf("Servidor: conexión aceptada de %s\n", inet_ntoa(client_addr.sin_addr));

        // Recibir y enviar datos al cliente
        while ((n = recv(new_sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            
            //Remover el caracter de nueva linea
            buffer[strcspn(buffer, "\n")] = 0;
            printf("Servidor: recibido '%s'\n", buffer);

            // Tomar solo la primera palabra (token)
            if (buffer != NULL) {

                printf("\n");
                
                // [!] Comparar el resultado con regex!     
                regerr  = regexec(&regex_select, buffer , 0, NULL, 0);
                regerr1 = regexec(&regex_insert, buffer , 0, NULL, 0); 
                regerr2 = regexec(&regex_update, buffer , 0, NULL, 0); 
                regerr3 = regexec(&regex_delete, buffer , 0, NULL, 0); 

                
                // Realizar accion de acuerdo con la regex que haga match
                if (regerr == 0) 
                {
                    printf("\nSelect\n");
                    char *token = strtok(buffer, " ");
                    char ** params[5];
                    int i = 0;
                    while(token != 0)
                    {
                        printf("%s, ",token);
                        //poblar el arreglo de parametros
                         if ( !es_keyword(token))
                        {
                           // Agregando cada parametro al arreglo de cadenas
                            params[i] = malloc(strlen(token) * sizeof(char*) ); 
                            strcpy(params[i], token);
                            printf("\nPARAMETROS: %s\n",params[i]);
                            i++;
                        }
                        //Iterar al siguiente token
                        token = strtok(0," ");
                    }                    

                    if (strcmp(params[0], "*") == 0) // Caso mas simple, SELECT *
                    {
                        printDiccionary(num_entries,diccionario);
                    } else
                    {
                        seleccionar(diccionario,params,i,num_entries);
                    }
                    
                    //[!] Liberar memoria
                    free(token);
                    for (int j = 0; j < i; j++) {
                        free(params[j]);
                    }


                } else if (regerr1 == 0)
                {
                    printf("\nInsert\n");

                    char *token = strtok(buffer, " ");
                    char* params[5];
                    int i = 0;
                    while(token != 0)
                    {
                        printf("%s, ",token);
                        //Poblar arreglo de parametros ignorando keywords.
                        if ( !es_keyword(token))
                        {
                           // Agregando cada parametro al arreglo de cadenas
                            params[i] = malloc(strlen(token) * sizeof(char*) ); 
                            strcpy(params[i], token);
                            //printf("\nPARAMETROS: %s\n",params[i]);
                            i++;
                        }
                        //Iterar al siguiente token
                        token = strtok(0," ");
                    }


                    int id =  max_id(diccionario, num_entries);
                    printDiccionary(num_entries,diccionario);
                    insertar_alumno(diccionario, &num_entries, id+1, params);
                    escribir_alumnos(diccionario, num_entries);
                    printDiccionary(num_entries,diccionario);

                    
                    //[!] Liberar memoria
                    free(token);
                    free(params[0]);
                    free(params[1]);

                } else if (regerr2 == 0)
                {
                    printf("\nUpdate\n");

                    char *token = strtok(buffer, " ");
                    char* params_update[3];
                    int id;
                    int i = 0;
                    while(token != 0 && !strcmp(token, "WHERE") == 0)
                    {
                        printf("%s, ",token);
                        //Poblar arreglo de parametros ignorando keywords.
                        if ( !es_keyword(token))
                        {
                           // Agregando cada parametro al arreglo de cadenas
                            params_update[i] = malloc(strlen(token) * sizeof(char*) ); 
                            strcpy(params_update[i], token);
                            //printf("\nPARAMETROS update: %s\n",params_update[i]);
                            i++;
                        }
                        //Iterar al siguiente token
                        token = strtok(0," ");
                    }

                    while(token != 0)
                    {
                        printf("%s, ",token);
                        //Poblar arreglo de parametros ignorando keywords.
                        if ( !es_keyword(token))
                        {
                           // Agregando cada parametro al arreglo de cadenas
                            id = atoi(token);
                            //printf("\nPARAMETROS id: %d\n",id);
                            i++;
                        }
                        //Iterar al siguiente token
                        token = strtok(0," ");
                    }


                    if (editar_campo_alumno(diccionario, num_entries,id, params_update[0], params_update[1])) {
                        escribir_alumnos(diccionario, num_entries);
                        printf("Nombre del alumno con ID %d editado con éxito.\n", id);
                    } else {
                        printf("Alumno con ID %d no encontrado o campo no válido.\n",id);
                    }
                    printDiccionary(num_entries,diccionario);

                     //[!] Liberar memoria
                    free(token);
                    free(params_update[0]);
                    free(params_update[1]);


                } else if (regerr3 == 0)
                {
                    printf("\nDelete\n");

                    // Se utiliza una nueva variable de token para la segunda iteración
                    char *token_delete = strtok(buffer, " ");
                    int id;
                    int i = 0;
                    while (token_delete != 0) {
                        printf("%s, ", token_delete);
                        // Poblar arreglo de parametros ignorando keywords.
                        if (!es_keyword(token_delete)) {
                            // Agregando id
                            id = atoi(token_delete);
                            i++;
                            printf("id= %d", id);
                        }
                        // Iterar al siguiente token
                        token_delete = strtok(0, " ");
                    }

                    if (eliminar_alumno(diccionario, &num_entries, id)) {
                        escribir_alumnos(diccionario, num_entries);
                        printf("Alumno con ID %d eliminado.\n", id);
                    } else {
                        printf("Alumno con ID %d no encontrado.\n", id);
                    }
                    printDiccionary(num_entries,diccionario);
                    

                    
                }

            }

            if (send(new_sockfd, buffer, n, 0) == -1) {
                perror("send");
                close(new_sockfd);
                break;
            }
        }

        if (n == 0) {
            printf("Servidor: conexión cerrada por el cliente.\n");
        } else if (n < 0) {
            perror("recv");
        }

        close(new_sockfd);
    }

    close(sockfd);
    return 0;
}
