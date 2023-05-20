#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILENAME "temp.txt" // Définition du nom du fichier temporaire

int main() {
    char buf[1024];
    int status;
    
    while(1){
        char commande[100];
        sprintf(commande, "./LCD/writeOnLcd 'Passez votre    carte'"); // Préparation de la commande pour afficher un message sur un écran LCD
        system(commande); // Exécution de la commande pour afficher le message sur l'écran LCD
        
        // Exécution du premier programme avec redirection de la sortie vers un fichier temporaire
        system("sudo ./RFID/Read_RFID > " FILENAME);

        // Lecture du contenu du fichier temporaire pour récupérer le message de fin du premier programme
        FILE* fp = popen("tail -1 " FILENAME, "r");
        fgets(buf, sizeof(buf), fp);
        pclose(fp);

        // Vérification du message de fin du premier programme
        if (strstr(buf, "Utilisateur reconnu") != NULL) {
           // Exécution du deuxième programme
           printf("\nCarte reconnue\n");
           system("sudo ./ecoflow");
        }else{
            printf("\nCarte Non reconnue\n");
        }

        // Suppression du fichier temporaire
        remove(FILENAME);
        sleep(1); // Attente de 1 seconde avant la prochaine itération
    }

    return 0;
}