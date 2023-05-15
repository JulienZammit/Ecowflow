#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <nfc/nfc.h>

#define SS_PIN 8 // SDA
#define RST_PIN 25 // RST

int main()
{
    nfc_device *pnd;
    nfc_target nt;
    nfc_context *context;

    // Initialisation de WiringPi
    wiringPiSetup();
    pinMode(SS_PIN, OUTPUT);
    pinMode(RST_PIN, OUTPUT);

    // Initialisation de la communication SPI avec le capteur NFC
    if (wiringPiSPISetup(0, 1000000) < 0) {
        printf("Erreur lors de l'initialisation de la communication SPI\n");
        return -1;
    }

    // Initialisation de la bibliothèque NFC
    nfc_init(&context);
    if (context == NULL) {
        printf("Impossible d'initialiser la bibliothèque NFC\n");
        return -1;
    }

    // Ouverture de la communication avec le capteur NFC
    pnd = nfc_open(context, NULL);
    if (pnd == NULL) {
        printf("Impossible d'ouvrir la communication avec le capteur NFC\n");
        return -1;
    }

    // Configuration du capteur NFC
    if (nfc_initiator_init(pnd) < 0) {
        printf("Impossible de configurer le capteur NFC\n");
        nfc_close(pnd);
        nfc_exit(context);
        return -1;
    }

    // Boucle principale
    while (1) {
        // Attente de la détection d'un tag NFC
        if (nfc_initiator_select_passive_target(pnd, NM_ISO14443A_106, NULL, 0, &nt) > 0) {
            printf("Tag NFC détecté ! UID : ");
            for (int i = 0; i < nt.nti.nai.szUidLen; i++) {
                printf("%02X ", nt.nti.nai.abtUid[i]);
            }
            printf("\n");
            delay(1000);
        }
    }

    // Fermeture de la communication avec le capteur NFC
    nfc_close(pnd);
    nfc_exit(context);

    return 0;
}