#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <lcd.h>

#define LCD_RS  20               //signal RS
#define LCD_E   19               //signal E
#define LCD_D4  13               //Donnée 1
#define LCD_D5  6                //Donnée 2
#define LCD_D6  5                //Donnée 3
#define LCD_D7  11               //Donnée 4

int main(int argc, char *argv[]) 
{
    char texte[32];               // texte à afficher (maximum 32 caractères)

    if (wiringPiSetupGpio() == -1) 
    {
        fprintf (stdout, "Impossible d'initialiser wiringPi\n");
        return 1;
    }

    //Initialisation de l'écran LCD
    // 2 lignes
    // 16 colonnes
    // 4 bits
    int lcd = lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

    // Vérification que le programme a bien été appelé avec un argument
    if (argc != 2) {
        fprintf (stdout, "Usage: %s texte_a_afficher\n", argv[0]);
        return 1;
    }

    //Affichage du texte sur l'écran LCD
    snprintf(texte, 32, "%s", argv[1]);   // récupération de l'argument et copie dans la variable 'texte'
    lcdClear(lcd);              // effacement de l'écran LCD
    lcdPosition(lcd, 0, 0);     // positionnement du curseur sur la première ligne
    lcdPuts(lcd, texte);         // affichage du texte sur l'écran LCD

    return 0;
}
