#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef char char16[16];
char16  * capteurDebit;

const char * capteurPath="/sys/bus/w1/devices";

// Cherche dans /sys/bus/w1/devices 
// pour le capteur de température
// Trouve le fichier qui commmence par 28-
//Initialise le capteur trouvé
void trouveCapteur(void)
{
    DIR * dossier;
    struct dirent *dossierFlux;

    // ouvre le repertoire
    if( (dossier = opendir(capteurPath)) == NULL)
    {
        return;
    }
    while( (dossierFlux = readdir(dossier)) != NULL) 
    {
        if( strncmp(dossierFlux->d_name, "28-", 3) == 0)
        {
            strncpy(capteurDebit[0], dossierFlux->d_name, sizeof(char16)-1);
        }
    }
}

//Permet de retourner un code de retour
//  1 - Succès : température trouvée
//  2 - Erreur : erreur CRC
//  3 - Erreur : Valeur non trouvée
//Il initialise aussi la température en cas de succès
int getTemperature(char * capteur, float * temperature)
{
    char nomFichier[1024];
    char buffer[1024];
    int fhandle;
    int nbOctets = 0;
    char * pointeur;
    int retourCode;
    int iTemperature;

    *temperature = 0.0;
    sprintf(nomFichier, "%s/%s/w1_slave", capteurPath, capteur);
    fhandle = open(nomFichier, O_RDONLY);
    if(fhandle < 0) 
    {
        return 0;
    }
    nbOctets = read(fhandle, buffer, sizeof(buffer));
    close(fhandle);

    if( (nbOctets > 0)  && (nbOctets < 1024) )
    {
        // force dernier octet a etre null , au cas ou
        buffer[nbOctets] = 0;

        // On cherche le "YES" dans le buffer
        pointeur = strstr(buffer, "YES");
        if(pointeur == NULL)
        {
            return 2;
        }
        
        // On cherche le "t=" dans le buffer
        pointeur = strstr(buffer,"t=");
        if(pointeur == NULL)
        {
            return 3;
        }
        pointeur += 2;

        // converti la valeur ascii en entier
        retourCode = sscanf(pointeur, "%d", &iTemperature);
        if(retourCode == 0) 
        {
            return 0;
        }

        // converti la valeur en point flottant
        *temperature = (float) iTemperature / 1000.0;
        
        return 1;
    }

    return 0;
}




int main()
{
    float temperature;
    int code_retour;
    
    // initialise Capteurs
    capteurDebit = malloc(sizeof(char16));

    //cherche capteur
    trouveCapteur();

    //affiche temperature de chaque capteur
    while(1)
    {
        printf("Capteur %s : ", capteurDebit[0]);
        fflush(stdout);
        code_retour = getTemperature(capteurDebit[0], &temperature);
    
        switch(code_retour)
        {
            case 1 : 
                printf("%6.2f ° Celsius\n", temperature);
                break;
            case 2 : 
                printf("--- Erreur CRC\n");
                break;
            case 3 : 
                printf("--- Valeur non trouvée\n");
                break;
            default: 
                printf("---\n");
        }
        sleep(1);
    }
    free(capteurDebit);

    return 0;
}
