#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd, new_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    char buffer[BUFFER_SIZE];
    int n;

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
            buffer[n] = '\0';
            printf("Servidor: recibido '%s'\n", buffer);
            printf("\n Buffer: %s\n",buffer[2]);

            // Dividir el string recibido en palabras clave
            char *token = strtok(buffer, " ");

            // Tomar solo la primera palabra (token)
            if (token != NULL) {
                // Imprimir la palabra recibida en formato de cadena y hexadecimal
                printf("Primera palabra recibida: '%s'\n", token);
                for (int i = 0; token[i] != '\0'; ++i) {
                    printf("%02x ", (unsigned char)token[i]);
                }
                printf("\n");
            }


            // Tomar solo la primera palabra (token)
            if (token != NULL) {
                
                // Imprimir el resultado de la comparación para depurar
                int cmp_result = strcasecmp(token, "select");
                //printf("Comparación con 'select': %d\n", cmp_result);

                // Aquí puedes realizar acciones basadas en la primera palabra
                if (cmp_result == 0) {
                    printf("\nSelect\n");
                    //la siguente palabra es * 
                    /*if(){

                    }
                    //es una lista de campos a mostrar 
                    else{
                        //mientras en el arreglo donde está almacenada la instrucción no aparezca un FROM
                        //deberá guardar los campos 

                    }*/
                    
                } else if (cmp_result == -10) {
                    printf("\nInsert\n");
                    //autoincremental
                    //buscar el id más alto para incrementar en uno su valor y evitar conflictos
                    //leer la cadena dentro de values()
                    //escribir en archivo
                } else if (cmp_result == 2) {
                    printf("\nUpdate\n");
                    //buscar el WHERE con ID y traer sus valores en diccionario 
                    //leer y guardar la cadena después de SET
                    //guarda el valor a SETear
                    //usa la variable key para cambiar el valor del diccionario
                    //eliminar el registro que había y escribirlo de nuevo
                } else if (cmp_result == -15) {
                    printf("\nDelete\n");
                } else {
                    // Palabra clave no reconocida, hacer algo por defecto
                    printf("\nComando no reconocido\n");
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
