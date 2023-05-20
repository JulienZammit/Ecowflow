#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 100
#define MAX_ENTRIES 100
#define SEPT_JOURS 7
#define TRENTE_JOURS 30

typedef struct {
  char date[11];
  double litres;
  double degres;
  int temps;
} DataEntry;

DataEntry entries[MAX_ENTRIES];

int mettreAJourDonnees() {
  FILE *file;
  char line[MAX_LINE_LENGTH];
  int numEntries = 0;

  // Ouvrir le fichier en mode lecture
  file = fopen("doucheSauvegarde.txt", "r");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier.\n");
    return numEntries;
  }

  // Lire les données ligne par ligne
  while (fgets(line, sizeof(line), file) != NULL) {
    // Découper la ligne en tokens en utilisant ":" comme délimiteur
    char *token;
    token = strtok(line, ":");
    // Découper la ligne en tokens en utilisant ":" comme délimiteur si la ligne n'est pas vide
    if (line[0] == '\n') {
      continue;
    }

    // Stocker les valeurs dans la structure DataEntry
    strncpy(entries[numEntries].date, token, sizeof(entries[numEntries].date));
    token = strtok(NULL, ":");
    entries[numEntries].litres = atof(token);
    token = strtok(NULL, ":");
    entries[numEntries].degres = atof(token);
    token = strtok(NULL, ":");
    entries[numEntries].temps = atoi(token);

    numEntries++;
  }

  // Fermer le fichier
  fclose(file);

  // Retourner le nombre de ligne
  return numEntries;
}

void displayMenu() {
  printf("Statistiques sur:\n");
  printf("1. Les dates\n");
  printf("2. Litres totals consommés\n");
  printf("3. Degrés moyens\n");
  printf("4. Temps de la douche\n");
  printf("5. Actualiser les données\n");
  printf("0. Quitter\n");
  printf("Choix : ");
}

int countShowers(DataEntry *entries, int numEntries, int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  int count = 0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    // Extraire les éléments de la date
    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    // Comparer les jours individuellement
    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      count++;
    }
  }

  return count;
}

double calculateAverageLitres(DataEntry *entries, int numEntries,
                              int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  double sum = 0.0;
  int count = 0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      sum += entries[i].litres;
      count++;
    }
  }

  if (count > 0) {
    return sum / count;
  } else {
    return 0.0;
  }
}

double calculateTotalLitres(DataEntry *entries, int numEntries,
                            int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  double sum = 0.0;
  int count = 0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      sum += entries[i].litres;
    }
  }

  return sum;
}

double calculateAverageDegres(DataEntry *entries, int numEntries,
                              int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  double sum = 0.0;
  int count = 0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      sum += entries[i].degres;
      count++;
    }
  }

  if (count > 0) {
    return sum / count;
  } else {
    return 0.0;
  }
}

double calculateAverageTime(DataEntry *entries, int numEntries,
                            int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  double sum = 0.0;
  double count = 0.0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      sum += entries[i].temps;
      count++;
    }
  }

  if (count > 0) {
    return sum / count;
  } else {
    return 0;
  }
}

int calculateTotalTime(DataEntry *entries, int numEntries, int nombreJours) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  int sum = 0;
  int count = 0;

  for (int i = 0; i < numEntries; i++) {
    int entry_day, entry_month, entry_year;

    sscanf(entries[i].date, "%d/%d/%d", &entry_day, &entry_month, &entry_year);

    if (entry_year == current_time->tm_year + 1900 &&
        entry_month == current_time->tm_mon + 1 &&
        entry_day >= current_time->tm_mday - nombreJours &&
        entry_day <= current_time->tm_mday) {
      sum += entries[i].temps;
    }
  }

  return sum;
}

int main() {

  int numEntries;

  numEntries = mettreAJourDonnees();

  // Afficher le menu et traiter le choix de l'utilisateur
  int choice = -1;
  int choicebis = -1;

  while (choice != 0) {
    displayMenu();
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      printf("Nombres de douches sur :\n");
      printf("1. Les 7 derniers jours\n");
      printf("2. Les 30 derniers jours\n");
      printf("Choix : ");
      scanf("%d", &choicebis);
      switch (choicebis) {
      case 1:
        printf("Nombre de douches : %d occurences\n",
               countShowers(entries, numEntries, SEPT_JOURS));
        printf("\n");
        break;
      case 2:
        printf("Nombre de douches : %d occurences\n",
               countShowers(entries, numEntries, TRENTE_JOURS));
        printf("\n");
        break;
      default:
        printf("Choix invalide. Veuillez réessayer.\n");
        break;
      }
      break;

    case 2:
      printf("Litres consommés:\n");
      printf("1. Sur les 7 derniers jours\n");
      printf("2. Sur 30 derniers jours\n");
      printf("Choix : ");
      scanf("%d", &choicebis);
      switch (choicebis) {
      case 1:
        printf("Nombre moyen de litres : %.2f L\n",
               calculateAverageLitres(entries, numEntries, SEPT_JOURS));
        printf("Nombre total de litres : %.2f L\n",
               calculateTotalLitres(entries, numEntries, SEPT_JOURS));
        printf("\n");
        break;
      case 2:
        printf("Nombre moyen de litres : %.2f L\n",
               calculateAverageLitres(entries, numEntries, TRENTE_JOURS));
        printf("Nombre total de litres : %.2f L\n",
               calculateTotalLitres(entries, numEntries, TRENTE_JOURS));
        printf("\n");
        break;
      default:
        printf("Choix invalide. Veuillez réessayer.\n");
        break;
      }
      break;

    case 3:
      printf("Degrés :\n");
      printf("1. Sur les 7 derniers jours\n");
      printf("2. Sur 30 derniers jours\n");
      printf("Choix : ");
      scanf("%d", &choicebis);
      switch (choicebis) {
      case 1:
        printf("Degrés moyen : %.2f °C\n",
               calculateAverageDegres(entries, numEntries, SEPT_JOURS));
        printf("\n");
        break;
      case 2:
        printf("Degrés moyen : %.2f °C\n",
               calculateAverageDegres(entries, numEntries, TRENTE_JOURS));
        printf("\n");
        break;
      default:
        printf("Choix invalide. Veuillez réessayer.\n");
        break;
      }
      break;
    case 4:
      printf("Temps sous la douche:\n");
      printf("1. Sur les 7 derniers jours\n");
      printf("2. Sur 30 derniers jours\n");
      printf("Choix : ");
      scanf("%d", &choicebis);
      switch (choicebis) {
      case 1:
        printf("Temps moyen : %.2f secondes\n",
               calculateAverageTime(entries, numEntries, SEPT_JOURS));
        printf("Temps total : %d secondes\n",
               calculateTotalTime(entries, numEntries, SEPT_JOURS));
        printf("\n");
        break;
      case 2:
        printf("Temps moyen : %.2f secondes\n",
               calculateAverageTime(entries, numEntries, TRENTE_JOURS));
        printf("Temps total : %d secondes\n",
               calculateTotalTime(entries, numEntries, TRENTE_JOURS));
        printf("\n");
        break;
      default:
        printf("Choix invalide. Veuillez réessayer.\n");
        break;
      }
      break;
    case 5:
      numEntries = mettreAJourDonnees();
      printf("Actualisation des données effectuée\n");
      break;
    }
  }

  return 0;
}

