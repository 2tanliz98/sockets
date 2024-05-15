#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "172.22.120.111"  // Dirección IP del servidor
#define SERVER_PORT 8080       // Puerto del servidor

int main(void)
{
    int s, t, len;
    struct sockaddr_in server_addr;
    char str[100];

    // Crear el socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    memset(&(server_addr.sin_zero), '\0', 8);

    // Conectar al servidor
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");

    // Bucle para enviar y recibir datos
    while(printf("> "), fgets(str, 100, stdin), !feof(stdin)) {
        // Enviar datos al servidor
        if (send(s, str, strlen(str), 0) == -1) {
            perror("send");
            exit(1);
        }

        // Recibir datos del servidor
        if ((t = recv(s, str, 100, 0)) > 0) {
            str[t] = '\0';
            printf("echo> %s", str);
        } else {
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }

    // Cerrar el socket
    close(s);

    return 0;
}
