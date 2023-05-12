#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softTone.h>
#include <lcd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "RFID/rfid_sensor.h"

// Définition des PINs
#define BuzzPin 0
#define FLOW_SENSOR_PIN 29
#define LCD_RS  20               //signal RS
#define LCD_E   19               //signal E
#define LCD_D4  13               //Donnée 1
#define LCD_D5  6                //Donnée 2
#define LCD_D6  5                //Donnée 3
#define LCD_D7  11               //Donnée 4
#define ELECTROVANNE_PIN 1

// Définition des constantes
#define LITRES_MAX 2

const char * capteurPath="/sys/bus/w1/devices";

// Définition des types
typedef char char16[16];
char16  * capteurTemperature;

// Variables Glogales pour le capteur de débit
int frequence_debit = 0;
long total_consomme = 0;

// Inclusion des fonctions des codes unitaires
void init_buzzer(void);
int getTemperature(char * capteur, float * temperature);
void capteur_debit_interruption();

void capteur_debit_interruption() 
{
  frequence_debit++;
  total_consomme++;
}

void init_buzzer(void) {
    // Initialisation du buzzer
    softToneCreate(BuzzPin);
    softToneWrite(BuzzPin, 0);
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

int main() {
    char commande[100];
    
    // Initialisation de la bibliothèque WiringPi
    if (wiringPiSetup() == -1) {
        fprintf(stdout, "Erreur d'initialisation de WiringPi\n");
        return 1;
    }

    // must be run as root to open /dev/mem in BMC2835
    if (geteuid() != 0)
    {
        p_printf(RED,"Must be run as root.\n");
        exit(1);
    }
    

    // Initialisation des capteurs et actionneurs
    pinMode(FLOW_SENSOR_PIN, INPUT);
    pullUpDnControl(FLOW_SENSOR_PIN, PUD_UP);
    wiringPiISR(FLOW_SENSOR_PIN, INT_EDGE_FALLING, &capteur_debit_interruption);
    
    init_buzzer();
    
    // Variables pour le capteur de température
    float temperature;
    int code_retour;

    // initialise Capteurs Température
    capteurTemperature = malloc(sizeof(char16));
    strcpy(capteurTemperature[0], "28-e6bccb0664ff");
        
    // Initialisation et Ouverture de l'électrovanne
    pinMode(ELECTROVANNE_PIN, OUTPUT);
    digitalWrite(ELECTROVANNE_PIN, HIGH);
    printf("Electrovanne Ouverte\n");
        
    float total = 0;
    float debit = 0;
    
        
    // Boucle de la douche
    while (total_consomme < (LITRES_MAX * 7.5 * 60)) 
    {
        char affichage[32];
        // Lecture du capteur de débit
        debit = ((float)frequence_debit / 7.5); // Convertir le débit en litre par minute
        total = ((float)total_consomme / 7.5 / 60); // Convertir le total en litre
        
        printf("Débit d'eau : %.2f L/min | Total : %.2f L\n", debit, total);
        frequence_debit = 0;
            
        // Lecture du capteur de température
        printf("Capteur %s : ", capteurTemperature[0]);
        fflush(stdout);
        code_retour = getTemperature(capteurTemperature[0], &temperature);
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
                printf("--- Error\n");
        }

        // Activation du buzzer 1 litre avant la fin de la douche
        if (total_consomme >= ((LITRES_MAX - 1) * 7.5 * 60)) {
            softToneWrite(BuzzPin, 200);
        }
            // Vérification du dépassement de la limite de consommation
        if (total_consomme >= (LITRES_MAX * 7.5 * 60)) {
            // Fermeture de l'électrovanne
            digitalWrite(ELECTROVANNE_PIN, LOW);

            // Arrêt du buzzer
            softToneStop(BuzzPin);

            // Affichage du message de fin de douche
            sprintf(affichage,"Douche terminee ! %.2f L", total);
            sprintf(commande, "./LCD/writeOnLcd '%s'", affichage);
            system(commande);

            // Réinitialisation des variables
            frequence_debit = 0;
            total_consomme = 0;
            temperature = 0;

            break;
        }
        // Mise à jour de l'affichage sur l'écran LCD
        sprintf(affichage,"%.2fL sur %dL   %6.2f Degres", total, LITRES_MAX, temperature);
        sprintf(commande, "./LCD/writeOnLcd '%s'", affichage);
        system(commande);
    }

    // Fermeture de l'électrovanne
    digitalWrite(ELECTROVANNE_PIN, LOW);
    printf("Electrovanne Fermée\n");

    // Arrêt du buzzer
    softToneStop(BuzzPin);
    
    //Libérer le capteur de Température
    free(capteurTemperature);

    return 0;
}
