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
    const char* keywords[] = {"INSERT", "INTO", "VALUES", "(", ")", "alumnos", "SET", "DELETE", "SELECT", "UPDATE", "WHERE", "FROM", "="};
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

    // Inicializar base de datos
    FILE *archivo;
    char line[MAX_LINE_LENGTH];
    struct Alumno diccionario[MAX_ENTRIES];
    int num_entries = 0;

    int id, semestre;
    char nombre[50], apellido[50], carrera[50];

    // Crear expresiones regulares para validar peticiones del cliente
    regex_t regex_select, regex_insert, regex_update, regex_delete, regex_salir;
    int regerr, regerr1, regerr2, regerr3, regerr4;

    regerr = regcomp(&regex_select,  "^SELECT . FROM [A-z]+", REG_EXTENDED );
    regerr1 = regcomp(&regex_insert, "^INSERT INTO [A-z]+ VALUES .+", REG_EXTENDED );
    regerr2 = regcomp(&regex_update, "^UPDATE [A-z]+ SET .+ WHERE id = [0-9]+$", REG_EXTENDED );
    regerr3 = regcomp(&regex_delete, "^DELETE FROM [A-z]+ WHERE id = [0-9]+$", REG_EXTENDED );
    regerr4 = regcomp(&regex_salir, "^SALIR$", REG_EXTENDED );

    if (regerr == 0 && regerr1 == 0 && regerr2 == 0 && regerr3 == 0 && regerr4 == 0) {
        printf("Todas las regex compiladas satisfactoriamente\n");
    } else {
        printf("ERROR: compilación de regex\n");
    }

    // Leer desde un archivo
    archivo = fopen("alumnos.txt", "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    char filebuffer[100];
    // Ignorar el encabezado
    fgets(filebuffer, sizeof(filebuffer), archivo);

    while (fgets(filebuffer, sizeof(filebuffer), archivo) != NULL) {
        printf("%s", filebuffer);
        sscanf(filebuffer, "%d,%49[^,],%49[^,],%d,%49[^\n]", &id, nombre, apellido, &semestre, carrera);
        diccionario[num_entries].id = id;
        strcpy(diccionario[num_entries].nombre, nombre);
        strcpy(diccionario[num_entries].apellido, apellido);
        diccionario[num_entries].semestre = semestre;
        strcpy(diccionario[num_entries].carrera, carrera);   
        num_entries++;
    }
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

    printf("\nEsperando conexiones...\n");

    int bandera = 1;
    FILE *send_stream;
    char *send_buffer;
    size_t send_size;

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        printf("Servidor: conexión aceptada de %s\n", inet_ntoa(client_addr.sin_addr));

        // Recibir y enviar datos al cliente
        while ((n = recv(new_sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[strcspn(buffer, "\n")] = 0;
            printf("Servidor: recibido '%s'\n", buffer);

            if (buffer != NULL) {
                printf("\n");

                // Comparar el resultado con regex
                regerr  = regexec(&regex_select, buffer, 0, NULL, 0);
                regerr1 = regexec(&regex_insert, buffer, 0, NULL, 0); 
                regerr2 = regexec(&regex_update, buffer, 0, NULL, 0); 
                regerr3 = regexec(&regex_delete, buffer, 0, NULL, 0);
                regerr4 = regexec(&regex_salir, buffer, 0, NULL, 0);

                // Realizar acción de acuerdo con la regex que haga match
                if (regerr == 0) {
                    printf("\nSelect en ejecución...\n");
                    char *token = strtok(buffer, " ");
                    char* params[2];
                    int i = 0;
                    while (token != 0) {
                        if (!es_keyword(token)) {
                            params[i] = malloc(strlen(token) * sizeof(char*)); 
                            strcpy(params[i], token);
                            i++;
                        }
                        token = strtok(0, " ");
                    }

                    if (strcmp(params[0], "*") == 0) {
                        bandera = 1;
                        printDiccionary(num_entries, diccionario);

                        // Imprimir a un buffer en memoria para mandar al cliente                
                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintDiccionary(num_entries, diccionario, send_stream);
                        fflush(send_stream);
                        fclose(send_stream);
                    } else {
                        // Consultas específicas
                    }

                    free(params[0]);
                    free(params[1]);

                } else if (regerr1 == 0) {
                    printf("\nInsert en ejecución...\n");

                    char *token = strtok(buffer, " ");
                    char* params[5];
                    int i = 0;
                    while (token != 0) {
                        if (!es_keyword(token)) {
                            params[i] = malloc(strlen(token) * sizeof(char*)); 
                            strcpy(params[i], token);
                            i++;
                        }
                        token = strtok(0, "(), ");
                    }

                    int id = max_id(diccionario, num_entries);
                    if (insertar_alumno(diccionario, &num_entries, id + 1, params)) {
                        escribir_alumnos(diccionario, num_entries);

                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintDiccionary(num_entries, diccionario, send_stream);
                        bandera = 1;
                    } else {
                        bandera = 0;
                    }

                    fflush(send_stream);
                    fclose(send_stream);

                    free(params[0]);
                    free(params[1]);

                } else if (regerr2 == 0) {
                    printf("\nUpdate en ejecución...\n");

                    char *token = strtok(buffer, " ");
                    char* params_update[5];
                    int id;
                    int i = 0;
                    while (token != 0) {
                        if (!es_keyword(token)) {
                            params_update[i] = malloc(strlen(token) * sizeof(char*)); 
                            strcpy(params_update[i], token);
                            i++;
                        }
                        token = strtok(0, "= ");
                    }

                    char* campo = params_update[1];
                    char* valor = params_update[2];
                    id = atoi(params_update[4]);

                    if (editar_campo_alumno(diccionario, num_entries, id, campo, valor)) {
                        escribir_alumnos(diccionario, num_entries);
                        printf("Nombre del alumno con ID %d editado con éxito.\n", id);

                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintf(send_stream, "Nombre del alumno con ID %d editado con éxito.\n", id);
                        fflush(send_stream);
                        fclose(send_stream);
                        bandera = 1;
                    } else {
                        bandera = 0;
                        printf("Alumno con ID %d no encontrado o campo no válido.\n", id);

                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintf(send_stream, "Alumno con ID %d no encontrado o campo no válido.\n", id);
                        fflush(send_stream);
                        fclose(send_stream);
                    }

                    free(token);
                    free(params_update[0]);
                    free(params_update[1]);

                } else if (regerr3 == 0) {
                    printf("\nDelete en ejecución...\n");

                    char *token_delete = strtok(buffer, " ");
                    int id;
                    int i = 0;
                    while (token_delete != 0) {
                        if (!es_keyword(token_delete)) {
                            id = atoi(token_delete);
                            i++;
                            printf("id= %d", id);
                        }
                        token_delete = strtok(0, " ");
                    }

                    if (eliminar_alumno(diccionario, &num_entries, id)) {
                        bandera = 1;
                        escribir_alumnos(diccionario, num_entries);
                        printf("Alumno con ID %d eliminado.\n", id);

                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintf(send_stream, "Alumno con ID %d eliminado.\n", id);
                        fflush(send_stream);
                        fclose(send_stream);
                    } else {
                        bandera = 0;
                        printf("Alumno con ID %d no encontrado.\n", id);

                        send_stream = open_memstream(&send_buffer, &send_size);
                        fprintf(send_stream, "Alumno con ID %d no encontrado.\n", id);
                        fflush(send_stream);
                        fclose(send_stream);
                    }

                    send_stream = open_memstream(&send_buffer, &send_size);
                    fprintDiccionary(num_entries, diccionario, send_stream);
                    fflush(send_stream);
                    fclose(send_stream);

                } else if (regexec(&regex_salir, buffer, 0, NULL, 0) == 0) {
                    close(new_sockfd);
                    close(sockfd);
                    return 0;
                }
            }

            const char *mensaje;
            int mensaje_len;

            if (bandera) {
                mensaje = "Exito";
            } else {
                mensaje = "Fallo";
            }

            mensaje_len = strlen(mensaje);

            if (send(new_sockfd, mensaje, mensaje_len, 0) == -1) {
                perror("send");
                close(new_sockfd);
            }

            if (send(new_sockfd, send_buffer, send_size, 0) == -1) {
                perror("send");
                close(new_sockfd);
                break;
            }

            // Clean buffers
            memset(send_buffer, 0, send_size);
            memset(buffer, 0, sizeof(buffer));
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
