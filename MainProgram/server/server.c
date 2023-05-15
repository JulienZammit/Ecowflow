#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080

void *client_handler(void *socket_desc) {
    int client_socket = *(int*)socket_desc;
    char buffer[1024] = {0};
    int valread;

    while ((valread = read(client_socket, buffer, sizeof(buffer)-1)) > 0) {
        printf("Message reçu du client : %s\n", buffer);

        // Écrire le message reçu dans un fichier de sauvegarde
        FILE *file = fopen("doucheSauvegarde.txt", "a");
        if (file != NULL) {
            fprintf(file, "%s\n", buffer);
            fclose(file);
            printf("Message sauvegardé dans le fichier.\n");
        } else {
            printf("Erreur lors de l'ouverture du fichier de sauvegarde.\n");
        }

        memset(buffer, 0, sizeof(buffer));
    }

    if (valread == 0) {
        printf("Connexion fermée par le client.\n");
    } else {
        perror("Erreur de lecture");
    }

    // Fermer la connexion client
    close(client_socket);

    // Libérer la structure socket_desc
    free(socket_desc);

    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket, *new_client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Créer le socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attacher le socket au port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Attacher le socket au port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Ecouter sur le socket
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de connexions...\n");

    while (1) {
        // Accepter la connexion entrante
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Nouvelle connexion acceptée.\n");

        // Créer une structure pour passer le socket client au thread
        new_client_socket = malloc(sizeof(*new_client_socket));
        *new_client_socket = new_socket;

        // Créer un thread pour gérer la connexion client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void*)new_client_socket) < 0) {
            perror("Erreur de création de thread");
            exit(EXIT_FAILURE);
        }

        // Détacher le thread
        pthread_detach(thread_id);
    }

    return 0;
}
