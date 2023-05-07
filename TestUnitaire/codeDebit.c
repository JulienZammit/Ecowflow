#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <unistd.h>

#define FLOW_SENSOR_PIN 26   //GPIO 26 sur Raspberry Pi

int frequence_debit = 0;
long total_consomme;

void capteur_debit_interruption() 
{
  frequence_debit++;
  total_consomme++;
}

int main() 
{
    if (wiringPiSetup() == -1) 
    {
        fprintf (stdout, "Impossible d'initialiser wiringPi\n");
        return 1;
    }

    pinMode(FLOW_SENSOR_PIN, INPUT);
    pullUpDnControl(FLOW_SENSOR_PIN, PUD_UP);
    wiringPiISR(FLOW_SENSOR_PIN, INT_EDGE_FALLING, &capteur_debit_interruption);

    while (1) 
    {
        sleep(1); 
        float debit = ((float)frequence_debit / 7.5); // Convertir le débit en litre par minute
        float total = ((float)total_consomme / 7.5 / 60); // Convertir le total en litre

        printf("Débit d'eau : %.2f L/min | Total : %.2f L\n", debit, total);
        frequence_debit = 0;
    }

    return 0;
}
